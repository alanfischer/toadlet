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

#ifndef LOGIT_LOG_H
#define LOGIT_LOG_H

#include "Logger.h"
#include "FileLoggerListener.h"
#include <stdarg.h>

#define LOGIT_MAKE_LOG_FUNCTION(name,level) \
	static void name(const char *text){name((char*)NULL,text);} \
	static void name(const char *categoryName,const char *text){ \
		Logger *instance=getInstance(); \
		if(level>Logger::Level_MAX) ; \
		else if(level<=instance->getMasterReportingLevel() && level<=instance->getCategoryReportingLevel(categoryName)){ \
			instance->addLogEntry(categoryName,level,text); \
		} \
	} \
	static void name##f(const char *format,...){ \
		char buffer[1024]; \
		va_list list; \
		va_start(list,format); \
		vsnprintf(buffer,1024,format,list); \
		va_end(list); \
		name((char*)NULL,buffer); \
	} \
	static void name##cf(const char *categoryName,const char *format,...){ \
		char buffer[1024]; \
		va_list list; \
		va_start(list,format); \
		vsnprintf(buffer,1024,format,list); \
		va_end(list); \
		name(categoryName,buffer); \
	}

namespace logit{

class LoggerListener;

class LOGIT_API Log{
public:
	enum Flags{
		Flags_START_SILENT=1<<0,
		Flags_PER_THREAD=1<<1,
		Flags_STORE_MAIN_ENTRIES=1<<2,
		Flags_STORE_THREAD_ENTRIES=1<<3,
	};

	static void initialize(int flags=0,const char *data=NULL);
	static Logger *getInstance();
	static void destroy();

	LOGIT_MAKE_LOG_FUNCTION(error,Logger::Level_ERROR);
	LOGIT_MAKE_LOG_FUNCTION(warning,Logger::Level_WARNING);
	LOGIT_MAKE_LOG_FUNCTION(alert,Logger::Level_ALERT);
	LOGIT_MAKE_LOG_FUNCTION(debug,Logger::Level_DEBUG);
	LOGIT_MAKE_LOG_FUNCTION(excess,Logger::Level_EXCESS);

	static Logger::timestamp mtime();
	static void *currentThread();
	static void *createMutex();
	static void destroyMutex(void *mutex);
	static bool lock(void *mutex);
	static bool unlock(void *mutex);
	static void *createCondition();
	static void destroyCondition(void *wait);
	static bool wait(void *condition,void *mutex);
	static void notify(void *condition);

private:
	static Logger *mTheLogger;
	static int mFlags;
	static Logger::List<LoggerListener*> mListeners;
	static Logger::List<Logger*> mThreadLoggers;
};

}

#endif
