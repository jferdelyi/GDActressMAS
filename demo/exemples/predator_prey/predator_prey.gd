extends GDEnvironment
class_name PredatorPreyEnvironment


const CELL_SIZE = 10

@export var grid_size := 50
@export var inital_prey := 100
@export var inital_predator := 2
@export var time_each_turn := 0

@onready var _cell_class := preload("res://exemples/predator_prey/cell.tscn")
@onready var _agent_prey_class := preload("res://exemples/predator_prey/agents/prey.tscn")
@onready var _agent_predator_class := preload("res://exemples/predator_prey/agents/predator.tscn")

var _prey_number := 0
var _predator_number := 0
var _the_grid: Dictionary = {}
var _elapsed_time := 0.0


# Called when the node enters the scene tree for the first time
func _ready() -> void:
	grid_size = grid_size
	for x in range(grid_size):
		for y in range(grid_size):
			var cell := _cell_class.instantiate()
			cell.position = Vector2(CELL_SIZE / 2.0 + (CELL_SIZE * x), CELL_SIZE / 2.0 + (CELL_SIZE * y))
			add_child(cell)
	for i in range(inital_prey):
		add_agent(get_random_valid_position(0, grid_size, 0, grid_size), _agent_prey_class.instantiate())
	for i in range(inital_predator):
		add_agent(get_random_valid_position(0, grid_size, 0, grid_size), _agent_predator_class.instantiate())
	_prey_number = inital_prey
	_predator_number = inital_predator


# Called every frame. 'delta' is the elapsed time since the previous frame
func _process(delta: float) -> void:
	_elapsed_time += delta
	if _elapsed_time > time_each_turn / 1000.0:
		_elapsed_time = 0.0
		one_turn(delta)
		print("Predator number " + str(_predator_number))
		print("Prey number " + str(_prey_number))
		print()


func _custom_see(_agent: GDAgent, _parameters: Variant) -> Array:
	var agent_ids := []
	var positions := get_eight_neighbors(_agent.position, false)
	for position in positions:
		if position in _the_grid and is_instance_valid(_the_grid[position]):
			agent_ids.append(_the_grid[position].get_id())
		else:
			_the_grid.erase(position)
	return agent_ids


# Get valid position
func get_valid_positions(min_x: int, max_x: int, min_y: int, max_y: int) -> Array:
	var values: Array = []
	for x in range(min_x, max_x + 1):
		for y in range(min_y, max_y + 1):
			if x < 0 or y < 0:
				continue
			if x >= grid_size or y >= grid_size:
				continue
			var position := Vector2(x, y)
			if position not in _the_grid:
				values.append(position)
	return values


# Get valid position 8N
func get_eight_neighbors(center: Vector2, valid_only := true) -> Array:
	var values: Array = []
	for x in range(center.x - 1, center.x + 2):
		for y in range(center.y - 1, center.y + 2):
			if x == center.x and y == center.y:
				continue
			if x < 0 or y < 0:
				continue
			if x >= grid_size or y >= grid_size:
				continue
			var position := Vector2(x, y)
			if valid_only:
				if position not in _the_grid:
					values.append(position)
			else:
				if position in _the_grid:
					values.append(position)
	return values


# Get random valid position on the grid in range
func get_random_valid_position(min_x: int, max_x: int, min_y: int, max_y: int) -> Variant: # Vector or null
	var valid_positions := get_valid_positions(min_x, max_x, min_y, max_y)
	if not valid_positions.is_empty():
		var index := randi_range(0, valid_positions.size() - 1)
		return valid_positions[index]
	return null


# Get random valid position 8N
func get_random_valid_position_eight_neighbors(center: Vector2) -> Variant: # Vector or null
	var valid_positions := get_eight_neighbors(center)
	if not valid_positions.is_empty():
		return valid_positions[randi_range(0, valid_positions.size() - 1)]
	return null




var _mutex: = Mutex.new()

func random_move(agent: AgentBase) -> void:
	_mutex.lock()
	var valid_positions: Variant = get_eight_neighbors(agent.position)
	if not valid_positions.is_empty():
		move_agent(agent, valid_positions[randi_range(0, valid_positions.size() - 1)])
	_mutex.unlock()



func random_bread(agent: AgentBase) -> void:
	_mutex.lock()
	var new_position: Variant = get_random_valid_position_eight_neighbors(agent.position)
	if new_position != null:
		if agent.type == AgentBase.AgentType.PREY:
			add_agent(new_position, _agent_prey_class.instantiate())
		elif agent.type == AgentBase.AgentType.PREDATOR:
			add_agent(new_position, _agent_predator_class.instantiate())
	_mutex.unlock()


func eat(predator: AgentBase, prey_id: String) -> void:
	_mutex.lock()
	var prey: AgentBase = get_agent(prey_id)
	var prey_position := prey.position
	remove_agent(prey)
	move_agent(predator, prey_position)
	_mutex.unlock()


func move_agent(agent: AgentBase, new_position: Vector2) -> void:
	_the_grid.erase(agent.position)
	_the_grid[new_position] = agent
	agent.position = new_position


func remove_agent(agent: AgentBase) -> void:
	_the_grid.erase(agent.position)
	if agent.type == AgentBase.AgentType.PREY:
		_prey_number -= 1
	else:
		_predator_number -= 1
	agent.remove()


func add_agent(position: Vector2, agent: AgentBase) -> void:
	if agent.type == AgentBase.AgentType.PREY:
		agent.label = "Prey " + str(_prey_number)
		_prey_number += 1
	else:
		agent.label = "Predator " + str(_predator_number)
		_predator_number += 1
	
	agent.grid_size = grid_size
	agent.position = position
	_the_grid[position] = agent
	
	add(agent)

