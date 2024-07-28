@tool
class_name MyAgent
extends Agent


const CHILD_COUNT = 10
const AGENT_COUNT = 10000


var _sum_id: int = 0
var _parent_id: String = ""
var _sum: int = 0
var _message_left: int = 0


# Called when the node enters the scene tree for the first time.
func _init(new_name: String = name, sum_id: int = 0, parent_id: String = "") -> void:
	name = new_name
	_sum_id = sum_id
	_parent_id = parent_id


# Setup the agent.
func setup() -> void:
	for i in range(1, CHILD_COUNT + 1):
		var new_id = _sum_id * 10 + i
		if new_id < AGENT_COUNT:
			_environment.call_deferred("add", MyAgent.new("a" + str(new_id), new_id, id))
			_message_left += 1
	if _message_left == 0:
		send(_parent_id, {"sum": _sum_id})
		stop()


# Compute action.
# @param message The message to compute
func action(message: Message) -> void:
	_sum += JSON.parse_string(message.data)["sum"]
	_message_left -= 1
	
	if _message_left == 0:
		if name == "a0":
			print("Sum ", _sum)
		else:
			send(_parent_id, {"sum": _sum})
		queue_free()

