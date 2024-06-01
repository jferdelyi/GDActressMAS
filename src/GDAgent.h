#ifndef GDAGENT
#define GDAGENT

#include <godot_cpp/variant/variant.hpp>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/core/binder_common.hpp>

#include <CPPActressMAS/Agent.h>

using namespace godot;
using namespace std;

namespace godot {

	/**
	 * Godot Agent
	 */
	class GDAgent : public Node, public cam::Agent {
		GDCLASS(GDAgent, Node)

		// Private attributs
		private:


		// Private methods
		private:

			//###############################################################
			//	Internals
			//###############################################################


			//###############################################################
			//	Wrapped methods
			//###############################################################


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
			~GDAgent() override;

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

	};
}


#endif // GDAGENT
