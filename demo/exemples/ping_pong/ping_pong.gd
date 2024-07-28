extends GDEnvironment
class_name PingPongEnvironment


# Max number of turns
@export var max_turns := 10000

# Max message
@export var message_count_max := 100 * 1000 * 10000;

# Number agent
@export var agent_count := 10;

# Trial max
var trial_max := 5;

# Actual trial
var _trial := 0;

# The sum
var _sum := 0.0

# Start trial time
var _start_time := 0

# Message count
static var message_count := 0


# Called when the node enters the scene tree for the first time
func _ready() -> void:
	set_process(false)
	set_physics_process(false)
	for i in range(agent_count):
		add(PinPongAgent.new("Agent " + str(i + 1), message_count_max, agent_count))
	start_trial(_trial + 1)


# One trial
func start_trial(trial: int) -> void:
	print("Trial " + str(trial))
	message_count = 0
	
	_start_time = Time.get_ticks_msec()
	for _turn in range(max_turns):
		one_turn(Time.get_ticks_msec() - _start_time)
	var elapsed_time := Time.get_ticks_msec() - _start_time
	
	print(str(message_count) + " messages")
	print(str(elapsed_time) + " ms")
	_sum += float(message_count) / float(elapsed_time)

	_trial += 1
	if _trial >= trial_max:
		_on_simulation_finished()
	else:
		start_trial(_trial + 1)


# On multi-agent simulation finished
func _on_simulation_finished() -> void:
	print(str(_sum / trial_max) + " messages/ms")
	print("Simulation finished")
	#get_tree().quit()
