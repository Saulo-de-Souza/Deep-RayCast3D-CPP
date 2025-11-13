extends SpringArm3D


func _physics_process(delta: float) -> void:
	global_rotation.y = lerp(global_rotation.y, global_rotation.y + 0.01, delta * 8.0)
