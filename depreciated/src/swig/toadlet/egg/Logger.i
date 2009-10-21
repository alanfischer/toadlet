%{
#	include <toadlet/egg/Logger.h>
%}

namespace toadlet{
namespace egg{

class Logger{
public:
	enum Level{
		LEVEL_DISABLED=0,

		LEVEL_ERROR,
		LEVEL_WARNING,
		LEVEL_ALERT,
		LEVEL_DEBUG,
		LEVEL_EXCESSIVE,

		LEVEL_MAX=100,
	};

	static Logger *getInstance();
	static void destroy();

	void addLogString(const char *category,Level level,const char *string);
	void addLogString(Level level,const char *string);

protected:
	Logger();
	~Logger();
};

}
}
