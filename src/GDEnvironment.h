#ifndef GDENVIRONMENT
#define GDENVIRONMENT

#include <godot_cpp/variant/variant.hpp>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/core/binder_common.hpp>

#include <CPPActressMAS/Environment.h>

using namespace godot;
using namespace std;

namespace godot {

	/**
	 * Godot Environment
	 */
	class GDEnvironment : public Node, public cam::Environment {
		GDCLASS(GDEnvironment, Node)

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

	};
}


#endif // GDENVIRONMENT
