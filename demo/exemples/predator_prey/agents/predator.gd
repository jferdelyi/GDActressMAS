class_name AgentPredator
extends AgentBase


var _turn_starving := 0


func _init() -> void:
	type = AgentType.PREDATOR


func _agent_action() -> void:
	_turn_starving += 1
	var observables_seen := see()
	if not observables_seen.is_empty():
		var ids := observables_seen.keys()
		var random_id: String = ids[randi_range(0, ids.size() - 1)]
		_environment.eat(self, random_id)
		_turn_starving = 0
	else:
		_environment.random_move(self)
	if _turn_alive >= 8:
		_turn_alive = 0
		_environment.random_bread(self)
	if _turn_starving >= 3:
		die()


func _perception_filter(observables_seen: Dictionary) -> bool:
	return observables_seen["type"] == AgentType.PREY
