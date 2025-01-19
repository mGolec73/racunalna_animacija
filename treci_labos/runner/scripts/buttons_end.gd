extends VBoxContainer
	
@onready var timerLabel = $"../Camera3D/ScoreTimeLabel"
@onready var pickUpLabel = $"../Camera3D/ScorePickUpLabel"

var timer = 0
var pickUp = 0

func _ready() -> void:
	timer = GameManager.timer
	pickUp = GameManager.present_counter
	timerLabel.text = "Time: " + str(floor(timer))
	pickUpLabel.text = "Score: " + str(pickUp)
	
func _on_play_again_pressed() -> void:
	GameManager.loadGame()

func _on_back_pressed() -> void:
	GameManager.loadMainMenu()
	
	
