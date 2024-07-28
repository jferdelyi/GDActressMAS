class_name AgentPrey
extends AgentBase


func _init() -> void:
	type = AgentType.PREY


func _agent_action() -> void:
	_turn_alive += 1
	_environment.random_move(self)
	if _turn_alive >= 3:
		_turn_alive = 0
		_environment.random_bread(self)

