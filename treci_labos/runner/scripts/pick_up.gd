extends Area3D

@export var rotation_speed : float = 90.0
@export var oscillation_amplitude : float = 0.2
@export var oscillation_speed : float = 5.0
var time_passed : float = 0.0

func _process(delta: float) -> void:
	rotation_degrees.y += rotation_speed * delta 
	time_passed += delta
	var new_height = oscillation_amplitude * sin(time_passed * oscillation_speed)
	position.y = new_height + 0.5

func _on_body_entered(body: Node3D) -> void:
	if(body.is_in_group("Player")):
		GameManager.increase_count()
		queue_free()
