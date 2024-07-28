#ifndef GDENVIRONMENT
#define GDENVIRONMENT

#include <future>
#include <random>

#include <godot_cpp/variant/variant.hpp>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/json.hpp>
#include <godot_cpp/core/binder_common.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include <ThreadPool/ThreadPool.h>

#include <tsl/ordered_map.h>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

#include "MPSCQueue.hpp"
#include "GDAgent.h"

using namespace godot;
using namespace std;

namespace godot {

    /**
     * Godot Environment
     */
    class GDEnvironment : public Node {
    GDCLASS(GDEnvironment, Node)

    public:
    enum EnvironmentMode {
        Parallel,
        Sequential,
        SequentialRandom
    };

        // Private attributes
    private:

        // Exposed

        /**
         * MAS execution mode: Parallel, Sequential, SequentialRandom (sequential but random order)
         */
        EnvironmentMode m_environment_mas_mode = EnvironmentMode::Parallel;

        /**
         * Random seed
         */
        int m_seed = static_cast<int>(std::time(nullptr));

        /**
         * If true, call "_get_obervable", this method must be defined by user
         */
        bool m_is_using_custom_see = false;

        // Internal

        /**
         * Actual number of turns of the simulation.
         */
        int m_turn = 0;

        /**
         * Information that agent can get about the environment.
         */
        json m_environment_data = {};

        /**
         * Agents in the environment.
		 * Agents: id, content
		 **/
        tsl::ordered_map<std::string, GDAgent*> m_agents = tsl::ordered_map<std::string, GDAgent*>();

        /**
         * Agents in the environment.
         * Agents: label, id
         **/
        std::unordered_multimap<std::string, std::string> m_agents_by_label = std::unordered_multimap<std::string, std::string>();

        /**
         * New agent buffer
         */
        MPSCQueue<GDAgent*> m_new_agents = MPSCQueue<GDAgent*>();

        /**
         * Thread pool for agents
         **/
        ThreadPool m_pool = ThreadPool(std::thread::hardware_concurrency() == 0 ? 8 : std::thread::hardware_concurrency());


        // Methods
    public:

        //###############################################################
        //	Get/Set
        //###############################################################

        // Environment
        void set_environment_mas_mode(const String& p_environment_mas_mode);
        String get_environment_mas_mode() const;

        // Seed
        void set_seed(const int p_seed) {
            m_seed = p_seed;
        }
        int get_seed() const {
            return m_seed;
        }

        // Custom see
        void set_using_custom_see(const bool p_is_using_custom_see) {
            m_is_using_custom_see = p_is_using_custom_see;
        }
        bool get_using_custom_see() const {
            return m_is_using_custom_see;
        }

        // MAS mode
        EnvironmentMode get_mode() const {
            return m_environment_mas_mode;
        }

        // Turn
        int get_turn() const {
            return m_turn;
        }

        //###############################################################
        //	Internal
        //###############################################################

        /**
         * Sends a message from the outside of the multi agent system. Whenever
         * possible, the agents should use the Send method of their own class, not
         * the Send method of the environment. This method can also be used to
         * simulate a forwarding behavior.
         * @param p_sender_id The sender ID
         * @param p_receiver_id The receiver label
         * @param p_message The message to be sent
         **/
        void send(const String& p_sender_id, const String& p_receiver_id, const String& p_message) const;

        /**
         * Sends a message by label.
         * @param p_sender_id The sender ID
         * @param p_receiver_label The receiver label
         * @param p_message The message to be sent
         * @param p_is_fragment If true search all agent that the label contain "p_receiver_label"
         * @param p_first_only If true send to the first agent found
         **/
        void send_by_label(const String& p_sender_id, const String& p_receiver_label, const String& p_message, bool p_is_fragment, bool p_first_only) const;

        /**
         * Send a new message to all agents.
         * @param p_sender_id From
         * @param p_message The message
         **/
        void broadcast(const String& p_sender_id, const String& p_message) const;

        /**
         * Adds an agent to the environment.
         * @param l_agent agent to add
         * @return ID of the agent
         **/
        //String add_generic_agent(String p_agent_label, bool p_is_using_observables);
        String add(const Variant& l_agent);

        /**
         * Stops the execution of the agent identified by id and removes it from the
         * environment. Use the Remove method instead of Agent.Stop when the decision
         * to stop an agent does not belong to the agent itself, but to some other
         * agent or to an external factor.
         *
         * @param p_agent_id The id of the agent to be removed
         **/
        void remove(const String& p_agent_id);

        /**
         * Add all nodes already on the tree
         **/
        void add_nodes_on_tree();

        /**
         * One turn
         * @param p_elapsed_time time between two calls
         **/
        void one_turn(float p_elapsed_time);

        /**
         * Stops the simulation.
         **/
        void stop();

        /**
         * Get an agent.
         * @param p_id Agent ID
         * @return Agent pointer
         **/
        std::optional<GDAgent*> get(const std::string& p_id) const;
        Variant get_agent(const String& p_id) const;

        /**
         * Return random number
         * @param p_min min value
         * @param p_max max value
         * @return random value between min and max
         **/
        template<class T>
        [[nodiscard]] T get_random_value(T p_min, T p_max) const {
            static std::default_random_engine s_seeder(static_cast<int>(m_seed));
            static std::mt19937 s_generator(s_seeder());
            std::uniform_real_distribution<T> l_dist(p_min, p_max);
            return l_dist(s_generator);
        }

        /**
         * Return a vector (p_number length) of random index.
         * Fisher-Yates shuffle.
         * @param p_number Number of value in the returned vector
         **/
        [[nodiscard]] std::vector<int> random_permutation(size_t p_number) const;

        /**
		 * Get all ids
		 * @param p_alive_only if true then return only alive agents
		 * @return All ids
		 **/
		[[nodiscard]] std::vector<std::string> get_ids(bool p_alive_only = true);

        /**
         * Get all agents by name
         * @param p_name the name of agent
         * @param p_first_only if true the first found
         * @return All agents by name
         **/
        [[nodiscard]] Array get_agents_by_label(const String& p_name, bool p_first_only) const;

        /**
         * Get first agent by name
         * @param p_name the name of agent
         * @return All agents by name
         **/
        [[nodiscard]] String get_first_agent_by_label(const String& p_name) const;

        /**
         * Get all agents by fragment name
         * @param p_fragment_name the fragment name of agent
         * @param p_first_only if true the first found
         * @return All agents by fragment name
         **/
        [[nodiscard]] Array get_filtered_agents(const String& p_fragment_name, bool p_first_only) const;

        /**
         * Get agent name by id
         * @param p_id the name of agent
         * @return Agents name
         **/
        [[nodiscard]] std::optional<String> get_agent_label(const String& p_id) const;

        /**
         * The number of agents in the environment
         **/
        [[nodiscard]] int agents_count() const {
            return m_agents.size();
        }

        /**
         * Get the list of observable agents for an agent and its perception filter.
         * @param p_perceiving_agent Perceiving agent
         * @param p_parameter Parameters
         **/
        [[nodiscard]] Dictionary get_obervables(GDAgent& p_perceiving_agent, const Variant& p_parameter);
        [[nodiscard]] Array default_get_obervables(const String& p_perceiving_agent_id) const;

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

        //###############################################################
        //	Constructor
        //###############################################################

        /**
         * Constructor
         */
        GDEnvironment();

        /**
         * Destructor
         */
        ~GDEnvironment() override;

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
         * A method that may be optionally overridden to perform additional
         * processing after the simulation has finished.
         **/
        void simulation_finished();

        /**
         * A method that may be optionally overridden to perform additional
         * processing after a turn of the simulation has finished.
         * @param p_turn The turn that has just finished
         **/
        void turn_finished(int p_turn);

    };
}

VARIANT_ENUM_CAST(GDEnvironment::EnvironmentMode)

#endif // GDENVIRONMENT
