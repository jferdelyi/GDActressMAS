# Agent.
#
# Core behavior of agent.

@tool
@icon("../agent.png")
class_name Agent
extends Node


## True if using observables.
@export var can_see: bool = false
## List of observables.
@export var observables: Dictionary = {}

## Unique ID.
var id: String = UUID.v4()
## True if setup.
var is_setup: bool = false

## The environment where the agent is.
var _environment: EnvironmentMAS = null
## Messages arrived.
var _messages: Array = []


# Setup
func _ready() -> void:
	if not Engine.is_editor_hint() and get_parent() is EnvironmentMAS:
		_environment = get_parent()
		_environment.add(self)


# Called every frame. 'delta' is the elapsed time since the previous frame.
# @param delta elapsed time since the previous frame
func _process(delta: float) -> void:
	if not Engine.is_editor_hint() and _environment:
		run_turn()


# Check if the agent is in an environment
# @return a list of warnings
func _get_configuration_warnings() -> PackedStringArray:
	if not get_parent() is EnvironmentMAS:
		return ['Agent must be in environment']
	return []


# When node enter tree
func _enter_tree() -> void:
	update_configuration_warnings()


# Run one turn (for one agent)
# @param run_setup_separately If true, run setup in the same tick as see/act
func run_turn(run_setup_separately: bool = true) -> void:
	if run_setup_separately:
		if not is_setup:
			internal_setup()
		else:
			if can_see:
				internal_see()
			internal_action()
	else:
		internal_setup()
		if can_see:
			internal_see()
		internal_action()


# Internal setup called by the environment.
func internal_setup() -> void:
	setup()
	is_setup = true


# Internal see called by the environment.
func internal_see() -> void:
	see(_environment.get_list_of_observable_agents(self))


# Internal action called by the environment.
func internal_action() -> void:
	if _messages.is_empty():
		default_action()
	else:
		while not _messages.is_empty():
			var message = _messages.pop_front()
			action(message)
	


# Stops the execution of the agent and removes it from the environment.
# Use the Stop method instead of Environment.
# Remove when the decision to be stopped belongs to the agent itself.
func stop() -> void:
	_environment.remove(self)
	queue_free()


# Receive a new message.
# @param message The new message
func post(message: Message) -> void:
	_messages.append(message)


# Send a new message by ID.
# @param receiver_id The id of the receiver
# @param message The message
func send(receiver_id: String, message: Variant) -> void:
	_environment.send(id, receiver_id,  JSON.stringify(message))


# Send a new message by name.
# @param receiver_name The name of the receiver
# @param message The message
# @param first_only If true, only the first agent found
func send_by_name(receiver_name: String, message: Variant, first_only: bool = true) -> void:
	_environment.send_by_name(id, receiver_name, JSON.stringify(message), false, first_only)


# Send a new message by fragment name.
# @param fragment_name The fragment of the receivers
# @param message The message
# @param first_only If true, only the first agent found
func send_by_fragment_name(fragment_name: String, message: Variant, first_only: bool = true) -> void:
	_environment.send_by_name(id, fragment_name, JSON.stringify(message), true, first_only)


# Send a new message to all agents.
# @param message The message
func broadcast(message: String) -> void:
	_environment.broadcast(id, message)


# Call to reset agent i.e. set setup and dead to false
func reset() -> void:
	is_setup = false


# Must be overridden


# Perception filter.
# @param observed Observed properties
# @return True if the agent is observable
func perception_filter(observed: Dictionary) -> bool:
	return false


# Setup the agent.
func setup() -> void:
	pass


# Compute see.
# @param observable_agents The list of observable agents
func see(observable_agents: Array) -> void:
	pass


# Compute action.
# @param message The message to compute
func action(message: Message) -> void:
	pass


# Compute action if there is no message.
func default_action() -> void:
	pass
