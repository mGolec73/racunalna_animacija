extends CharacterBody3D

const SPEED = 5.0 
const JUMP_VELOCITY = 4.5

func _physics_process(delta: float) -> void:
	
	if not is_on_floor():
		velocity += get_gravity() * delta
	
	if Input.is_action_just_pressed("ui_accept") and is_on_floor():
		velocity.y = JUMP_VELOCITY

	#vektor dobiven iz korisničkog unosa
	var input_dir := Input.get_vector("ui_left", "ui_right", "ui_up", "ui_down")
	#dobivanje globalnog smjera kretanja iz lokalnog 2d ulaza
	var direction := (transform.basis * Vector3(input_dir.x, 0, input_dir.y)).normalized()
	if direction:
		velocity.x = direction.x * SPEED
	else:
		velocity.x = move_toward(velocity.x, 0, SPEED) # smanjivanje brzine prema 0 

	var collision = get_last_slide_collision( )
	if collision:
		var collider = collision.get_collider()  
		if collider and collider.is_in_group("Obstacle"): #otkrivanje kolizije s preprekom
			end_game()		
	move_and_slide()


func end_game():
	const end_scene = preload("res://scenes/endScene.tscn")
	get_tree().change_scene_to_packed(end_scene)
	
