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

#ifndef LOGIT_BASELOGGERLISTENER_H
#define LOGIT_BASELOGGERLISTENER_H

#include "LoggerListener.h"
#include <time.h>

namespace logit{

class BaseLoggerListener:public LoggerListener{
public:
	BaseLoggerListener(){
		newLine[0]=(char)10;
		newLine[1]=0;
	}

	const char *getTimeString(uint64 time){
		time_t tt=time/1000;
		struct tm *ts=gmtime(&tt);
		strftime(timeString,sizeof(timeString),"%Y-%m-%d %H:%M:%S",ts);
		return timeString;
	}

	const char *getLevelString(Logger::Level level){
		switch(level){
			case Logger::Level_DISABLED:
				return "LOGGER:  ";
			break;
			case Logger::Level_ERROR:
				return "ERROR:   ";
			break;
			case Logger::Level_WARNING:
				return "WARNING: ";
			break;
			case Logger::Level_ALERT:
				return "ALERT:   ";
			break;
			case Logger::Level_DEBUG:
				return "DEBUG:   ";
			break;
			case Logger::Level_EXCESS:
				return "EXCESS:  ";
			break;
			default:
				return "UNKNOWN: ";
			break;
		}
	}

	void flush(){}
	
protected:
	char timeString[128];
	char newLine[2];
};

}

#endif
