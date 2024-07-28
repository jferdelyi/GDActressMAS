extends GDAgent
class_name WriterAgent


# Constructor
func _init(new_label: String) -> void:
	label = new_label


# Message received
func _action(_delta: float, _sender: String, message: String) -> void:
	print("[" + label + "]: has received " + message)

