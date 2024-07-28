extends GDEnvironment


# Max number of turns
@export var max_turns := 5

# Delay after turn
@export var delay_after_turn := 1000

# Agent classe
@onready var _agent_class := preload("res://exemples/exemple4/agents/Agent.tscn")


# Called when the node enters the scene tree for the first time
func _ready() -> void:
	add(WriterAgent.new("Writer"))
	
	for i in range (4):
		var agent := _agent_class.instantiate()
		agent.label = "Agent " + str(i + 1)
		add(agent)


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
