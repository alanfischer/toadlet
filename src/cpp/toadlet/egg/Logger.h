/********** Copyright header - do not remove **********
 *
 * The Toadlet Engine
 *
 * Copyright 2009, Lightning Toads Productions, LLC
 *
 * Author(s): Alan Fischer, Andrew Fischer
 *
 * This file is part of The Toadlet Engine.
 *
 * The Toadlet Engine is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * The Toadlet Engine is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.

 * You should have received a copy of the GNU Lesser General Public License
 * along with The Toadlet Engine.  If not, see <http://www.gnu.org/licenses/>.
 *
 ********** Copyright header - do not remove **********/

#ifndef TOADLET_EGG_LOGGER_H
#define TOADLET_EGG_LOGGER_H

#include <toadlet/egg/Categories.h>
#include <toadlet/egg/Map.h>
#include <toadlet/egg/String.h>
#if !defined(TOADLET_NO_THREADS)
	#include <toadlet/egg/Mutex.h>
#endif

#define TOADLET_MAKE_LOGGER_FUNCTION(name,level) \
	static void name(const String &text){name((char*)NULL,text);} \
	static void name(const String &categoryName,const String &text){ \
		Logger *instance=getInstance(); \
		if(level>Level_MAX) ; \
		else if(level<=instance->getMasterReportingLevel() && level<=instance->getCategoryReportingLevel(categoryName)){ \
			instance->addLogEntry(categoryName,level,text); \
		} \
	}

namespace toadlet{
namespace egg{

class LoggerListener;

/// @brief A Singleton Logger class
///
/// Used to log all messages from the engine
class TOADLET_API Logger{
public:
	enum Level{
		Level_DISABLED=0,

		Level_ERROR,
		Level_WARNING,
		Level_ALERT,
		Level_DEBUG,
		Level_EXCESS,

		Level_MAX,
	};

	class Category{
	public:
		Category(const String &name,Level reportingLevel=Level_MAX){
			this->name=name;
			this->reportingLevel=reportingLevel;
		}

		String name;
		Level reportingLevel;
	};

	class Entry{
	public:
		Entry(Category *category=NULL,Level level=Level_MAX,uint64 time=0,const String &text=(char*)NULL){
			this->category=category;
			this->level=level;
			this->time=time;
			this->text=text;
		}

		Category *category;
		Level level;
		uint64 time;
		String text;
	};

	static Logger *getInstance();
	static void destroy();

	TOADLET_MAKE_LOGGER_FUNCTION(error,Level_ERROR);
	TOADLET_MAKE_LOGGER_FUNCTION(warning,Level_WARNING);
	TOADLET_MAKE_LOGGER_FUNCTION(alert,Level_ALERT);
	TOADLET_MAKE_LOGGER_FUNCTION(debug,Level_DEBUG);
	TOADLET_MAKE_LOGGER_FUNCTION(excess,Level_EXCESS);

	void setMasterReportingLevel(Level level);
	Level getMasterReportingLevel() const{return mReportingLevel;}

	void setCategoryReportingLevel(const String &categoryName,Level level);
	Level getCategoryReportingLevel(const String &categoryName);

	Level getMasterCategoryReportingLevel(const String &categoryName);

	void addLoggerListener(LoggerListener *listener);
	void removeLoggerListener(LoggerListener *listener);

	void setOutputLogEntry(bool outputLogEntry);
	bool getOutputLogEntry() const{return mOutputLogEntry;}

	void setStoreLogEntry(bool storeLogEntry);
	bool getStoreLogEntry() const{return mStoreLogEntry;}

	void addLogEntry(const String &categoryName,Level level,const String &text);
	void addLogEntry(Level level,const String &text){addLogEntry((char*)NULL,level,text);}

	int getNumLogEntries();
	Entry *getLogEntry(int i);

	Category *addCategory(const String &categoryName);
	Category *getCategory(const String &categoryName);

private:
	Logger();
	~Logger();

	typedef Map<String,Category*> CategoryNameCategoryMap;

	void addCompleteLogEntry(Category *category,Level level,const String &text);

	void lock();
	void unlock();

	static Logger *mTheLogger;

	egg::Collection<LoggerListener*> mLoggerListeners;
	bool mOutputLogEntry;
	bool mStoreLogEntry;
	Level mReportingLevel;
	bool mLoggedMessage;
	egg::Collection<Entry*> mLogEntries;
	CategoryNameCategoryMap mCategoryNameCategoryMap;
	#if !defined(TOADLET_NO_THREADS)
		Mutex mMutex;
	#endif
};

}
}

#endif
