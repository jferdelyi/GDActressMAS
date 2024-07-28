extends GDAgent


# Number of turns
var _turn := 1


# No message this turn
func _default_action(_delta: float) -> void:
	if _turn > 3:
		stop()
	else:
		for i in range(1, 4):
			send_by_label("Writer", JSON.stringify({"turn": _turn, "index": i, "from": label, "timestamp": Time.get_ticks_usec()}), true)
		_turn += 1

