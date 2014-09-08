%{
#	include <toadlet/egg/Logger.h>
%}

namespace toadlet{
namespace egg{

class Logger{
public:
	enum Level{
		DISABLED=0,

		ERROR,
		WARNING,
		ALERT,
		DEBUG,
		EXCESS,

		MAX,
	};

	Logger(bool startSilent);
	virtual ~Logger();

	void setMasterReportingLevel(Level level);
	Level getMasterReportingLevel() const;

	void setCategoryReportingLevel(const char *categoryName,Level level);
	Level getCategoryReportingLevel(const char *categoryName);

	Level getMasterCategoryReportingLevel(const char *categoryName);

	void setStoreLogEntry(bool storeLogEntry);
	bool getStoreLogEntry() const;

	void addLogEntry(const char *categoryName,Level level,const char *text);
	void addLogEntry(Level level,const char *text);

	void flush();
};

}
}
