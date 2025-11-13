#include "DeepRayCast3D.hpp"
using namespace godot;

#pragma region Constructor and Destructor
DeepRayCast3D::DeepRayCast3D()
{
    enabled             = true;
    auto_forward        = true;
    exclude_parent      = true;
    hit_back_faces      = true;
    hit_from_inside     = true;
    raycast_visible     = true;
    activate_emission   = true;
    collide_with_bodies = true;
    collide_with_areas  = false;
    margin              = 0.01f;
    radius              = 0.02f;
    opacity             = 0.7f;
    emission_energy     = 10.0f;
    forward_distance    = 10.0f;
    layers              = 1;
    collision_mask      = 1;
    rings               = 4;
    max_result          = 10;
    segments            = 64;
    color               = Color(1.0, 0.0, 0.0, 1.0);
    position_offset     = Vector3();

    _resource_material = ResourceLoader::get_singleton()->load("res://addons/deep_raycast_3d/resources/material.tres");
    if (!_resource_material.is_null())
    {
        Ref<Resource> duplicated = _resource_material->duplicate();
        _resource_material = duplicated;
    }

    _direction = Vector3();
    _distance = 0.0f;
    _excludes_rid = TypedArray<RID>();
    _deep_results = TypedArray<DeepRayCast3DResult>();

    _material = Ref<StandardMaterial3D>(memnew(StandardMaterial3D));
    _params = Ref<PhysicsRayQueryParameters3D>(memnew(PhysicsRayQueryParameters3D));
    _mesh = Ref<CylinderMesh>(memnew(CylinderMesh));
}

DeepRayCast3D::~DeepRayCast3D()
{
    _node_container = nullptr;
    _mesh_instance = nullptr;
}
#pragma endregion

#pragma region Private Methods
void DeepRayCast3D::_create_line()
{
    Ref<StandardMaterial3D> material = _material;
    material->set_emission(color);
    material->set_albedo(color);
    Color albedo = material->get_albedo();
    albedo.a = opacity;
    material->set_albedo(albedo);
    if (activate_emission)
    {
        material->set_emission(color);
        material->set_emission_energy_multiplier(emission_energy);
    }
    else
    {
        material->set_emission(Color(0, 0, 0));
        material->set_emission_energy_multiplier(0.0f);
    }

    Ref<CylinderMesh> mesh;
    mesh.instantiate();
    mesh->set_top_radius(radius);
    mesh->set_bottom_radius(radius);
    mesh->set_rings(rings);
    mesh->set_radial_segments(segments);
    mesh->set_height(_distance);
    mesh->set_material(material);
    _mesh = mesh;

    _node_container = memnew(Node3D);
    _node_container->set_name("RayCast3DNodeContainer");
    _node_container->set_visible(raycast_visible);
    add_child(_node_container);

    _mesh_instance = memnew(MeshInstance3D);
    _mesh_instance->set_mesh(mesh);
    _mesh_instance->set_cast_shadows_setting(GeometryInstance3D::SHADOW_CASTING_SETTING_OFF);
    _mesh_instance->set_rotation_degrees(Vector3(-90.0, 0.0, 0.0));
    _mesh_instance->set_position(Vector3(0, 0, _distance / -2.0f));
    _mesh_instance->set_layer_mask(layers);

    _node_container->add_child(_mesh_instance);
}

void DeepRayCast3D::_verify_mesh()
{
    Node *parent_node = get_parent();
    if (parent_node == nullptr || !Object::cast_to<Node3D>(parent_node))
    {
        if (_mesh_instance)
        {
            _mesh_instance->set_visible(false);
        }
        return;
    }

    if (!auto_forward && (_to == nullptr || parent_node == _to))
    {
        if (_mesh_instance)
        {
            _mesh_instance->set_visible(false);
        }
    }
    else
    {
        if (_mesh_instance)
        {
            _mesh_instance->set_visible(true);
        }
    }
}

void DeepRayCast3D::_update_line()
{
    Node *parent_node = get_parent();
    if (parent_node == nullptr || !Object::cast_to<Node3D>(parent_node))
    {
        return;
    }

    Node3D *parent = Object::cast_to<Node3D>(parent_node);

    Vector3 start_position = parent->to_global(position_offset);
    Vector3 target_position;

    if (auto_forward)
    {
        target_position = start_position + (parent->get_global_transform().basis.get_column(2) * -forward_distance);
    }
    else
    {
        if (_to == nullptr || parent == _to)
        {
            return;
        }
        target_position = _to->get_global_position();
    }

    _distance = start_position.distance_to(target_position);
    _direction = (target_position - start_position).normalized();

    if (_mesh.is_valid())
    {
        _mesh->set_height(_distance);
        _mesh->set_top_radius(radius);
        _mesh->set_bottom_radius(radius);
    }

    if (_mesh_instance)
    {
        _mesh_instance->set_position(Vector3(0, 0, _distance / -2.0f));
    }

    if (_node_container)
    {
        Transform3D transform = _node_container->get_global_transform();
        transform.origin = start_position;
        _node_container->set_global_transform(transform);
        _node_container->look_at(start_position + _direction, Vector3(0, 1, 0));
        _node_container->set_visible(raycast_visible);
    }

    if (_material.is_valid())
    {
        _material->set_albedo(color);
        Color albedo = _material->get_albedo();
        albedo.a = opacity;
        _material->set_albedo(albedo);

        if (activate_emission)
        {
            _material->set_emission(color);
            _material->set_emission_energy_multiplier(emission_energy);
        }
        else
        {
            _material->set_emission(Color(0, 0, 0));
            _material->set_emission_energy_multiplier(0.0f);
        }
    }
}

void DeepRayCast3D::_update_raycast()
{
    if (Engine::get_singleton()->is_editor_hint())
        return;

    _deep_results.clear();

    if (!enabled)
        return;

    Node3D *parent = Object::cast_to<Node3D>(get_parent());
    if (!parent)
        return;

    Vector3 from = parent->to_global(position_offset);
    Vector3 target_position;

    if (auto_forward)
    {
        target_position = from + (parent->get_global_transform().basis.get_column(2) * -forward_distance);
    }
    else
    {
        if (_to == nullptr || get_parent() == _to)
            return;
        target_position = _to->get_global_position();
    }

    Vector3 to_dir = (target_position - from).normalized();
    float remaining_distance = from.distance_to(target_position);

    Ref<World3D> world = parent->get_world_3d();
    if (world.is_null())
        return;

    PhysicsDirectSpaceState3D *space_state = world->get_direct_space_state();
    if (space_state == nullptr)
        return;

    TypedArray<RID> base_excludes;
    for (int i = 0; i < _excludes_rid.size(); i++)
        base_excludes.push_back(_excludes_rid[i]);

    const float min_margin = 0.05f;

    for (int i = 0; i < max_result; i++)
    {
        if (remaining_distance <= 0.0f)
            break;

        Vector3 to_point = from + to_dir * remaining_distance;

        Ref<PhysicsRayQueryParameters3D> params;
        params.instantiate();
        params->set_from(from);
        params->set_to(to_point);
        params->set_collide_with_areas(collide_with_areas);
        params->set_collide_with_bodies(collide_with_bodies);
        params->set_collision_mask(collision_mask);
        params->set_exclude(base_excludes);
        params->set_hit_back_faces(hit_back_faces);
        params->set_hit_from_inside(hit_from_inside);

        Dictionary hit = space_state->intersect_ray(params);

        if (hit.is_empty())
            break;

        PhysicsBody3D *collider_body = Object::cast_to<PhysicsBody3D>(hit["collider"]);
        int collider_id = (int)hit["collider_id"];
        Vector3 normal = (Vector3)hit["normal"];
        Vector3 position = (Vector3)hit["position"];
        int face_index = (int)hit["face_index"];
        RID rid = (RID)hit["rid"];
        int shape = (int)hit["shape"];

        Ref<DeepRayCast3DResult> result;
        result.instantiate();
        result->set_collider(collider_body);
        result->set_collider_id(collider_id);
        result->set_normal(normal);
        result->set_position(position);
        result->set_face_index(face_index);
        result->set_rid(rid);
        result->set_shape(shape);

        _deep_results.append(result);

        if (collider_body)
        {
            RID rid_exclude;
            if (collider_body->has_method("get_rid"))
                rid_exclude = collider_body->call("get_rid");
            else
                rid_exclude = rid;

            base_excludes.push_back(rid_exclude);
        }

        from = position + to_dir * min_margin;
        remaining_distance = target_position.distance_to(from);
    }

    if (_deep_results.size() > 0)
        emit_signal("cast_collider", _deep_results);
}

#pragma endregion

#pragma region Public Methods
int DeepRayCast3D::get_collider_count()
{
    return _deep_results.size();
}

PhysicsBody3D *DeepRayCast3D::get_collider(int p_value)
{
    Ref<DeepRayCast3DResult> ref = _deep_results[p_value];
    return ref->collider;
}

Vector3 DeepRayCast3D::get_normal(int p_value)
{
    Ref<DeepRayCast3DResult> ref = _deep_results[p_value];
    return ref->normal;
}

Vector3 DeepRayCast3D::get_position(int p_value)
{
    Ref<DeepRayCast3DResult> ref = _deep_results[p_value];
    return ref->position;
}

void DeepRayCast3D::add_exclude(PhysicsBody3D *p_value)
{
    if (p_value == nullptr)
    {
        return;
    }

    if (!p_value->has_method("get_rid"))
    {
        return;
    }

    Variant rid_var = p_value->call("get_rid");
    if (rid_var.get_type() != Variant::RID)
    {
        return;
    }

    RID rid = rid_var;

    for (int i = 0; i < _excludes_rid.size(); i++)
    {
        if ((RID)_excludes_rid[i] == rid)
        {
            return; // já existe
        }
    }

    _excludes_rid.append(rid);

    if (_params.is_valid())
    {
        _params->set_exclude(_excludes_rid);
    }
}

void DeepRayCast3D::remove_exclude(PhysicsBody3D *p_value)
{
    if (p_value == nullptr || !p_value->has_method("get_rid"))
    {
        return;
    }

    Variant rid_var = p_value->call("get_rid");
    if (rid_var.get_type() != Variant::RID)
    {
        return;
    }

    RID rid = rid_var;

    TypedArray<RID> new_excludes;
    for (int i = 0; i < _excludes_rid.size(); i++)
    {
        RID current = _excludes_rid[i];
        if (current != rid)
        {
            new_excludes.append(current);
        }
    }

    _excludes_rid = new_excludes;

    if (_params.is_valid())
    {
        _params->set_exclude(_excludes_rid);
    }
}

void DeepRayCast3D::clear_exclude()
{
    _excludes_rid.clear();

    if (_params.is_valid())
    {
        _params->set_exclude(_excludes_rid);
    }
}
#pragma endregion

#pragma region Engine Methods
void DeepRayCast3D::_ready()
{
    if (!to_path.is_empty())
    {
        Node *n = get_node_or_null(to_path);
        _to = Object::cast_to<Node3D>(n);
    }

    if (_resource_material.is_valid())
    {
        _material = _resource_material->duplicate();
    }

    if (_params.is_valid())
    {
        _params->set_exclude(_excludes_rid);
    }

    if (exclude_parent)
    {
        PhysicsBody3D *parent_node = Object::cast_to<PhysicsBody3D>(get_parent());
        if (parent_node)
        {
            add_exclude(parent_node);
        }
    }

    _create_line();
    _update_line();
    _verify_mesh();
}

void DeepRayCast3D::_physics_process(double delta)
{
    _update_line();
    _update_raycast();
    _verify_mesh();
}

void DeepRayCast3D::_enter_tree()
{
    update_configuration_warnings();
}

PackedStringArray DeepRayCast3D::_get_configuration_warnings() const
{
    PackedStringArray warnings;

    Node *parent_node = get_parent();
    if (parent_node == nullptr || !Object::cast_to<Node3D>(parent_node))
    {
        warnings.push_back("The parent of DeepRayCast3D must be a 3D node.");
    }

    if (!auto_forward)
    {
        if (_to == nullptr)
        {
            warnings.push_back("The to_path property cannot be null when Auto Forward is disabled.");
        }
        else if (parent_node == _to)
        {
            warnings.push_back("The to_path property cannot be the same as the parent node.");
        }
    }

    return warnings;
}
#pragma endregion

#pragma region Getters and Setters
void DeepRayCast3D::set_enabled(bool p_enabled)
{
    enabled = p_enabled;
}
bool DeepRayCast3D::get_enabled()
{
    return enabled;
}

void DeepRayCast3D::set_margin(float p_margin)
{
    margin = p_margin;
}
float DeepRayCast3D::get_margin()
{
    return margin;
}

void DeepRayCast3D::set_max_result(int p_max_result)
{
    max_result = p_max_result;
}
int DeepRayCast3D::get_max_result()
{
    return max_result;
}

void DeepRayCast3D::set_activate_emission(bool p_activate_emission)
{
    activate_emission = p_activate_emission;
}
bool DeepRayCast3D::get_activate_emission()
{
    return activate_emission;
}

void DeepRayCast3D::set_emission_energy(float p_emission_energy)
{
    emission_energy = p_emission_energy;
}
float DeepRayCast3D::get_emission_energy()
{
    return emission_energy;
}

void DeepRayCast3D::set_rings(int p_rings)
{
    rings = p_rings;
    if (_mesh.is_valid())
    {
        _mesh->set_rings(rings);
    }
}
int DeepRayCast3D::get_rings()
{
    return rings;
}

void DeepRayCast3D::set_segments(int p_segments)
{
    segments = p_segments;
    if (_mesh.is_valid())
    {
        _mesh->set_radial_segments(segments);
    }
}
int DeepRayCast3D::get_segments()
{
    return segments;
}

void DeepRayCast3D::set_auto_forward(bool p_auto_forward)
{
    auto_forward = p_auto_forward;
    update_configuration_warnings();
}
bool DeepRayCast3D::get_auto_forward()
{
    return auto_forward;
}

void DeepRayCast3D::set_forward_distance(float p_forward_distance)
{
    forward_distance = p_forward_distance;
}
float DeepRayCast3D::get_forward_distance()
{
    return forward_distance;
}

void DeepRayCast3D::set_to_path(NodePath p_to_path)
{
    to_path = p_to_path;
    Node *n = get_node_or_null(to_path);
    _to = Object::cast_to<Node3D>(n);
    update_configuration_warnings();
}
NodePath DeepRayCast3D::get_to_path()
{
    return to_path;
}

void DeepRayCast3D::set_exclude_parent(bool p_exclude_parent)
{
    exclude_parent = p_exclude_parent;
    PhysicsBody3D *node = Object::cast_to<PhysicsBody3D>(get_parent());

    if (node)
    {
        if (exclude_parent)
        {
            add_exclude(node);
        }
        else
        {
            remove_exclude(node);
        }
    }
}
bool DeepRayCast3D::get_exclude_parent()
{
    return exclude_parent;
}

void DeepRayCast3D::set_collide_with_bodies(bool p_value)
{
    collide_with_bodies = p_value;
}
bool DeepRayCast3D::get_collide_with_bodies()
{
    return collide_with_bodies;
}

void DeepRayCast3D::set_collide_with_areas(bool p_value)
{
    collide_with_areas = p_value;
}
bool DeepRayCast3D::get_collide_with_areas()
{
    return collide_with_areas;
}

void DeepRayCast3D::set_hit_back_faces(bool p_value)
{
    hit_back_faces = p_value;
}
bool DeepRayCast3D::get_hit_back_faces()
{
    return hit_back_faces;
}

void DeepRayCast3D::set_hit_from_inside(bool p_value)
{
    hit_from_inside = p_value;
}
bool DeepRayCast3D::get_hit_from_inside()
{
    return hit_from_inside;
}

void DeepRayCast3D::set_collision_mask(int p_value)
{
    collision_mask = p_value;
}
int DeepRayCast3D::get_collision_mask()
{
    return collision_mask;
}

void DeepRayCast3D::set_raycast_visible(bool p_value)
{
    raycast_visible = p_value;
    if (_node_container)
    {
        _node_container->set_visible(raycast_visible);
    }
}
bool DeepRayCast3D::get_raycast_visible()
{
    return raycast_visible;
}

void DeepRayCast3D::set_color(Color p_value)
{
    color = p_value;
}
Color DeepRayCast3D::get_color()
{
    return color;
}

void DeepRayCast3D::set_radius(float p_value)
{
    radius = p_value;
}
float DeepRayCast3D::get_radius()
{
    return radius;
}

void DeepRayCast3D::set_opacity(float p_value)
{
    opacity = p_value;
}
float DeepRayCast3D::get_opacity()
{
    return opacity;
}

void DeepRayCast3D::set_layers(int p_value)
{
    layers = p_value;
    if (_mesh_instance)
    {
        _mesh_instance->set_layer_mask(layers);
    }
}
int DeepRayCast3D::get_layers()
{
    return layers;
}

void DeepRayCast3D::set_position_offset(Vector3 p_value)
{
    position_offset = p_value;
    if (_node_container)
    {
        _update_line();
    }
}
Vector3 DeepRayCast3D::get_position_offset()
{
    return position_offset;
}
#pragma endregion

#pragma region Register
void DeepRayCast3D::_bind_methods()
{
    ADD_GROUP("Deep RayCast3D", "");

    ADD_SUBGROUP("Process", "");
    ClassDB::bind_method(D_METHOD("set_enabled", "p_enabled"), &DeepRayCast3D::set_enabled);
    ClassDB::bind_method(D_METHOD("get_enabled"), &DeepRayCast3D::get_enabled);
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "enabled", PROPERTY_HINT_NONE, ""), "set_enabled", "get_enabled");

    ClassDB::bind_method(D_METHOD("set_margin", "p_margin"), &DeepRayCast3D::set_margin);
    ClassDB::bind_method(D_METHOD("get_margin"), &DeepRayCast3D::get_margin);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "margin", PROPERTY_HINT_RANGE, "0.01, 1.0, 0.01, or_greater, suffix:m"), "set_margin", "get_margin");

    ClassDB::bind_method(D_METHOD("set_max_result", "p_max_result"), &DeepRayCast3D::set_max_result);
    ClassDB::bind_method(D_METHOD("get_max_result"), &DeepRayCast3D::get_max_result);
    ADD_PROPERTY(PropertyInfo(Variant::INT, "max_result", PROPERTY_HINT_RANGE, "1, 32, 1"), "set_max_result", "get_max_result");

    ADD_SUBGROUP("Emission", "");
    ClassDB::bind_method(D_METHOD("set_activate_emission", "p_activate_emission"), &DeepRayCast3D::set_activate_emission);
    ClassDB::bind_method(D_METHOD("get_activate_emission"), &DeepRayCast3D::get_activate_emission);
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "activate_emission", PROPERTY_HINT_NONE, ""), "set_activate_emission", "get_activate_emission");

    ClassDB::bind_method(D_METHOD("set_emission_energy", "p_emission_energy"), &DeepRayCast3D::set_emission_energy);
    ClassDB::bind_method(D_METHOD("get_emission_energy"), &DeepRayCast3D::get_emission_energy);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "emission_energy", PROPERTY_HINT_RANGE, "0.0, 10.0, 0.01, or_greater"), "set_emission_energy", "get_emission_energy");

    ClassDB::bind_method(D_METHOD("set_rings", "p_rings"), &DeepRayCast3D::set_rings);
    ClassDB::bind_method(D_METHOD("get_rings"), &DeepRayCast3D::get_rings);
    ADD_PROPERTY(PropertyInfo(Variant::INT, "rings", PROPERTY_HINT_RANGE, "3, 10, 1"), "set_rings", "get_rings");

    ClassDB::bind_method(D_METHOD("set_segments", "p_segments"), &DeepRayCast3D::set_segments);
    ClassDB::bind_method(D_METHOD("get_segments"), &DeepRayCast3D::get_segments);
    ADD_PROPERTY(PropertyInfo(Variant::INT, "segments", PROPERTY_HINT_RANGE, "4, 64, 4"), "set_segments", "get_segments");

    ADD_SUBGROUP("Interaction", "");
    ClassDB::bind_method(D_METHOD("set_auto_forward", "p_auto_forward"), &DeepRayCast3D::set_auto_forward);
    ClassDB::bind_method(D_METHOD("get_auto_forward"), &DeepRayCast3D::get_auto_forward);
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "auto_forward", PROPERTY_HINT_NONE, ""), "set_auto_forward", "get_auto_forward");

    ClassDB::bind_method(D_METHOD("set_forward_distance", "p_forward_distance"), &DeepRayCast3D::set_forward_distance);
    ClassDB::bind_method(D_METHOD("get_forward_distance"), &DeepRayCast3D::get_forward_distance);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "forward_distance", PROPERTY_HINT_RANGE, "0.1, 100.0, 0.1, or_greater"), "set_forward_distance", "get_forward_distance");

    ClassDB::bind_method(D_METHOD("set_to_path", "p_to_path"), &DeepRayCast3D::set_to_path);
    ClassDB::bind_method(D_METHOD("get_to_path"), &DeepRayCast3D::get_to_path);
    ADD_PROPERTY(PropertyInfo(Variant::NODE_PATH, "to_path"), "set_to_path", "get_to_path");

    ClassDB::bind_method(D_METHOD("set_exclude_parent", "p_exclude_parent"), &DeepRayCast3D::set_exclude_parent);
    ClassDB::bind_method(D_METHOD("get_exclude_parent"), &DeepRayCast3D::get_exclude_parent);
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "exclude_parent", PROPERTY_HINT_NONE, ""), "set_exclude_parent", "get_exclude_parent");

    ADD_SUBGROUP("Physics", "");
    ClassDB::bind_method(D_METHOD("set_collide_with_bodies", "p_value"), &DeepRayCast3D::set_collide_with_bodies);
    ClassDB::bind_method(D_METHOD("get_collide_with_bodies"), &DeepRayCast3D::get_collide_with_bodies);
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "collide_with_bodies", PROPERTY_HINT_NONE, ""), "set_collide_with_bodies", "get_collide_with_bodies");

    ClassDB::bind_method(D_METHOD("set_collide_with_areas", "p_value"), &DeepRayCast3D::set_collide_with_areas);
    ClassDB::bind_method(D_METHOD("get_collide_with_areas"), &DeepRayCast3D::get_collide_with_areas);
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "collide_with_areas", PROPERTY_HINT_NONE, ""), "set_collide_with_areas", "get_collide_with_areas");

    ClassDB::bind_method(D_METHOD("set_hit_back_faces", "p_value"), &DeepRayCast3D::set_hit_back_faces);
    ClassDB::bind_method(D_METHOD("get_hit_back_faces"), &DeepRayCast3D::get_hit_back_faces);
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "hit_back_faces", PROPERTY_HINT_NONE, ""), "set_hit_back_faces", "get_hit_back_faces");

    ClassDB::bind_method(D_METHOD("set_hit_from_inside", "p_value"), &DeepRayCast3D::set_hit_from_inside);
    ClassDB::bind_method(D_METHOD("get_hit_from_inside"), &DeepRayCast3D::get_hit_from_inside);
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "hit_from_inside", PROPERTY_HINT_NONE, ""), "set_hit_from_inside", "get_hit_from_inside");

    ClassDB::bind_method(D_METHOD("set_collision_mask", "p_value"), &DeepRayCast3D::set_collision_mask);
    ClassDB::bind_method(D_METHOD("get_collision_mask"), &DeepRayCast3D::get_collision_mask);
    ADD_PROPERTY(PropertyInfo(Variant::INT, "collision_mask", PROPERTY_HINT_LAYERS_3D_PHYSICS), "set_collision_mask", "get_collision_mask");

    ADD_SUBGROUP("Render", "");
    ClassDB::bind_method(D_METHOD("set_raycast_visible", "p_value"), &DeepRayCast3D::set_raycast_visible);
    ClassDB::bind_method(D_METHOD("get_raycast_visible"), &DeepRayCast3D::get_raycast_visible);
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "raycast_visible", PROPERTY_HINT_NONE, ""), "set_raycast_visible", "get_raycast_visible");

    ClassDB::bind_method(D_METHOD("set_color", "p_value"), &DeepRayCast3D::set_color);
    ClassDB::bind_method(D_METHOD("get_color"), &DeepRayCast3D::get_color);
    ADD_PROPERTY(PropertyInfo(Variant::COLOR, "color", PROPERTY_HINT_NONE, ""), "set_color", "get_color");

    ClassDB::bind_method(D_METHOD("set_radius", "p_value"), &DeepRayCast3D::set_radius);
    ClassDB::bind_method(D_METHOD("get_radius"), &DeepRayCast3D::get_radius);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "radius", PROPERTY_HINT_RANGE, "0.01, 0.5, 0.01, suffix:m"), "set_radius", "get_radius");

    ClassDB::bind_method(D_METHOD("set_opacity", "p_value"), &DeepRayCast3D::set_opacity);
    ClassDB::bind_method(D_METHOD("get_opacity"), &DeepRayCast3D::get_opacity);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "opacity", PROPERTY_HINT_RANGE, "0.01, 1.0, 0.01"), "set_opacity", "get_opacity");

    ClassDB::bind_method(D_METHOD("set_layers", "p_value"), &DeepRayCast3D::set_layers);
    ClassDB::bind_method(D_METHOD("get_layers"), &DeepRayCast3D::get_layers);
    ADD_PROPERTY(PropertyInfo(Variant::INT, "layers", PROPERTY_HINT_LAYERS_3D_RENDER), "set_layers", "get_layers");

    ADD_SUBGROUP("Transform", "");
    ClassDB::bind_method(D_METHOD("set_position_offset", "p_value"), &DeepRayCast3D::set_position_offset);
    ClassDB::bind_method(D_METHOD("get_position_offset"), &DeepRayCast3D::get_position_offset);
    ADD_PROPERTY(PropertyInfo(Variant::VECTOR3, "position_offset", PROPERTY_HINT_LAYERS_3D_RENDER), "set_position_offset", "get_position_offset");

    ADD_SIGNAL(MethodInfo("cast_collider", PropertyInfo(Variant::ARRAY, "results", PROPERTY_HINT_ARRAY_TYPE, "DeepRayCast3DResult")));

    ClassDB::bind_method(D_METHOD("get_collider_count"), &DeepRayCast3D::get_collider_count);
    ClassDB::bind_method(D_METHOD("get_collider", "p_value"), &DeepRayCast3D::get_collider);
    ClassDB::bind_method(D_METHOD("get_normal", "p_value"), &DeepRayCast3D::get_normal);
    ClassDB::bind_method(D_METHOD("get_position", "p_value"), &DeepRayCast3D::get_position);
    ClassDB::bind_method(D_METHOD("add_exclude", "p_value"), &DeepRayCast3D::add_exclude);
    ClassDB::bind_method(D_METHOD("remove_exclude", "p_value"), &DeepRayCast3D::remove_exclude);
    ClassDB::bind_method(D_METHOD("clear_exclude"), &DeepRayCast3D::clear_exclude);
}
#pragma endregion