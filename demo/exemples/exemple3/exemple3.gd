extends GDEnvironment


# Max number of turns
@export var max_turns := 10

# Delay after turn
@export var delay_after_turn := 100

# Agent classe
@onready var _agent_class := preload("res://exemples/exemple3/agents/agent.gd")


# Called when the node enters the scene tree for the first time
func _ready() -> void:
	for i in range (4):
		add(_agent_class.new("Agent " + str(i + 1)))


# Called every frame. 'delta' is the elapsed time since the previous frame
func _process(delta: float) -> void:
	if get_turn() < max_turns:
		one_turn(delta)
	else:
		stop()


# Turn finished
func _on_turn_finished(turn: int) -> void:
	print("Turn " + str(turn + 1) + " finished")
	OS.delay_msec(delay_after_turn)


# On multi-agent simulation finished
func _on_simulation_finished() -> void:
	print("Simulation finished")
	get_tree().quit()
