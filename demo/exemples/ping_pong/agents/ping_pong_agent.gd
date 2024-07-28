extends GDAgent
class_name PinPongAgent

# Max message
var message_count_max := 100 * 1000 * 10000;

# Number agent
var agent_count := 10;


# Constructor
func _init(new_label: String, new_message_count_max: int, new_agent_count: int) -> void:
	label = new_label
	message_count_max = new_message_count_max
	agent_count = new_agent_count


# Setup the agent.
func _setup() -> void:
	broadcast("")
	PingPongEnvironment.message_count += agent_count


# Compute action.
# @param message The message to compute
func _action(_delta: float, sender: String, _message: String) -> void:
	if PingPongEnvironment.message_count < message_count_max:
		send(sender, "")
		PingPongEnvironment.message_count += 1
