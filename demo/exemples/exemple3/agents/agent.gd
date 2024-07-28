extends GDAgent


# Monitor ID
var _monitor_id: String


# Constructor
func _init(new_label: String) -> void:
	label = new_label


# Setup Agent
func _setup() -> void:
	_monitor_id = get_first_agent_by_label("Monitor")


# New message
func _action(_delta: float, sender: String, message: String) -> void:
	print("[" + label + "]: has received " + message)
	var data: Dictionary = JSON.parse_string(message)
	if sender == _monitor_id and data["data"] == "start" || data["data"] == "continue":
		send(_monitor_id, JSON.stringify({"data": "done", "from": label}))
