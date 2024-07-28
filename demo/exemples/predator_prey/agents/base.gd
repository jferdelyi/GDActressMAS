extends GDAgent
class_name  AgentBase


const CELL_SIZE = 10


enum AgentType {PREY, PREDATOR}


var type: AgentType:
	set(new_type):
		type = new_type
	get:
		return type
var grid_size := 50
var position := Vector2():
	set(new_position):
		position = new_position
		set_sprite_position(Vector2(int(new_position.x * CELL_SIZE + (CELL_SIZE / 2.0)), int(new_position.y * CELL_SIZE + (CELL_SIZE / 2.0))))
	get:
		return position
var _turn_alive := 0
var _environment: PredatorPreyEnvironment


func _ready() -> void:
	_environment = get_environment()
	observables["type"] = type


func _default_action(_delta: float) -> void:
	_turn_alive += 1
	call("_agent_action")


func die() -> void:
	get_environment().remove_agent(self)


func set_sprite_position(new_position: Vector2) -> void:
	$Sprite.position = new_position


func set_sprite_visible(visible: bool) -> void:
	$Sprite.visible = false


func remove() -> void:
	set_sprite_visible(false)
	stop()
