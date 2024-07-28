#include "GDEnvironment.h"

#include <random>

#include <godot_cpp/core/class_db.hpp>

#include "GDAgent.h"
#include "Message.h"

using namespace godot;

GDEnvironment::GDEnvironment() = default;

GDEnvironment::~GDEnvironment() {
    stop();
}

//###############################################################
//	Godot methods
//###############################################################

void GDEnvironment::_bind_methods() {
    // Methods
    //ClassDB::bind_method(D_METHOD("initialise", "no_turns", "stop_if_empty", "mode", "delay_after_turn", "seed"),&GDEnvironment::initialise);
    //ClassDB::bind_method(D_METHOD("set_global_data", "key", "value"), &GDEnvironment::set_global_data);
    //ClassDB::bind_method(D_METHOD("add", "label", "using_observables"), &GDEnvironment::add);

    ClassDB::bind_method(D_METHOD("add"), &GDEnvironment::add);
    ClassDB::bind_method(D_METHOD("one_turn"), &GDEnvironment::one_turn);
    ClassDB::bind_method(D_METHOD("stop"), &GDEnvironment::stop);
    ClassDB::bind_method(D_METHOD("get_turn"), &GDEnvironment::get_turn);
    ClassDB::bind_method(D_METHOD("agents_count"), &GDEnvironment::agents_count);
    ClassDB::bind_method(D_METHOD("get_agent"), &GDEnvironment::get_agent);

    ClassDB::bind_method(D_METHOD("randi"), &GDEnvironment::randi);
    ClassDB::bind_method(D_METHOD("randi_range"), &GDEnvironment::randi_range);
    ClassDB::bind_method(D_METHOD("randf"), &GDEnvironment::randf);
    ClassDB::bind_method(D_METHOD("randf_range"), &GDEnvironment::randf_range);

    // Signals
    ADD_SIGNAL(MethodInfo("simulation_finished"));
    ADD_SIGNAL(MethodInfo("turn_finished", PropertyInfo(Variant::INT, "turn")));

    // Enum
    BIND_ENUM_CONSTANT(Parallel);
    BIND_ENUM_CONSTANT(Sequential);
    BIND_ENUM_CONSTANT(SequentialRandom);

    // Properties
    ClassDB::bind_method(D_METHOD("set_environment_mas_mode"), &GDEnvironment::set_environment_mas_mode);
    ClassDB::bind_method(D_METHOD("get_environment_mas_mode"), &GDEnvironment::get_environment_mas_mode);
    ClassDB::add_property(
            "GDEnvironment",
            PropertyInfo(Variant::STRING, "environment_mas_mode", PROPERTY_HINT_ENUM, "Parallel,Sequential,Sequential Random"),
            "set_environment_mas_mode",
            "get_environment_mas_mode"
    );

    ClassDB::bind_method(D_METHOD("set_seed"), &GDEnvironment::set_seed);
    ClassDB::bind_method(D_METHOD("get_seed"), &GDEnvironment::get_seed);
    ClassDB::add_property(
            "GDEnvironment",
            PropertyInfo(Variant::INT, "seed", PROPERTY_HINT_NONE, "Random seed"),
            "set_seed",
            "get_seed"
    );

    ClassDB::bind_method(D_METHOD("set_using_custom_see"), &GDEnvironment::set_using_custom_see);
    ClassDB::bind_method(D_METHOD("get_using_custom_see"), &GDEnvironment::get_using_custom_see);
    ClassDB::add_property(
            "GDEnvironment",
            PropertyInfo(Variant::BOOL, "is_using_custom_see", PROPERTY_HINT_NONE, "If true, \"_get_obervables\" must be defined"),
            "set_using_custom_see",
            "get_using_custom_see"
    );
}

void GDEnvironment::set_environment_mas_mode(const String& p_environment_mas_mode) {
    if (p_environment_mas_mode == "Sequential") {
        m_environment_mas_mode = GDEnvironment::EnvironmentMode::Sequential;
    } else if (p_environment_mas_mode == "Sequential Random") {
        m_environment_mas_mode = GDEnvironment::EnvironmentMode::SequentialRandom;
    } else {
        m_environment_mas_mode = GDEnvironment::EnvironmentMode::Parallel;
    }
}
String GDEnvironment::get_environment_mas_mode() const {
    if (m_environment_mas_mode == GDEnvironment::EnvironmentMode::Sequential) {
        return "Sequential";
    } else if (m_environment_mas_mode == GDEnvironment::EnvironmentMode::SequentialRandom) {
        return "Sequential Random";
    }
    return "Parallel";
}

//###############################################################
//	Internals
//###############################################################

void GDEnvironment::send(const String& p_sender_id, const String& p_receiver_id, const String& p_message) const {
    const auto& l_agent = get(p_receiver_id.utf8().get_data());
    if (l_agent) {
        if (l_agent.value()->is_dead()) {
            return;
        }
        l_agent.value()->post(std::make_shared<Message>(p_sender_id, p_receiver_id, p_message));
    }
}

void GDEnvironment::send_by_label(const String& p_sender_id, const String& p_receiver_label, const String& p_message, const bool p_is_fragment, const bool p_first_only) const {
    const std::string& l_receiver_label = p_receiver_label.utf8().get_data();
    for (auto l_filtered_elements = m_agents_by_label | std::views::filter([p_is_fragment, l_receiver_label](auto& p_value) {
        if (p_is_fragment) {
            return p_value.first.find(l_receiver_label) != std::string::npos;
        }
        return p_value.first == l_receiver_label;
    });
    const auto& [l_name, l_id] : l_filtered_elements) {
        const auto& l_agent = get(l_id);
        if (!l_agent || l_agent.value()->is_dead()) {
            continue;
        }

        l_agent.value()->post(std::make_shared<Message>(p_sender_id, l_id.c_str(), p_message));
        if (p_first_only) {
            break;
        }
    }
}

void GDEnvironment::broadcast(const String& p_sender_id, const String& p_message) const {
    for (auto& [l_id, l_agent]: m_agents) {
        if (l_id != p_sender_id.utf8().get_data() && !l_agent->is_dead()) {
            l_agent->post(std::make_shared<Message>(p_sender_id, l_id.c_str(), p_message));
        }
    }
}


String GDEnvironment::add(const Variant& p_agent) {
    auto* l_agent = dynamic_cast<GDAgent*>(p_agent.operator Object *());
    if (l_agent) {
        l_agent->set_environment(this);
        l_agent->set_process(false);
        l_agent->set_physics_process(false);
        m_new_agents.enqueue(l_agent);
        return l_agent->get_id();
    }
    return "";
}


void GDEnvironment::remove(const String& p_id) {
    const std::string l_id = p_id.utf8().get_data();
    if (m_agents.contains(l_id)) {
        m_agents.erase(l_id);
    }
}

void GDEnvironment::add_nodes_on_tree() {
    const auto& l_children = get_children();
    for (int i = 0; i < l_children.size(); ++i) {
        auto* l_agent = dynamic_cast<GDAgent*>(l_children[i].operator Object *());
        if (l_agent) {
            // Temporary
            remove_child(l_agent);
            l_agent->set_environment(this);
            l_agent->set_process(false);
            l_agent->set_physics_process(false);
            m_new_agents.enqueue(l_agent);
        }
    }
}

void GDEnvironment::one_turn(float p_elapsed_time) {

    /**
     * First turn add all node already on the tree
     */

    if (m_turn == 0) {
        add_nodes_on_tree();
    }

    /**
     * Process buffers
     */

    // Remove dead agents
    std::vector<GDAgent*> l_to_delete_agents;
    l_to_delete_agents.reserve(m_agents.size());
    for (auto& [l_id, l_agent]: m_agents) {
        if (l_agent->is_dead()) {
            l_to_delete_agents.push_back(l_agent);
            const auto [l_start, l_end] = m_agents_by_label.equal_range(l_agent->get_label().utf8().get_data());
            for (auto l_agent_it = l_start; l_agent_it != l_end; ++l_agent_it) {
                if (l_agent_it->second == l_id) {
                    m_agents_by_label.erase(l_agent_it);
                    break;
                }
            }
        }
    }

    for (const auto& l_agent: l_to_delete_agents) {
        m_agents.erase(l_agent->get_id().utf8().get_data());
        memdelete(l_agent);
    }

    // Add new agents
    if (GDAgent* l_agent; m_new_agents.dequeue(l_agent)) {
        do {
            if (m_environment_mas_mode == EnvironmentMode::Parallel) {
                call_deferred("add_child",l_agent);
            } else {
                call("add_child",l_agent);
            }
            m_agents.insert(std::make_pair(l_agent->get_id().utf8().get_data(), l_agent));
            m_agents_by_label.insert(std::make_pair(l_agent->get_label().utf8().get_data(), l_agent->get_id().utf8().get_data()));
        } while (m_new_agents.dequeue(l_agent));
    }

    /**
     * One turn
     */

    // Sequential
    if (m_environment_mas_mode == EnvironmentMode::Sequential) {
        for (auto& [l_agent_id, l_agent]: m_agents) {
            l_agent->run_turn(p_elapsed_time);
        }

    // Random
    } else {
        const auto m_parallel = m_environment_mas_mode == EnvironmentMode::Parallel;
        std::vector<std::future<void>> l_asyncs;
        const std::vector<std::string> l_agents = get_ids();
        const auto l_count = l_agents.size();
        const std::vector<int> l_agent_order = random_permutation(l_count);
        size_t l_index = 0;

        while (l_index < l_count) {
            const std::string& l_agent_id = l_agents.at(l_agent_order.at(l_index++));
            const auto& l_agent = get(l_agent_id);
            if (l_agent) {
                if (m_parallel) {
                    l_asyncs.push_back(m_pool.addWorkFunc([l_agent, p_elapsed_time] {
                        l_agent.value()->run_turn(p_elapsed_time);
                    }));
                } else {
                    l_agent.value()->run_turn(p_elapsed_time);
                }

            }
        }
        for (std::future<void>& l_async: l_asyncs) {
            l_async.wait();
        }
    }

    /**
     * End of the turn
     */

    turn_finished(m_turn++);
}

void GDEnvironment::stop() {
    simulation_finished();
}

std::optional<GDAgent*> GDEnvironment::get(const std::string& p_id) const {
    const auto& l_it = m_agents.find(p_id);
    if (l_it == m_agents.end()) {
        // Must be replaced by true iterator....
        auto l_new_it = m_new_agents.begin();
        do {
            const auto& l_agent = l_new_it.m_node->data;
            if (l_agent->get_id().utf8().get_data() == p_id) {
                return l_agent;
            }
        } while (++l_new_it.m_node != nullptr);
        return std::nullopt;
    }
    return l_it->second;
}

Variant GDEnvironment::get_agent(const String& p_id) const {
    const auto l_agent = get(p_id.utf8().get_data());
    if (l_agent.has_value()) {
        return l_agent.value();
    }
    return Variant();
}

std::vector<int> GDEnvironment::random_permutation(const size_t p_number) const {
    std::vector<int> l_numbers(p_number);
    if (p_number == 0) {
        return l_numbers;
    }
    for (size_t l_index = 0; l_index < p_number; l_index++) {
        l_numbers[l_index] = static_cast<int>(l_index);
    }

    size_t l_index = p_number - 1;
    while (l_index > 1) {
        const auto l_k = get_random_value<size_t>(0, l_index);
        if (l_k == l_index) {
            continue;
        }
        const int l_temp = l_numbers[l_index];
        l_numbers[l_index] = l_numbers[l_k];
        l_numbers[l_k] = l_temp;
        l_index--;
    }

    return l_numbers;
}

std::vector<std::string> GDEnvironment::get_ids(const bool p_alive_only) {
	std::vector<std::string> l_result;
	l_result.reserve(m_agents.size());

	for (auto& [l_id, l_agent]: m_agents) {
		if (!p_alive_only || !l_agent->is_dead()) {
			l_result.push_back(l_id);
		}
	}

	return l_result;
}

Array GDEnvironment::get_agents_by_label(const String& p_name, bool p_first_only) const {
    Array l_returned_agents;
    for (const auto& [l_id, l_agent]: m_agents) {
        if (l_agent->get_label() == p_name) {
            l_returned_agents.push_back(String(l_id.c_str()));
            if (p_first_only) {
                break;
            }
        }
    }
    return l_returned_agents;
}

String GDEnvironment::get_first_agent_by_label(const String& p_name) const {
    const auto& l_agents = get_agents_by_label(p_name, true);
    if (l_agents.is_empty()) {
        return {};
    }
    return l_agents[0];
}

Array GDEnvironment::get_filtered_agents(const String& p_fragment_name, bool p_first_only) const {
    Array l_returned_agents;
    for (const auto& [l_id, l_agent]: m_agents) {
        if (l_agent->get_label().find(p_fragment_name) != std::string::npos) {
            l_returned_agents.push_back(l_agent->get_id());
            if (p_first_only) {
                break;
            }
        }
    }
    return l_returned_agents;
}

std::optional<String> GDEnvironment::get_agent_label(const String& p_id) const {
    const auto& l_agent = m_agents.find(p_id.utf8().get_data());
    if (l_agent == m_agents.end()) {
        return {};
    }
    return l_agent.value()->get_label();
}

Dictionary GDEnvironment::get_obervables(GDAgent& p_perceiving_agent, const Variant& p_parameters) {
    Array l_agent_ids;
    if (m_is_using_custom_see) {
        l_agent_ids = call("_custom_see", static_cast<Object *>(&p_perceiving_agent), p_parameters);
    } else {
        l_agent_ids = default_get_obervables(p_perceiving_agent.get_id());
    }

    Dictionary l_observables;
    for (int i = 0; i < l_agent_ids.size(); ++i) {
        const std::string& l_id = String(l_agent_ids[i]).utf8().get_data();
        if (!m_agents.contains(l_id)) {
            continue;
        }
        auto* l_agent = m_agents.at(l_id);
        if (l_agent->is_dead() || l_agent->get_id() == p_perceiving_agent.get_id().utf8().get_data()) {
            continue;
        }
        const auto& l_observable = l_agent->get_observables();
        if (l_observable.is_empty()) {
            continue;
        }
        if (p_perceiving_agent.call("_perception_filter", l_agent->get_observables())) {
            l_observables[l_agent->get_id()] = l_agent->get_observables();
        }
    }
    return l_observables;
}

Array GDEnvironment::default_get_obervables(const String& p_perceiving_agent_id) const {
    Array l_agents_id;

    // Map id:agent
    for (auto& [l_id, l_agent]: m_agents) {
        l_agents_id.push_back(l_id.c_str());
    }
    return l_agents_id;
}

unsigned int GDEnvironment::randi() {
    return get_random_value<unsigned int>(0, 4294967295);
}

int GDEnvironment::randi_range(int p_min, int p_max) {
    return get_random_value<int>(p_min, p_max);
}

double GDEnvironment::randf() {
    return get_random_value<double>(0.0, 1.0);
}

double GDEnvironment::randf_range(double p_min, double p_max) {
    return get_random_value<double>(p_min, p_max);
}

//###############################################################
//	Signals
//###############################################################

void GDEnvironment::simulation_finished() {
    if (m_environment_mas_mode == EnvironmentMode::Parallel) {
        call_deferred("emit_signal", "simulation_finished");
    } else {
        call("emit_signal", "simulation_finished");
    }
}

void GDEnvironment::turn_finished(int p_turn) {
    if (m_environment_mas_mode == EnvironmentMode::Parallel) {
        call_deferred("emit_signal", "turn_finished", p_turn);
    } else {
        call("emit_signal", "turn_finished", p_turn);
    }
}
