# Environment MAS
#
# An abstract base class for the multiagent environment, where all the agents are executed.
@icon("../environment_mas.png")
class_name EnvironmentMAS 
extends Node


# Environments execution modes
enum EnvironmentMasMode {
	Parallel,
	Sequential,
	SequentialRandom,
	Free
}


## The maximum number of turns of the simulation.
@export var number_of_turns: int = 0
## If true the simulation stops if there is no more agents.
@export var stop_if_empty: bool = false
## A delay (in milliseconds) after each turn.
@export var delay_after_turn: int = 0
## MAS execution mode: Parallel, Sequential, SequentialRandom (sequential but random order), Free (use _process of Agent nodes)
@export var environment_mas_mode: EnvironmentMasMode = EnvironmentMasMode.Parallel
## Random seed
@export var seed: int = (int)(Time.get_unix_time_from_system() * 1000)

## Unique ID.
var id: String = UUID.v4()
## Actual number of turns of the simulation.
var turn: int = 0
## Information that agent can get about the environment.
var environment_data: Dictionary = {}

## The agents in the environment.
var _agent_collection: Dictionary = {}
## Random number generator
var _rng:RandomNumberGenerator = RandomNumberGenerator.new()
## Thread pool tasks
var _tasks: Array = []
## True if execution mode Free is enable
var _execution_mode_free: bool = false


# Initializes a new instance of the Environment class.
func _init() -> void:
	_rng.seed = seed


# Setup
func _ready() -> void:
	get_tree().call_group(id, "set_physics_process", false)
	get_tree().call_group(id, "set_process", false)
	set_process(false)
	set_physics_process(false)
	_execution_mode_free = false


# Process one turn.
func _process(delta: float) -> void:
	if stop_if_empty and get_tree().get_nodes_in_group(id).size() == 0:
		stop()
		return
	
	# Sequential
	if environment_mas_mode == EnvironmentMasMode.Sequential:
		get_tree().call_group(id, "run_turn")
	
	# Random
	elif environment_mas_mode == EnvironmentMasMode.Parallel or environment_mas_mode == EnvironmentMasMode.SequentialRandom:
		var parallel = environment_mas_mode == EnvironmentMasMode.Parallel
		var agents = random_permutation(get_tree().get_nodes_in_group(id).size())
		for agent in agents:
			if agent:
				if parallel:
					_tasks.append(WorkerThreadPool.add_task(Callable(agent, "run_turn"), true))
				else:
					agent.run_turn()
		var task:int = 0
		while task < _tasks.size():
			if WorkerThreadPool.is_task_completed(_tasks[task]):
				_tasks.pop_at(task)
	
	if environment_mas_mode == EnvironmentMasMode.Free and not _execution_mode_free:
		get_tree().call_group(id, "set_process", true)
		_execution_mode_free = true
	
	elif environment_mas_mode != EnvironmentMasMode.Free and _execution_mode_free:
		get_tree().call_group(id, "set_process", false)
		_execution_mode_free = false
	
	if delay_after_turn > 0:
		set_process(false)
		await get_tree().create_timer(delay_after_turn / 1000).timeout
		set_process(true)
	turn_finished(turn)
	turn += 1
	if number_of_turns != 0 and turn >= number_of_turns:
		stop()


# Adds an agent to the environment.
func add(agent: Agent):
	if agent.get_parent():
		agent.reparent(self)
	elif agent.get_parent() != self:
		add_child(agent)
	agent.add_to_group(id)
	_agent_collection[agent.id] = agent


# Stops the execution of the agent identified by id and removes it from the environment.
func remove(agent: Agent):
	call_deferred("remove_from_group", id)
	_agent_collection.erase(agent.id)
	#if is_instance_valid(agent) and not agent.is_dead:
	#	agent.stop()


# Sends a message from the outside of the multiagent system.
func send(sender_id: String, receiver_id: String, message: String):
	for agent in get_tree().get_nodes_in_group(id):
		if agent.id == receiver_id:
			agent.post(Message.new(sender_id, message))


# Sends a message by name.
func send_by_name(sender_id: String, receiver_name: String, message: String, is_fragment: bool, first_only: bool):
	for agent in get_tree().get_nodes_in_group(id):
		var is_match:bool = false 
		if is_fragment:
			is_match = agent.label.find(receiver_name) != -1
		else:
			is_match = agent.label == receiver_name
			
		if is_match:
			agent.post(Message.new(sender_id, message))
			if first_only:
				break


# Send a new message to all agents.
func broadcast(sender_id: String, message: String):
	get_tree().call_group(id, "post", sender_id, message)


func start():
	set_process(true)
	if environment_mas_mode == EnvironmentMasMode.Free and not _execution_mode_free:
		get_tree().call_group(id, "set_process", true)
		_execution_mode_free = true


# Stops the simulation.
func stop():
	set_process(false)
	get_tree().call_group(id, "set_process", false)
	_execution_mode_free = false
	simulation_finished()


func random_permutation(number: int) -> Array:
	var agents = []
	agents.resize(number)
	if number == 0:
		return agents
	for index in range(number):
		agents[index] = get_tree().get_nodes_in_group(id)[index]
	var index = number - 1
	while index > 1:
		var k = _rng.randi_range(0, index)
		if k == index:
			continue
		var temp = agents[index]
		agents[index] = agents[k]
		agents[k] = temp
		index -= 1
	return agents


# Get the list of observable agents for an agent and its perception filter.
func get_list_of_observable_agents(perceiving_agent: Agent) -> Array:
	var observable_agent_list = []
	# Parcours des agents dans m_agent_collection.m_agents
	for agent in get_tree().get_nodes_in_group(id):
		if agent.is_dead or id == perceiving_agent.id:
			continue
		if agent.observables.is_empty():
			continue
		if perceiving_agent.perception_filter(agent.observables):
			observable_agent_list.append(agent.get_observables())
	return observable_agent_list


# Must be overridden


# A method that may be optionally overridden to perform additional processing after the simulation has finished.
func simulation_finished():
	pass


# A method that may be optionally overridden to perform additional processing after a turn of the simulation has finished.
func turn_finished(turn: int):
	pass

