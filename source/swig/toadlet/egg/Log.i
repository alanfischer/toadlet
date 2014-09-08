%{
#	include <toadlet/egg/Log.h>
%}

namespace toadlet{
namespace egg{

class Log{
public:
	static void initialize(bool startSlient=false,bool perThread=false,const char *options=NULL);
	static Logger *getInstance();
	static void destroy();
	
	static void error(const char *text);
	static void error(const char *categoryName,const char *text);

	static void warning(const char *text);
	static void warning(const char *categoryName,const char *text);

	static void alert(const char *text);
	static void alert(const char *categoryName,const char *text);

	static void debug(const char *text);
	static void debug(const char *categoryName,const char *text);

	static void excess(const char *text);
	static void excess(const char *categoryName,const char *text);
};

}
}
