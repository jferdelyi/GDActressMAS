extends GDEnvironment


# Max number of turns
@export var max_turns := 100


# Called when the node enters the scene tree for the first time
func _ready() -> void:
	add(AgentExemple2_1.new("Agent 1"))
	add(AgentExemple2_2.new("Agent 2"))


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

