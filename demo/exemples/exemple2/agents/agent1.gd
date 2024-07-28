extends GDAgent
class_name AgentExemple2_1


# Constructor
func _init(new_label: String) -> void:
	label = new_label


# Setup agent
func _setup() -> void:
	send_by_label("Agent 2", JSON.stringify({"data": "start", "from": label}), false);
	for i in range(10):
		send_by_label("Agent 2", JSON.stringify({"data": "in setup #" + str(i + 1), "from": label}), false);
		OS.delay_msec(100)


# New message
func _action(_delta: float, _sender: String, message: String) -> void:
	print("[" + label + "]: has received " + message)
	for i in range(10):
		send_by_label("Agent 2", JSON.stringify({"data": "in action #" + str(i + 1), "from": label}), false);
		OS.delay_msec(100)
