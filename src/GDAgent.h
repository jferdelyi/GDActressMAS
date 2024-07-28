#ifndef GDAGENT
#define GDAGENT

#include <godot_cpp/variant/variant.hpp>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/core/binder_common.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include <nlohmann/json.hpp>
#include <utility>
using json = nlohmann::json;

#include "MPSCQueue.hpp"
#include "Message.h"

using namespace godot;
using namespace std;

namespace godot {

    class GDEnvironment;

    /**
     * Represents the observable properties of an agent. They depend on the set of
     * Observables properties of an agent and on the PerceptionFilter function of an
     * agent who wants to observe other agents.
     **/
    using Observables = std::unordered_map<std::string, json>;
    using ObservablesPointer = std::shared_ptr<const Observables>;

    /**
     * Godot Agent
     */
    class GDAgent : public Node {
    GDCLASS(GDAgent, Node)

        // Private attributes
    private:
        // Exposed

        /**
         * Agent label
         **/
        String m_label = "Agent";

        /**
         * List of observables.
         **/
        Dictionary m_observables = Dictionary();

        /**
         * True if using observables.
         **/
        //bool m_can_see = false;


        // Internal

        /**
         * Unique ID.
         **/
        String m_id = "-1";

        /**
         * The environment where the agent is.
         **/
        GDEnvironment* m_environment = nullptr;

        /**
         * True if setup.
         **/
        bool m_is_setup = false;

        /**
         * True if dead.
         **/
        bool m_is_dead = false;

        /**
         * Messages arrived.
         **/
        MPSCQueue<MessagePointer> m_messages = MPSCQueue<MessagePointer>();

        // Public methods
    public:

        //###############################################################
        //	Internals
        //###############################################################


        // Must be overridden

        /**
         * Perception filter.
         * @param p_observed Observed properties
         * @return True if the agent is observable
         **/
        //[[nodiscard]] bool perception_filter(const Dictionary& p_observed);

        // Others

        /**
         * The number of agents in the environment
         **/
        int agents_count() const;

        /**
         * Add agent
         * @param p_agent agent
         * @return agent ID
        **/
        String add(const Variant& p_agent) const;

        /**
         * Get all agents by label
         * @param p_label the label of agent
         * @param p_first_only if true the first found
         * @return All agents by label
         **/
        [[nodiscard]] Array get_agents_by_label(const String& p_label, bool p_first_only = false) const;

        /**
         * Get first agent by label
         * @param p_label the label of agent
         * @return All agents by label
         **/
        [[nodiscard]] String get_first_agent_by_label(const String& p_label) const;

        /**
         * Get all agents by fragment label
         * @param p_fragment_label the fragment label of agent
         * @param p_first_only if true the first found
         * @return All agents by fragment label
         **/
        [[nodiscard]] Array get_filtered_agents(const String& p_fragment_label, bool p_first_only = false) const;

        /**
         * Get agent name by id
         * @param p_id the name of agent
         * @return Agents name
         **/
        [[nodiscard]] Variant get_agent_label(const String& p_id) const;

    public:

        GDEnvironment* get_environment() const {
            return m_environment;
        }

        void set_environment(GDEnvironment* p_environment) {
            m_environment = p_environment;
        }

        String get_id() const {
            return m_id;
        }

        String get_label() const {
            return m_label;
        }
        void set_label(const String& p_label) {
            m_label = p_label;
        }
        /*bool get_can_see() const {
            return m_can_see;
        }
        void set_can_see(bool p_can_see) {
            m_can_see = p_can_see;
        }*/
        Dictionary get_observables() const {
            return m_observables;
        }
        void set_observables(Dictionary p_observables) {
            m_observables = std::move(p_observables);
        }

        /**
         * True if using observables.
         * @return True if using observables
         **/
        /*[[nodiscard]] bool can_see() const {
            return m_can_see;
        }*/

        /**
         * True is must run setup.
         * @return True is must run setup
         **/
        [[nodiscard]] bool is_setup() const {
            return m_is_setup;
        }

        /**
         * True if is dead.
         * @return True if is dead
         **/
        [[nodiscard]] bool is_dead() const {
            return m_is_dead;
        }

        /**
         * Receive a new message.
         * @param p_message The new message
         **/
        void post(const MessagePointer& p_message) {
            m_messages.enqueue(p_message);
        }

        /**
         * Stops the execution of the agent and removes it from the environment.
         * Use the Stop method instead of Environment.
         * Remove when the decision to be stopped belongs to the agent itself.
         **/
        void stop() {
            m_is_dead = true;
            call("emit_signal", "stopped");
        }

        /**
         * Send a new message by ID.
         * @param p_receiver_id The id of the receiver
         * @param p_message The message
         **/
        void send(const String& p_receiver_id, const String& p_message) const;

        /**
         * Send a new message by label.
         * @param p_receiver_label The label of the receiver
         * @param p_message The message
         * @param p_first_only If true, only the first agent found
         **/
        void send_by_label(const String& p_receiver_label, const String& p_message, bool p_first_only) const;

        /**
         * Send a new message by fragment label.
         * @param p_fragment_label The fragment of the receivers
         * @param p_message The message
         * @param p_first_only If true, only the first agent found
         **/
        void send_by_fragment_label(const String& p_fragment_label, const String& p_message, bool p_first_only) const;

        /**
         * Send a new message to all agents.
         * @param p_message The message
         * @param p_length The message size
         **/
        void broadcast(const String& p_message) const;

        /**
         * Return random number (long)
         * @return random value between 0 and 4294967295
         **/
        unsigned int randi();

        /**
         * Return random number (range long)
         * @param p_min min value
         * @param p_max max value
         * @return random value between min and max
         **/
        int randi_range(int p_min, int p_max);

        /**
         * Return random number (double)
         * @return random value between 0.0 and 1.0
         **/
        double randf();

        /**
         * Return random number (range double)
         * @param p_min min value
         * @param p_max max value
         * @return random value between min and max
         **/
        double randf_range(double p_min, double p_max);

        /**
         * Run one turn (for one agent)
         * @param p_elapsed_time elapsed time between two calls
         * @param p_run_setup_separately If true, run setup in the same tick as see/act
         **/
        void run_turn(float p_elapsed_time, bool p_run_setup_separately = true);

        /**
         * Call to reset agent i.e. set setup and dead to false
         **/
        //void reset();

        // Public methods
    public:

        //###############################################################
        //	Constructor
        //###############################################################

        /**
         * Constructor
         */
        GDAgent();

        /**
         * Destructor
         */
        ~GDAgent() override = default;

        //###############################################################
        //	Godot methods
        //###############################################################

        /**
         * Bind methods, signals etc.
         */
        static void _bind_methods();


        //###############################################################
        //	Callbacks
        //###############################################################


        //###############################################################
        //	Emit handlers
        //###############################################################

        /**
         * Setup the agent.
         **/
        virtual void setup();

        /**
         * Compute see.
         * @param p_parameters parameters
         * @return observables
         **/
        virtual Dictionary see(const Variant& p_parameters);

        /**
         * Compute action.
         * @param p_elapsed_time elapsed time between two calls
         **/
        virtual void action(float p_elapsed_time);

        /**
         * Compute action if there is no message.
         * @param p_elapsed_time elapsed time between two calls
         **/
        virtual void default_action(float p_elapsed_time);
    };
}


#endif // GDAGENT
