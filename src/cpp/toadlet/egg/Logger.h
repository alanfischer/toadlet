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
	static void name(const String &description){name((char*)NULL,description);} \
	static void name(const String &categoryName,const String &description){ \
		Logger *instance=getInstance(); \
		if(level>Level_MAX) ; \
		else if(level<=instance->getMasterReportingLevel() && level<=instance->getCategoryReportingLevel(categoryName)){ \
			instance->addLogString(categoryName,level,description); \
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
		Category(const String &name):
			reportingLevel(Level_MAX)
		{
			this->name=name;
		}

		String name;
		Level reportingLevel;
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

	void setOutputLogString(bool outputLogString);
	bool getOutputLogString() const{return mOutputLogString;}

	void setStoreLogString(bool storeLogString);
	bool getStoreLogString() const{return mStoreLogString;}

	void addLogString(const String &categoryName,Level level,const String &string);
	void addLogString(Level level,const String &string);

	String getLogString();

private:
	Logger();
	~Logger();

	typedef Map<String,Category*> CategoryNameCategoryMap;

	Category *addCategory(const String &categoryName);
	Category *getCategory(const String &categoryName);

	void addCompleteLogString(Category *category,Level level,const String &data);

	void lock();
	void unlock();

	static Logger *mTheLogger;

	egg::Collection<LoggerListener*> mLoggerListeners;
	bool mOutputLogString;
	bool mStoreLogString;
	Level mReportingLevel;
	bool mLoggedMessage;
	CategoryNameCategoryMap mCategoryNameCategoryMap;
	String mLogString;
	#if !defined(TOADLET_NO_THREADS)
		Mutex mMutex;
	#endif
};

}
}

#endif
