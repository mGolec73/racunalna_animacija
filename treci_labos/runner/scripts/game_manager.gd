extends Node3D

const GAME = preload("res://scenes/world.tscn")
const MENU = preload("res://scenes/mainMenu.tscn")
const END = preload("res://scenes/endScene.tscn")


var timer = 0
var present_counter = 0
var playing = false

func loadMainMenu() -> void:
	get_tree().change_scene_to_packed(MENU)

func loadGame() -> void:
	get_tree().change_scene_to_packed(GAME)
	playing = true
	timer = 0
	present_counter = 0
	

func loadEndScene() -> void:
	get_tree().change_scene_to_packed(END)
	playing = false
	
func _process(delta: float) -> void:
	if playing:
		timer += delta
		var timerLabel = get_node_or_null("/root/World/Player/Camera3D/TimerLabel")
		if timerLabel != null:
			timerLabel.text = "Time: " + str(floor(timer))
		
func increase_count():
	present_counter += 1
	var presentLabel = get_node_or_null("/root/World/Player/Camera3D/PickUpLabel")
	if presentLabel != null:
		presentLabel.text = str(present_counter)
