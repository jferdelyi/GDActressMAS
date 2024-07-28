#include "GDAgent.h"

#include <godot_cpp/core/class_db.hpp>

#include <uuid/UUID.hpp>

#include "GDEnvironment.h"

using namespace godot;

GDAgent::GDAgent() {
    m_id = String(UUID::generate_uuid().c_str());
}

//###############################################################
//	Godot methods
//###############################################################

void GDAgent::_bind_methods() {
    // Methods
    ClassDB::bind_method(D_METHOD("is_setup"), &GDAgent::is_setup);
    ClassDB::bind_method(D_METHOD("is_dead"), &GDAgent::is_dead);
    ClassDB::bind_method(D_METHOD("stop"), &GDAgent::stop);
    ClassDB::bind_method(D_METHOD("get_id"), &GDAgent::get_id);

    ClassDB::bind_method(D_METHOD("add"), &GDAgent::add);
    ClassDB::bind_method(D_METHOD("agents_count"), &GDAgent::agents_count);
    ClassDB::bind_method(D_METHOD("get_agents_by_label"), &GDAgent::get_agents_by_label);
    ClassDB::bind_method(D_METHOD("get_first_agent_by_label"), &GDAgent::get_first_agent_by_label);
    ClassDB::bind_method(D_METHOD("get_filtered_agents"), &GDAgent::get_filtered_agents);
    ClassDB::bind_method(D_METHOD("get_agent_label"), &GDAgent::get_agent_label);

    ClassDB::bind_method(D_METHOD("send"), &GDAgent::send);
    ClassDB::bind_method(D_METHOD("send_by_label"), &GDAgent::send_by_label);
    ClassDB::bind_method(D_METHOD("send_by_fragment_label"), &GDAgent::send_by_fragment_label);
    ClassDB::bind_method(D_METHOD("broadcast"), &GDAgent::broadcast);
    ClassDB::bind_method(D_METHOD("get_environment"), &GDAgent::get_environment);

    ClassDB::bind_method(D_METHOD("randi"), &GDAgent::randi);
    ClassDB::bind_method(D_METHOD("randi_range"), &GDAgent::randi_range);
    ClassDB::bind_method(D_METHOD("randf"), &GDAgent::randf);
    ClassDB::bind_method(D_METHOD("randf_range"), &GDAgent::randf_range);

    ClassDB::bind_method(D_METHOD("see"), &GDAgent::see, DEFVAL(""));

    // Properties
    ClassDB::bind_method(D_METHOD("set_label"), &GDAgent::set_label);
    ClassDB::bind_method(D_METHOD("get_label"), &GDAgent::get_label);
    ClassDB::add_property(
            "GDAgent",
            PropertyInfo(Variant::STRING, "label", PROPERTY_HINT_NONE, "Agent Label"),
            "set_label",
            "get_label"
    );

    //ClassDB::bind_method(D_METHOD("set_can_see"), &GDAgent::set_can_see);
    //ClassDB::bind_method(D_METHOD("get_can_see"), &GDAgent::get_can_see);
    //ClassDB::add_property(
    //        "GDAgent",
    //        PropertyInfo(Variant::BOOL, "can_see", PROPERTY_HINT_NONE, "Agent Can See"),
    //        "set_can_see",
    //        "get_can_see"
    //);

    ClassDB::bind_method(D_METHOD("set_observables"), &GDAgent::set_observables);
    ClassDB::bind_method(D_METHOD("get_observables"), &GDAgent::get_observables);
    ClassDB::add_property(
            "GDAgent",
            PropertyInfo(Variant::DICTIONARY, "observables", PROPERTY_HINT_NONE, "Agent Observables"),
            "set_observables",
            "get_observables"
    );

    // Signals
    //ADD_SIGNAL(MethodInfo("setup", PropertyInfo(Variant::OBJECT, "receiver")));
    //ADD_SIGNAL(MethodInfo("see", PropertyInfo(Variant::OBJECT, "receiver"), PropertyInfo(Variant::DICTIONARY, "observables")));
    //ADD_SIGNAL(MethodInfo("action", PropertyInfo(Variant::OBJECT, "receiver"), PropertyInfo(Variant::STRING, "sender"), PropertyInfo(Variant::STRING, "message")));
    //ADD_SIGNAL(MethodInfo("default_action", PropertyInfo(Variant::OBJECT, "receiver")));
    ADD_SIGNAL(MethodInfo("stopped"));
}

//###############################################################
//	Internals
//###############################################################

void GDAgent::run_turn(float p_elapsed_time, bool p_run_setup_separately) {
    if (is_dead()) {
        return;
    }

    if (p_run_setup_separately) {
        if (!is_setup()) {
            setup();
        } else {
            //if (m_can_see) {
            //    see();
            //}
            if (is_dead()) {
                return;
            }
            action(p_elapsed_time);
        }

    } else {
        setup();
        if (is_dead()) {
            return;
        }
        //if (m_can_see) {
        //    see();
        //}
        if (is_dead()) {
            return;
        }
        action(p_elapsed_time);
    }
}

int GDAgent::agents_count() const {
    return static_cast<int>(m_environment->agents_count());
}

String GDAgent::add(const Variant& p_agent) const {
    return m_environment->add(p_agent);
}

Array GDAgent::get_agents_by_label(const String& p_label, bool p_first_only) const {
    return m_environment->get_agents_by_label(p_label, p_first_only);
}

String GDAgent::get_first_agent_by_label(const String& p_label) const{
    return m_environment->get_first_agent_by_label(p_label);
}

Array GDAgent::get_filtered_agents(const String& p_fragment_label, bool p_first_only) const {
    return m_environment->get_filtered_agents(p_fragment_label, p_first_only);
}

Variant GDAgent::get_agent_label(const String& p_id) const {
    const auto& l_data = m_environment->get_agent_label(p_id);
    if (l_data.has_value()) {
        return l_data.value();
    }
    return {};
}

void GDAgent::send(const String& p_receiver_id, const String& p_message) const {
    m_environment->send(m_id, p_receiver_id, p_message);
}

void GDAgent::send_by_label(const String& p_receiver_label, const String& p_message, bool p_first_only) const {
    m_environment->send_by_label(m_id, p_receiver_label, p_message, false, p_first_only);
}

void GDAgent::send_by_fragment_label(const String& p_fragment_label, const String& p_message, bool p_first_only) const {
    m_environment->send_by_label(m_id, p_fragment_label, p_message, true, p_first_only);
}

void GDAgent::broadcast(const String& p_message) const {
    m_environment->broadcast(m_id, p_message);
}

unsigned int GDAgent::randi() {
    return m_environment->randi();
}

int GDAgent::randi_range(int p_min, int p_max) {
    return m_environment->randi_range(p_min, p_max);
}

double GDAgent::randf() {
    return m_environment->randf();
}

double GDAgent::randf_range(double p_min, double p_max) {
    return m_environment->randf_range(p_min, p_max);
}

//###############################################################
// To override
//###############################################################

void GDAgent::setup() {
    m_is_setup = true;
    //emit_signal("setup", this);
    //call_deferred("emit_signal", "setup", this);
    call("_setup");
}

Dictionary GDAgent::see(const Variant& p_parameters) {
    return m_environment->get_obervables(*this, p_parameters);
}

void GDAgent::action(float p_elapsed_time) {
    if (MessagePointer l_message; m_messages.dequeue(l_message)) {
        do {
            //emit_signal("action", this, l_message->get_sender(), l_message->to_string());
            //call_deferred("emit_signal", "action", this, l_message->get_sender(), l_message->to_string());
            call("_action", p_elapsed_time, l_message->get_sender(), l_message->to_string());
        } while (m_messages.dequeue(l_message));
    } else {
        default_action(p_elapsed_time);
    }
}

void GDAgent::default_action(float p_elapsed_time) {
    //emit_signal("default_action", this);
    //call_deferred("emit_signal", "default_action", this);
    call("_default_action", p_elapsed_time);
}
