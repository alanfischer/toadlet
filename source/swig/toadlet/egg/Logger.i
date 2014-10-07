%{
#	include <toadlet/egg/Logger.h>
%}

namespace toadlet{
namespace egg{

class LoggerEntry;

template<typename Type>
class LoggerList{};

class Logger{
public:
	typedef uint64 timestamp;

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

	const LoggerList<LoggerEntry*>& getLogEntries() const;
	
	void flush();
};

class LoggerEntry{
public:
	Logger::Level level;
	Logger::timestamp time;
	char *text;
};

}
}

// This is from: http://stackoverflow.com/questions/9465856/no-iterator-for-java-when-using-swig-with-cs-stdmap

%typemap(javainterfaces) EntryIterator "java.util.Iterator<LoggerEntry>"
%typemap(javacode) EntryIterator %{
public void remove() throws UnsupportedOperationException { throw new UnsupportedOperationException(); }
public LoggerEntry next() throws java.util.NoSuchElementException { if (!hasNext()) { throw new java.util.NoSuchElementException(); } return nextImpl(); }
%}

%javamethodmodifiers EntryIterator::nextImpl "private";
%inline %{
class EntryIterator:public RangeIterator<toadlet::egg::LoggerList<toadlet::egg::LoggerEntry*> >{
public:
	EntryIterator(const toadlet::egg::LoggerList<toadlet::egg::LoggerEntry*>& r) : RangeIterator(r) {}
	bool hasNext() const{return RangeIterator::hasNext();}
	toadlet::egg::LoggerEntry *nextImpl(){return RangeIterator::nextImpl();}
};
%}

%typemap(javainterfaces) toadlet::egg::LoggerList<toadlet::egg::LoggerEntry*> "Iterable<LoggerEntry>"

%newobject toadlet::egg::LoggerList<toadlet::egg::LoggerEntry*>::iterator() const;
%extend toadlet::egg::LoggerList<toadlet::egg::LoggerEntry*> {
	EntryIterator *iterator() const {
		return new EntryIterator(*$self);
	}
}

%template(EntryList) toadlet::egg::LoggerList<toadlet::egg::LoggerEntry*>;
