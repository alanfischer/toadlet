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

	void setCategoryReportingLevel(String categoryName,Level level);
	Level getCategoryReportingLevel(String categoryName);

	Level getMasterCategoryReportingLevel(String categoryName);

	void setStoreLogEntry(bool storeLogEntry);
	bool getStoreLogEntry() const;

	void addLogEntry(String categoryName,Level level,String text);
	void addLogEntry(Level level,String text);

	void flush();
};

}
}
