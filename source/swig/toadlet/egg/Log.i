%{
#	include <toadlet/egg/Log.h>
%}

namespace toadlet{
namespace egg{

class Log{
public:
	enum Flags{
		Flags_START_SILENT=1<<0,
		Flags_PER_THREAD=1<<1,
		Flags_STORE_MAIN_ENTRIES=1<<2,
		Flags_STORE_THREAD_ENTRIES=1<<3,
	};

	static void initialize(int flags,const char *data=NULL);
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
