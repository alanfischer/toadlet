#include "../egg/Bindings.h"
#include <toadlet/hop/Simulator.h>

using namespace emscripten;
using namespace toadlet::egg;
using namespace toadlet::hop;

EMSCRIPTEN_BINDINGS(hop) {
    using namespace emscripten::internal;

	class_<Simulator>("Simulator")
		.smart_ptr_constructor(&make_ptr<Simulator>)
	;
}
