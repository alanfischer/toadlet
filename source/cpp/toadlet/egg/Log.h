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

#ifndef TOADLET_EGG_LOG_H
#define TOADLET_EGG_LOG_H

#include "Logger.h"

#define TOADLET_MAKE_LOG_FUNCTION(name,level) \
	static void name(const char *text){name((char*)NULL,text);} \
	static void name(const char *categoryName,const char *text){ \
		Logger *instance=getInstance(); \
		if(level>Logger::Level_MAX) ; \
		else if(level<=instance->getMasterReportingLevel() && level<=instance->getCategoryReportingLevel(categoryName)){ \
			instance->addLogEntry(categoryName,level,text); \
		} \
	}

namespace toadlet{
namespace egg{

class LoggerListener;

class TOADLET_API Log{
public:
	static void initialize(bool startSilent=false,bool perThread=false,const char *options=NULL);
	static Logger *getInstance();
	static void destroy();

	TOADLET_MAKE_LOG_FUNCTION(error,Logger::Level_ERROR);
	TOADLET_MAKE_LOG_FUNCTION(warning,Logger::Level_WARNING);
	TOADLET_MAKE_LOG_FUNCTION(alert,Logger::Level_ALERT);
	TOADLET_MAKE_LOG_FUNCTION(debug,Logger::Level_DEBUG);
	TOADLET_MAKE_LOG_FUNCTION(excess,Logger::Level_EXCESS);

	static Logger::timestamp mtime();
	static void *currentThread();
	static void *createMutex();
	static void destroyMutex(void *mutex);
	static void lock(void *mutex);
	static void unlock(void *mutex);
	static void *createCondition();
	static void destroyCondition(void *wait);
	static void wait(void *condition,void *mutex);
	static void notify(void *condition);

private:
	static Logger *mTheLogger;
	static bool mPerThread;
	static Logger::List<LoggerListener*> mListeners;
	static Logger::List<Logger*> mThreadLoggers;
};

}
}

#endif
