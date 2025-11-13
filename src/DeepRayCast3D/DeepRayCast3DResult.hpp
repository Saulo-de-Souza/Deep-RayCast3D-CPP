#ifndef _DEEP_RAYCAST_3D_RESULT
#define _DEEP_RAYCAST_3D_RESULT

#include <godot_cpp/godot.hpp>
#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/classes/physics_body3d.hpp>

namespace godot
{
    class DeepRayCast3DResult : public RefCounted
    {
        GDCLASS(DeepRayCast3DResult, RefCounted);

    protected:
        static void _bind_methods();

    public:
        PhysicsBody3D *collider;
        int collider_id;
        Vector3 normal;
        Vector3 position;
        int face_index;
        RID rid;
        int shape;

        DeepRayCast3DResult();
        DeepRayCast3DResult(PhysicsBody3D *p_collider, int p_collider_id, Vector3 p_normal, Vector3 p_position, int p_face_index, RID p_rid, int p_shape);
        ~DeepRayCast3DResult();

        void set_collider(PhysicsBody3D *p_value);
        PhysicsBody3D *get_collider();

        void set_collider_id(int p_value);
        int get_collider_id();

        void set_normal(Vector3 p_value);
        Vector3 get_normal();

        void set_position(Vector3 p_value);
        Vector3 get_position();

        void set_face_index(int p_value);
        int get_face_index();

        void set_rid(RID p_value);
        RID get_rid();

        void set_shape(int p_value);
        int get_shape();
    };

}

#endif