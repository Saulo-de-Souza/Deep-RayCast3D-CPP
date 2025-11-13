#ifndef DEEP_RAYCAST_3D_H
#define DEEP_RAYCAST_3D_H

#include <godot_cpp/godot.hpp>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/classes/world3d.hpp>
#include <godot_cpp/classes/cylinder_mesh.hpp>
#include <godot_cpp/classes/physics_body3d.hpp>
#include <godot_cpp/classes/mesh_instance3d.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/classes/standard_material3d.hpp>
#include <godot_cpp/classes/physics_direct_space_state3d.hpp>
#include <godot_cpp/classes/physics_ray_query_parameters3d.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/variant/node_path.hpp>
#include "DeepRayCast3DResult.hpp"

namespace godot
{
        class DeepRayCast3D : public Node
        {
                GDCLASS(DeepRayCast3D, Node);

        private:
#pragma region Private Properties
                Node3D *_to = nullptr;
                Node3D *_node_container = nullptr;
                MeshInstance3D *_mesh_instance = nullptr;

                TypedArray<RID> _excludes_rid;
                TypedArray<DeepRayCast3DResult> _deep_results;

                Ref<StandardMaterial3D> _resource_material;
                Ref<PhysicsRayQueryParameters3D> _params;
                Ref<StandardMaterial3D> _material;
                Ref<CylinderMesh> _mesh;

                Vector3 _direction;
                float _distance;
#pragma endregion

        protected:
#pragma region Engine Methods Protected
                static void _bind_methods();
#pragma endregion

        public:
#pragma region Exported Properties
                bool enabled;
                bool activate_emission;
                bool auto_forward;
                bool exclude_parent;
                bool collide_with_bodies;
                bool collide_with_areas;
                bool hit_back_faces;
                bool hit_from_inside;
                bool raycast_visible;
                float margin;
                float emission_energy;
                float forward_distance;
                float radius;
                float opacity;
                int max_result;
                int rings;
                int segments;
                int collision_mask;
                int layers;
                NodePath to_path;
                Vector3 position_offset;
                Color color;
#pragma endregion

#pragma region Constructor and Destructor
                DeepRayCast3D();
                ~DeepRayCast3D();
#pragma endregion

#pragma region Engine Methods
                virtual void _ready() override;
                virtual void _physics_process(double delta) override;
                virtual void _enter_tree() override;
                virtual PackedStringArray _get_configuration_warnings() const override;
#pragma endregion

#pragma region Signals
                void cast_collider(const TypedArray<DeepRayCast3DResult> &results);
#pragma endregion

#pragma region Getters and Setters
                void set_enabled(bool p_enabled);
                bool get_enabled();

                void set_activate_emission(bool p_activate_emission);
                bool get_activate_emission();

                void set_auto_forward(bool p_auto_forward);
                bool get_auto_forward();

                void set_exclude_parent(bool p_exclude_parent);
                bool get_exclude_parent();

                void set_collide_with_bodies(bool p_value);
                bool get_collide_with_bodies();

                void set_collide_with_areas(bool p_value);
                bool get_collide_with_areas();

                void set_hit_back_faces(bool p_value);
                bool get_hit_back_faces();

                void set_hit_from_inside(bool p_value);
                bool get_hit_from_inside();

                void set_raycast_visible(bool p_value);
                bool get_raycast_visible();

                void set_margin(float p_margin);
                float get_margin();

                void set_emission_energy(float p_emission_energy);
                float get_emission_energy();

                void set_forward_distance(float p_forward_distance);
                float get_forward_distance();

                void set_radius(float p_value);
                float get_radius();

                void set_opacity(float p_value);
                float get_opacity();

                void set_max_result(int p_max_result);
                int get_max_result();

                void set_rings(int p_rings);
                int get_rings();

                void set_segments(int p_segments);
                int get_segments();

                void set_collision_mask(int p_value);
                int get_collision_mask();

                void set_layers(int p_value);
                int get_layers();

                void set_to_path(NodePath p_to_path);
                NodePath get_to_path();

                void set_color(Color p_value);
                Color get_color();

                void set_position_offset(Vector3 p_value);
                Vector3 get_position_offset();
#pragma endregion

#pragma region Public Methods
                int get_collider_count();
                PhysicsBody3D *get_collider(int p_value);
                Vector3 get_normal(int p_value);
                Vector3 get_position(int p_value);
                void add_exclude(PhysicsBody3D *p_value);
                void remove_exclude(PhysicsBody3D *p_value);
                void clear_exclude();
#pragma endregion

#pragma region Private Methods
                void _create_line();
                void _verify_mesh();
                void _update_line();
                void _update_raycast();
#pragma endregion
        };
}

#endif