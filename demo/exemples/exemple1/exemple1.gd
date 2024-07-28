extends GDEnvironment


# Classes
@onready var _agent_class := preload("res://exemples/exemple1/agents/agent.gd")
@onready var _monitor_class := preload("res://exemples/exemple1/agents/monitor.gd")

# Max number of turns
@export var max_turns := 100


# Called when the node enters the scene tree for the first time
func _ready() -> void:
	add(_monitor_class.new("Monitor"))
	add(_agent_class.new("Agent 1"))
	add(_agent_class.new("Agent 2"))


# Called every frame. 'delta' is the elapsed time since the previous frame
func _process(delta: float) -> void:
	if get_turn() < max_turns:
		one_turn(delta)
	else:
		stop()


# On multi-agent simulation finished
func _on_simulation_finished() -> void:
	print("Simulation finished")
	get_tree().quit()
