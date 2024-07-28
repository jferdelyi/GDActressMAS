extends GDAgent
class_name AgentExemple2_2


# Constructor
func _init(new_label: String) -> void:
	label = new_label


# Setup agent
func _setup() -> void:
	send_by_label("Agent 1", JSON.stringify({"data": "start", "from": label}), false);


# New message
func _action(_delta: float, _sender: String, message: String) -> void:
	print("[" + label + "]: has received " + message)
