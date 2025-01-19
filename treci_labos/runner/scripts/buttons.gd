extends VBoxContainer

func _on_start_button_pressed() -> void:
	GameManager.loadGame()
	
func _on_quit_button_pressed() -> void:
	get_tree().quit()
