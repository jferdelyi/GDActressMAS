extends GDAgent


# Actual round
var _round := 0

# Max number of rounds
var _max_rounds := 3

# Finished agents
var _finished := {}

# Agents ID
var _agents_id := []


# Setup agent
func _setup() -> void:
	_max_rounds = 3
	_round = 0
	
	print("[" + label + "]: start round 1 in setup")
	
	for agent_id in get_filtered_agents("Agent", false):
		_agents_id.append(agent_id)
		_finished[agent_id] = false
	
	for agent_id in _agents_id:
		var agent_name = get_agent_label(agent_id)
		if agent_name:
			print("[" + label + "]: sending to " + agent_name)
			send(agent_id, JSON.stringify({"data": "start", "from": label}))


# Message received
func _action(_delta: float, sender: String, message: String) -> void:
	print("[" + label + "]: has received " + message)
	var data: Dictionary = JSON.parse_string(message)
	
	if data["data"] == "done":
		_finished[sender] = true
	
	if _all_finished():
		_round += 1
		if _round >= _max_rounds:
			return
	
		for agent_id in _agents_id:
			_finished[agent_id] = false
	
		print("[" + label +  "]: start round " + str(_round + 1) + " in action")
	
		for agent_id in _agents_id:
			var agent_name = get_agent_label(agent_id)
			if agent_name:
				print("[" + label +  "]: sending to " + agent_name)
				send(agent_id, JSON.stringify({"data": "continue", "from": label}))


func _all_finished() -> bool:
	for value in _finished.values():
		if not value:
			return false
	return true
