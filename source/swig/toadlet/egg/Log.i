%{
#	include <toadlet/egg/Log.h>
%}

namespace toadlet{
namespace egg{

class Log{
public:
	static void initialize(bool startSlient=false);
	static Logger *getInstance();
	static void destroy();
	
	static void error(String text);
	static void error(String categoryName,String text);

	static void warning(String text);
	static void warning(String categoryName,String text);

	static void alert(String text);
	static void alert(String categoryName,String text);

	static void debug(String text);
	static void debug(String categoryName,String text);

	static void excess(String text);
	static void excess(String categoryName,String text);
};

}
}
