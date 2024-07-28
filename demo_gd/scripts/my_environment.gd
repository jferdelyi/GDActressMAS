class_name MyEnvironment
extends EnvironmentMAS


var _start_time


# Setup
func _ready() -> void:
	_start_time = Time.get_ticks_msec()
	super()
	set_process(true)


# A method that may be optionally overridden to perform additional processing after the simulation has finished.
func simulation_finished():
	print((Time.get_ticks_msec() - _start_time) / 1000.0, "s")



