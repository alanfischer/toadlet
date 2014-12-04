%{
#	include <toadlet/egg/Error.h>
%}

namespace toadlet{
namespace egg{

class Error{
public:
	enum Throw{
		Throw_NO,
		Throw_YES
	};

	static void initialize(bool handler=true);
	static Errorer *getInstance();
	static void destroy();
};

}
}
