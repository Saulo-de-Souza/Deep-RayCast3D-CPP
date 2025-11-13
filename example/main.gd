extends Node3D
@onready var deep_ray_cast: DeepRayCast3D = $From/DeepRayCast3D


func _physics_process(_delta: float) -> void:
	for i in range(deep_ray_cast.get_collider_count()):
		var collider = deep_ray_cast.get_collider(i)
		var pos = deep_ray_cast.get_position(i)
		var normal = deep_ray_cast.get_normal(i)
		print("Object: %s Position: %s Normal: %s" %[collider, pos, normal])
