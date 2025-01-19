extends Node
class_name TerrainController

var Terrains: Array = []
var terrain_view: Array[MeshInstance3D] = []
var min_vel = 8.5
@export var t_velocity: float = 8.5
@export var t_in_scene = 4

@export_dir var t_path = "res://terrain"
@export_dir var t_start_path = "res://terrain_start.tscn"

var start_scene: MeshInstance3D = null

func _ready() -> void:
	load_terrains(t_path) # učitavanje svih terena 
	init_terrain(t_in_scene) # inicijalizacija terena za scenu 


func _physics_process(delta: float) -> void:
	t_velocity = min_vel + delta * 0.1
	move_terrain(delta)


func init_terrain(n:int) -> void:
	if t_start_path != "":
		var start_terrain = load(t_start_path)
		if start_terrain:
			start_scene = start_terrain.instantiate() as MeshInstance3D
			if start_scene:
				start_scene.position = Vector3(0,0,0)
				add_child(start_scene)
	for index in range(n):
		var next_block = Terrains.pick_random().instantiate() as MeshInstance3D
		if index == 0:
			if start_scene:
				calc_terrain_position(start_scene,next_block)
			else:
				next_block.position.z = next_block.mesh.size.y / 2
		else:
			var previous = terrain_view[index-1]
			calc_terrain_position(previous, next_block)
		add_child(next_block)
		terrain_view.append(next_block)

func move_terrain(delta:float) -> void:
	if start_scene:
		start_scene.position.z += t_velocity * delta
	for terr in terrain_view:
		terr.position.z += t_velocity * delta
	

	if terrain_view.size() > 0 and terrain_view[0].position.z >= (terrain_view[0].mesh.size.y / 2):
		var last = terrain_view[-1]
		var passed = terrain_view.pop_front()
		var new = Terrains.pick_random().instantiate() as MeshInstance3D
		calc_terrain_position(last,new)
		add_child(new)
		terrain_view.append(new)
		passed.queue_free()
		
		if start_scene and start_scene.position.z >= start_scene.mesh.size.y:
			start_scene.queue_free()
			start_scene = null
		
func calc_terrain_position(previous: MeshInstance3D, next: MeshInstance3D) -> void:
	var p_offset = previous.position.z - (previous.mesh.size.y /2)
	var n_offset = p_offset - (next.mesh.size.y/2)
	next.position.z = n_offset

func load_terrains(ter_path: String) -> void:
	var data = DirAccess.open(ter_path)
	if data:
		data.list_dir_begin() 
		var terrain_path = data.get_next()  
		while terrain_path:
			if data.current_is_dir():
				print("Skipping directory: " + terrain_path)
			else:
				var full_path = ter_path  + "/" + terrain_path
				print("Terrain block scene: " + full_path)
				
				var terrain_scene = load(full_path)
			
				if terrain_scene:
					Terrains.append(terrain_scene)
				else:
					print("Error loading terrain scene: " + full_path)
				
				terrain_path = data.get_next()
				data.list_dir_end()
	else:
		print("Error: Unable to open directory " + ter_path)
