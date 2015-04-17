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

#ifndef LOGIT_LOGGER_H
#define LOGIT_LOGGER_H

#if defined(LOGIT_PLATFORM_WIN32)
	#pragma warning(disable:4996)
	namespace std{ struct input_iterator_tag; }
#else
	#include <iterator>
#endif

namespace logit{

class LoggerListener;

class LOGIT_API Logger{
public:
	#if defined(LOGIT_PLATFORM_WIN32)
		typedef unsigned __int64 timestamp;
	#else
		typedef unsigned long long int timestamp;
	#endif

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
		Category(const char *name,Level reportingLevel=Level_MAX):data(NULL){init(name,reportingLevel);}
		Category(const Category &category):data(NULL){init(category.name,category.reportingLevel);}
		~Category(){delete[] name;}

		void init(const char *name,Level reportingLevel){
			if(name==NULL){this->name=NULL;}
			else{
				this->name=new char[strlen(name)+1];
				strcpy(this->name,name);
			}
			this->reportingLevel=reportingLevel;
		}

		char *name;
		Level reportingLevel;
		void *data;
	};

	class Entry{
	public:
		Entry(Category *category=NULL,Level level=Level_MAX,timestamp time=0,const char *text=(char*)NULL){init(category,level,time,text);}
		Entry(const Entry &entry){init(entry.category,entry.level,entry.time,entry.text);}
		~Entry(){delete[] text;}

		void init(Category *category,Level level,timestamp time,const char *text){
			this->category=category;
			this->level=level;
			this->time=time;
			if(text==NULL){this->text=NULL;}
			else{
				this->text=new char[strlen(text)+1];
				strcpy(this->text,text);
			}
		}

		Category *category;
		Level level;
		timestamp time;
		char *text;
	};

	template<typename Type>
	class List{
	public:
		typedef Type value_type;
		typedef Type& reference;
		typedef const Type& const_reference;
		typedef Type* pointer;
		typedef int difference_type;
		typedef std::input_iterator_tag iterator_category;
	
		struct node{
			node():next(NULL){}
			Type item;
			node *next;
		};

		struct iterator{
			typedef typename List<Type>::value_type value_type;
			typedef typename List<Type>::reference reference;
			typedef typename List<Type>::const_reference const_reference;
			typedef typename List<Type>::pointer pointer;
			typedef typename List<Type>::difference_type difference_type;
			typedef typename List<Type>::iterator_category iterator_category;

			iterator(node *n):next(n){}
			inline operator Type&() const{return next->item;}
			inline Type &operator*() const{return next->item;}
			inline Type *operator->() const{return &next->item;}
			inline bool operator==(const iterator &it) const{return it.next==next;}
			inline bool operator!=(const iterator &it) const{return it.next!=next;}
			inline iterator& operator++(){next=next->next;return *this;}
			inline iterator operator++(int){iterator it=*this;next=next->next;return it;}
			node *next;
		};

		List(){
			head=new node();
			tail=head;
		}

		~List(){
			node *n=head;
			while(n->next!=NULL){
				node *t=n;
				n=n->next;
				delete t;
			}
			delete n;
		}

		void push_back(const Type &item){
			node *n=head;
			while(n->next!=NULL) n=n->next;
			n->item=item;
			n->next=new node();
			tail=n->next;
		}

		void remove(const Type &item){
			node *n=head,*p=NULL;
			while(n->next!=NULL && n->item!=item){
				p=n;
				n=n->next;
			}
			if(n->next!=NULL){
				node *t=n;
				if(p!=NULL) p->next=t->next;
				if(t==head) head=n->next;
				delete t;
			}
		}

		void clear(){
			node *n=head;
			while(n->next!=NULL){
				node *t=n;
				n=n->next;
				delete t;
			}
			head=n;
			tail=head;
		}

		iterator begin() const{return iterator(head);}
		iterator end() const{return iterator(tail);}
		inline bool empty() const{return head==tail;}

		node *head,*tail;
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

	Category *getCategory(const char *categoryName);

	const List<Entry*> &getLogEntries() const{return mLogEntries;}
	void clearLogEntries();

	void setThread(void *thread){mThread=thread;}
	void *getThread() const{return mThread;}

	void lock();
	void unlock();

private:
	void addCompleteLogEntry(Category *category,Level level,const char *text);

	Level mReportingLevel;
	List<Category*> mCategories;
	List<LoggerListener*> mLoggerListeners;
	bool mStoreLogEntry;
	List<Entry*> mLogEntries;
	void *mThread;
	void *mMutex;
};

typedef Logger::Category LoggerCategory;
typedef Logger::Entry LoggerEntry;
typedef Logger::List<LoggerEntry*> LoggerEntryList;

}

#endif
