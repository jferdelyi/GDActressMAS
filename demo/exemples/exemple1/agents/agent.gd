extends GDAgent


# Random number generator
var _rng = RandomNumberGenerator.new()


# Constructor
func _init(new_label: String) -> void:
	label = new_label


# On setup: Agent 1 & 2
func _setup() -> void:
	print("[" + label + "]: starting")
	for i in range(1, 11):
		print("[" + label + "]: sending " + str(i))
		send_by_label("monitor", JSON.stringify({"data": i, "from": label}), true)
		OS.delay_msec(_rng.randi_range(10, 100))


# On action: Agent 1 & 2
func _action(_delta: float, sender: String, message: String) -> void:
	print("[" + label + "]: has received " + message + " from " + sender)
