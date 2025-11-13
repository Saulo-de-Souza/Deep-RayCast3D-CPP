#include "DeepRayCast3DResult.hpp"
#include <godot_cpp/godot.hpp>

using namespace godot;

void DeepRayCast3DResult::_bind_methods()
{

    ClassDB::bind_method(D_METHOD("set_collider", "p_value"), &DeepRayCast3DResult::set_collider);
    ClassDB::bind_method(D_METHOD("get_collider"), &DeepRayCast3DResult::get_collider);

    ClassDB::bind_method(D_METHOD("set_collider_id", "p_value"), &DeepRayCast3DResult::set_collider_id);
    ClassDB::bind_method(D_METHOD("get_collider_id"), &DeepRayCast3DResult::get_collider_id);

    ClassDB::bind_method(D_METHOD("set_normal", "p_value"), &DeepRayCast3DResult::set_normal);
    ClassDB::bind_method(D_METHOD("get_normal"), &DeepRayCast3DResult::get_normal);

    ClassDB::bind_method(D_METHOD("set_position", "p_value"), &DeepRayCast3DResult::set_position);
    ClassDB::bind_method(D_METHOD("get_position"), &DeepRayCast3DResult::get_position);

    ClassDB::bind_method(D_METHOD("set_face_index", "p_value"), &DeepRayCast3DResult::set_face_index);
    ClassDB::bind_method(D_METHOD("get_face_index"), &DeepRayCast3DResult::get_face_index);

    ClassDB::bind_method(D_METHOD("set_rid", "p_value"), &DeepRayCast3DResult::set_rid);
    ClassDB::bind_method(D_METHOD("get_rid"), &DeepRayCast3DResult::get_rid);

    ClassDB::bind_method(D_METHOD("set_shape", "p_value"), &DeepRayCast3DResult::set_shape);
    ClassDB::bind_method(D_METHOD("get_shape"), &DeepRayCast3DResult::get_shape);
}

DeepRayCast3DResult::DeepRayCast3DResult()
{
    collider = nullptr;
    collider_id = -1;
    normal = Vector3();
    position = Vector3();
    face_index = -1;
    shape = -1;
}
DeepRayCast3DResult::DeepRayCast3DResult(PhysicsBody3D *p_collider, int p_collider_id, Vector3 p_normal, Vector3 p_position, int p_face_index, RID p_rid, int p_shape)
{
    collider = p_collider;
    collider_id = p_collider_id;
    normal = p_normal;
    position = p_position;
    face_index = p_face_index;
    rid = p_rid;
    shape = p_shape;
    UtilityFunctions::print("result");
}

DeepRayCast3DResult::~DeepRayCast3DResult()
{
}

// GETTERS AND SETTERS ***************************************************************************
void DeepRayCast3DResult::set_collider(PhysicsBody3D *p_value)
{
    collider = p_value;
}
PhysicsBody3D *DeepRayCast3DResult::get_collider()
{
    return collider;
}

void DeepRayCast3DResult::set_collider_id(int p_value)
{
    collider_id = p_value;
}
int DeepRayCast3DResult::get_collider_id()
{
    return collider_id;
}

void DeepRayCast3DResult::set_normal(Vector3 p_value)
{
    normal = p_value;
}
Vector3 DeepRayCast3DResult::get_normal()
{
    return normal;
}

void DeepRayCast3DResult::set_position(Vector3 p_value)
{
    position = p_value;
}
Vector3 DeepRayCast3DResult::get_position()
{
    return position;
}

void DeepRayCast3DResult::set_face_index(int p_value)
{
    face_index = p_value;
}
int DeepRayCast3DResult::get_face_index()
{
    return face_index;
}

void DeepRayCast3DResult::set_rid(RID p_value)
{
    rid = p_value;
}
RID DeepRayCast3DResult::get_rid()
{
    return rid;
}

void DeepRayCast3DResult::set_shape(int p_value)
{
    shape = p_value;
}
int DeepRayCast3DResult::get_shape()
{
    return shape;
}
// GETTERS AND SETTERS ***************************************************************************
