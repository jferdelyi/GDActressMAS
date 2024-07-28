extends GDEnvironment

# Start time
var _start_time := 0


# Called when the node enters the scene tree for the first time
func _ready() -> void:
	_start_time = Time.get_ticks_msec()
	add(SkynetAgent.new("a0", 0, ""))
	one_turn(Time.get_ticks_msec() - _start_time)


# Called every frame. 'delta' is the elapsed time since the previous frame
# _delta: delta time (not used)
func _process(delta: float) -> void:
	if agents_count() > 0:
		one_turn(delta)
	else:
		stop()


# Called when simulation is finished
func _on_simulation_finished() -> void:
	print((Time.get_ticks_msec() - _start_time) / 1000.0, "s")
	OS.delay_msec(1000)
	get_tree().quit()
