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

#include <toadlet/egg/Types.h>
#include <toadlet/egg/Collection.h>

#if defined(TOADLET_PLATFORM_WIN32)
	#pragma warning(disable:4996)
#endif

namespace toadlet{
namespace egg{

class LoggerListener;

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
		Category(const char *name,Level reportingLevel=Level_MAX){
			if(name==NULL){
				this->name=NULL;
			}
			else{
				this->name=new char[strlen(name)+1];
				strcpy(this->name,name);
			}
			this->reportingLevel=reportingLevel;
		}
		~Category(){
			delete[] name;
		}

		char *name;
		Level reportingLevel;
	};

	class Entry{
	public:
		Entry(Category *category=NULL,Level level=Level_MAX,uint64 time=0,const char *text=(char*)NULL){
			this->category=category;
			this->level=level;
			this->time=time;
			if(text==NULL){
				this->text=NULL;
			}
			else{
				this->text=new char[strlen(text)+1];
				strcpy(this->text,text);
			}
		}
		~Entry(){
			delete[] text;
		}

		Category *category;
		Level level;
		uint64 time;
		char *text;
	};

	Logger(bool startSilent);
	virtual ~Logger();

	void setMasterReportingLevel(Level level);
	Level getMasterReportingLevel() const{return mReportingLevel;}

	void setCategoryReportingLevel(const char *categoryName,Level level);
	Level getCategoryReportingLevel(const char *categoryName);

	Level getMasterCategoryReportingLevel(const char *categoryName);

	void addLoggerListener(LoggerListener *listener);
	void removeLoggerListener(LoggerListener *listener);

	void setStoreLogEntry(bool storeLogEntry);
	bool getStoreLogEntry() const{return mStoreLogEntry;}

	void addLogEntry(Level level,const char *text){addLogEntry((char*)NULL,level,text);}
	void addLogEntry(const char *categoryName,Level level,const char *text){addLogEntry(getCategory(categoryName),level,text);}
	void addLogEntry(Category *category,Level level,const char *text);

	void flush();

	void clearLogEntries();

	int getNumLogEntries();
	Entry *getLogEntry(int i);
	const char *getLogString(int i){return getLogEntry(i)->text;}

	Category *addCategory(const char *categoryName);
	Category *getCategory(const char *categoryName);

	void lock();
	void unlock();

private:
	void addCompleteLogEntry(Category *category,Level level,const char *text);

	egg::Collection<LoggerListener*> mLoggerListeners;
	Level mReportingLevel;
	egg::Collection<Category*> mCategories;
	bool mStoreLogEntry;
	egg::Collection<Entry*> mLogEntries;
	void *mMutex;
};

}
}

#endif
