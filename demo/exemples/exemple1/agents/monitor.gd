extends GDAgent


# Constructor
func _init(new_label: String) -> void:
	label = new_label


# On action: Monitor
func _action(_delta: float, sender: String, message: String) -> void:
	print("[" + label + "]: has received " + message + " from " + sender)
