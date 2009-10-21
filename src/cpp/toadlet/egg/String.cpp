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

#include <toadlet/egg/String.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#if defined(TOADLET_PLATFORM_POSIX) || defined(TOADLET_PLATFORM_NDS)
	#include <wchar.h>
	#include <wctype.h>
#endif

// TODO: Move the snwprintf/printf stuff to the same way we did the scanf
#if defined(TOADLET_PLATFORM_WIN32)
	#define TOADLET_SNWPRINTF _snwprintf
	#define TOADLET_SSCANF(str,fmt,x) \
		swscanf(str->wc_str(),L##fmt,x)
#elif defined(TOADLET_PLATFORM_POSIX)
	#define TOADLET_SNWPRINTF swprintf
	#define TOADLET_SSCANF(str,fmt,x) \
		swscanf(str->wc_str(),L##fmt,x)
#else
	#define TOADLET_SSCANF(str,fmt,x) \
		sscanf(str->c_str(),fmt,x)
#endif

namespace toadlet{
namespace egg{

String::String(){
	mData=new stringchar[1];
	mData[0]=0;
	mLength=0;

	mNarrowData=NULL;
}

String::String(const char *text){
	if(text==NULL){
		mLength=0;
		mData=new stringchar[1];
		mData[0]=0;

		mNarrowData=NULL;

		return;
	}

	mLength=strlen(text);
	mData=new stringchar[mLength+1];
	stringchar *dest=mData;
	while((*dest++=*text++)!=0);

	mNarrowData=NULL;
}

String::String(const unsigned char *text){
	if(text==NULL){
		mLength=0;
		mData=new stringchar[1];
		mData[0]=0;

		mNarrowData=NULL;

		return;
	}

	mLength=strlen((const char*)text);
	mData=new stringchar[mLength+1];
	stringchar *dest=mData;
	while((*dest++=*text++)!=0);

	mNarrowData=NULL;
}

void String::internal_String(const stringchar *text){
	if(text==NULL){
		mLength=0;
		mData=new stringchar[1];
		mData[0]=0;

		mNarrowData=NULL;

		return;
	}

	mLength=wcslen((wchar_t*)text);
	mData=new stringchar[mLength+1];
	wcsncpy((wchar_t*)mData,(wchar_t*)text,mLength+1);

	mNarrowData=NULL;
}

String::String(int length){
	mLength=length;
	mData=new stringchar[mLength+1];

	mNarrowData=NULL;
}

String::String(long int length){
	mLength=length;
	mData=new stringchar[mLength+1];

	mNarrowData=NULL;
}

String::String(const String &string){
	mLength=string.mLength;
	mData=new stringchar[mLength+1];
	wcsncpy((wchar_t*)mData,(wchar_t*)string.mData,mLength+1);

	mNarrowData=NULL;
}

String::~String(){
	clearExtraData();
	delete[] mData;
	mData=NULL;
}

int String::find(char c) const{
	int i;
	for(i=0;i<mLength;++i){
		if(mData[i]==c){
			return i;
		}
	}

	return npos;
}

int String::internal_find(stringchar c) const{
	int i;
	for(i=0;i<mLength;++i){
		if(mData[i]==c){
			return i;
		}
	}

	return npos;
}

int String::find(char c,int i) const{
	for(;i<mLength;++i){
		if(mData[i]==c){
			return i;
		}
	}

	return npos;
}

int String::internal_find(stringchar c,int i) const{
	for(;i<mLength;++i){
		if(mData[i]==c){
			return i;
		}
	}

	return npos;
}

int String::rfind(char c) const{
	int i;
	for(i=mLength-1;i>=0;--i){
		if(mData[i]==c){
			return i;
		}
	}

	return npos;
}

int String::internal_rfind(stringchar c) const{
	int i;
	for(i=mLength-1;i>=0;--i){
		if(mData[i]==c){
			return i;
		}
	}

	return npos;
}

int String::rfind(char c,int i) const{
	for(;i>0;--i){
		if(mData[i]==c){
			return i;
		}
	}
	if(mData[0]==c){
		return 0;
	}

	return npos;
}

int String::internal_rfind(stringchar c,int i) const{
	for(;i>0;--i){
		if(mData[i]==c){
			return i;
		}
	}
	if(mData[0]==c){
		return 0;
	}

	return npos;
}

int String::find(const String &string) const{
	const wchar_t *result=wcsstr((wchar_t*)mData,(wchar_t*)string.mData);
	if(result==NULL){
		return npos;
	}

	return result-(wchar_t*)mData;
}

int String::find(const char *text) const{
	const char *result=strstr(c_str(),text);
	if(result==NULL){
		return npos;
	}

	return result-c_str();
}

int String::internal_find(const stringchar *text) const{
	const wchar_t *result=wcsstr((wchar_t*)mData,(wchar_t*)text);
	if(result==NULL){
		return npos;
	}

	return result-(wchar_t*)mData;
}

int String::find(const String &string,int i) const{
	const wchar_t *result=wcsstr((wchar_t*)mData+i,(wchar_t*)string.mData);
	if(result==NULL){
		return npos;
	}

	return result-(wchar_t*)mData;
}

int String::find(const char *text,int i) const{
	const char *result=strstr(c_str()+i,text);
	if(result==NULL){
		return npos;
	}

	return result-c_str();
}

int String::internal_find(const stringchar *text,int i) const{
	const wchar_t *result=wcsstr((wchar_t*)mData+i,(wchar_t*)text);
	if(result==NULL){
		return npos;
	}

	return result-(wchar_t*)mData;
}

String String::substr(int start,int amt) const{
	if(start+amt>mLength){
		amt=mLength-start;
	}

	String s(amt);
	wcsncpy((wchar_t*)s.mData,(wchar_t*)mData+start,s.mLength);
	s.mData[s.mLength]=0;

	return s;
}

bool String::startsWith(const String &string) const{
	int len=string.mLength;
	return substr(0,len).equals(string);
}

bool String::endsWith(const String &string) const{
	int len=string.mLength;
	return substr(mLength-len,len).equals(string);
}

const char *String::c_str() const{
	if(mNarrowData==NULL){
		mNarrowData=new char[mLength+1];
		const stringchar *source=mData;
		char *dest=mNarrowData;
		while((*dest++=*source++)!=0);
	}
	return mNarrowData;
}

const stringchar *String::internal_wc_str() const{
	return mData;
}

String String::toLower() const{
	String string(*this);
	stringchar *p=string.mData;
	while(*p!=0){
		*p=tolower(*p);
		p++;
	}
	return string;
}

String String::toUpper() const{
	String string(*this);
	stringchar *p=string.mData;
	while(*p!=0){
		*p=toupper(*p);
		p++;
	}
	return string;
}

String String::trimLeft() const{
	// Find first non-whitespace
	int i;
	for(i=0;i<mLength && iswspace(mData[i]);++i);

	if(mLength-i>0){
		return substr(i,mLength-i);
	}
	else{
		return String();
	}
}

String String::trimRight() const{
	// Find last non-whitespace
	int i;
	for(i=mLength-1;i>=0 && iswspace(mData[i]);--i);

	if(i>=0){
		return substr(0,i+1);
	}
	else{
		return String();
	}
}

int32 String::toInt32() const{
	int32 i=0;
	TOADLET_SSCANF(this,"%d",&i);
	return i;
}

uint32 String::toUInt32() const{
	uint32 i=0;
	TOADLET_SSCANF(this,"%u",&i);
	return i;
}

int64 String::toInt64() const{
	int64 i=0;
	TOADLET_SSCANF(this,"%lld",&i);
	return i;
}

uint64 String::toUInt64() const{
	uint64 i=0;
	TOADLET_SSCANF(this,"%llu",&i);
	return i;
}

float String::toFloat() const{
	float f=0;
	TOADLET_SSCANF(this,"%f",&f);
	return f;
}

double String::toDouble() const{
	double f=0;
	TOADLET_SSCANF(this,"%Lf",&f);
	return f;
}

const String &String::operator=(const String &string){
	if(this==&string){
		return *this;
	}
	clearExtraData();
	mLength=string.mLength;
	delete[] mData;
	mData=new stringchar[mLength+1];
	wcsncpy((wchar_t*)mData,(wchar_t*)string.mData,mLength+1);
	return *this;
}

String String::operator+(const String &string) const{
	String s(mLength+string.mLength);
	wcsncpy((wchar_t*)s.mData,(wchar_t*)mData,mLength);
	wcsncpy((wchar_t*)s.mData+mLength,(wchar_t*)string.mData,string.mLength);
	s.mData[s.mLength]=0;
	return s;
}

String String::operator+(const char *text) const{
	if(text==NULL){
		return *this;
	}

	int len2=strlen(text);

	String s(mLength+len2);
	wcsncpy((wchar_t*)s.mData,(wchar_t*)mData,mLength);
	stringchar *dest=s.mData+mLength;
	while((*dest++=*text++)!=0);
	s.mData[s.mLength]=0;
	return s;
}

String String::internal_add(const stringchar *text) const{
	if(text==NULL){
		return *this;
	}

	int len2=wcslen((wchar_t*)text);

	String s(mLength+len2);
	wcsncpy((wchar_t*)s.mData,(wchar_t*)mData,mLength);
	wcsncpy((wchar_t*)s.mData+mLength,(wchar_t*)text,len2);
	s.mData[s.mLength]=0;
	return s;
}

String String::operator+(char c) const{
	String s(mLength+1);
	wcsncpy((wchar_t*)s.mData,(wchar_t*)mData,mLength);
	s.mData[mLength]=c;
	s.mData[s.mLength]=0;
	return s;
}

String String::internal_add(stringchar c) const{
	String s(mLength+1);
	wcsncpy((wchar_t*)s.mData,(wchar_t*)mData,mLength);
	s.mData[mLength]=c;
	s.mData[s.mLength]=0;
	return s;
}

String String::operator+(bool b) const{
	return operator+((int32)b);
}

String String::operator+(int32 i) const{
	#if defined(TOADLET_SNWPRINTF)
		stringchar buffer[128];
		TOADLET_SNWPRINTF((wchar_t*)buffer,128,L"%d",i);
		int size=wcslen((wchar_t*)buffer);

		String s(mLength+size);
		wcsncpy((wchar_t*)s.mData,(wchar_t*)mData,mLength);
		wcsncpy((wchar_t*)s.mData+mLength,(wchar_t*)buffer,size);
		s.mData[s.mLength]=0;
		return s;
	#else
		char buffer[128];
		snprintf(buffer,128,"%d",i);
		int size=strlen(buffer);

		String s(mLength+size);
		wcsncpy((wchar_t*)s.mData,(wchar_t*)mData,mLength);
		stringchar *dest=s.mData+mLength;
		char *src=buffer;
		while((*dest++=*src++)!=0);
		s.mData[s.mLength]=0;
		return s;
	#endif
}

String String::operator+(uint32 i) const{
	#if defined(TOADLET_SNWPRINTF)
		stringchar buffer[128];
		TOADLET_SNWPRINTF((wchar_t*)buffer,128,L"%u",i);
		int size=wcslen((wchar_t*)buffer);

		String s(mLength+size);
		wcsncpy((wchar_t*)s.mData,(wchar_t*)mData,mLength);
		wcsncpy((wchar_t*)s.mData+mLength,(wchar_t*)buffer,size);
		s.mData[s.mLength]=0;
		return s;
	#else
		char buffer[128];
		snprintf(buffer,128,"%u",i);
		int size=strlen(buffer);

		String s(mLength+size);
		wcsncpy((wchar_t*)s.mData,(wchar_t*)mData,mLength);
		stringchar *dest=s.mData+mLength;
		char *src=buffer;
		while((*dest++=*src++)!=0);
		s.mData[s.mLength]=0;
		return s;
	#endif
}


String String::operator+(int64 i) const{
	#if defined(TOADLET_SNWPRINTF)
		stringchar buffer[128];
		TOADLET_SNWPRINTF((wchar_t*)buffer,128,L"%lld",i);
		int size=wcslen((wchar_t*)buffer);

		String s(mLength+size);
		wcsncpy((wchar_t*)s.mData,(wchar_t*)mData,mLength);
		wcsncpy((wchar_t*)s.mData+mLength,(wchar_t*)buffer,size);
		s.mData[s.mLength]=0;
		return s;
	#else
		char buffer[128];
		snprintf(buffer,128,"%lld",i);
		int size=strlen(buffer);

		String s(mLength+size);
		wcsncpy((wchar_t*)s.mData,(wchar_t*)mData,mLength);
		stringchar *dest=s.mData+mLength;
		char *src=buffer;
		while((*dest++=*src++)!=0);
		s.mData[s.mLength]=0;
		return s;
	#endif
}

String String::operator+(uint64 i) const{
	#if defined(TOADLET_SNWPRINTF)
		stringchar buffer[128];
		TOADLET_SNWPRINTF((wchar_t*)buffer,128,L"%llu",i);
		int size=wcslen((wchar_t*)buffer);

		String s(mLength+size);
		wcsncpy((wchar_t*)s.mData,(wchar_t*)mData,mLength);
		wcsncpy((wchar_t*)s.mData+mLength,(wchar_t*)buffer,size);
		s.mData[s.mLength]=0;
		return s;
	#else
		char buffer[128];
		snprintf(buffer,128,"%llu",i);
		int size=strlen(buffer);

		String s(mLength+size);
		wcsncpy((wchar_t*)s.mData,(wchar_t*)mData,mLength);
		stringchar *dest=s.mData+mLength;
		char *src=buffer;
		while((*dest++=*src++)!=0);
		s.mData[s.mLength]=0;
		return s;
	#endif
}

String String::operator+(float f) const{
	#if defined(TOADLET_SNWPRINTF)
		stringchar buffer[128];
		TOADLET_SNWPRINTF((wchar_t*)buffer,128,L"%f",f);
		int size=wcslen((wchar_t*)buffer);

		String s(mLength+size);
		wcsncpy((wchar_t*)s.mData,(wchar_t*)mData,mLength);
		wcsncpy((wchar_t*)s.mData+mLength,(wchar_t*)buffer,size);
		s.mData[s.mLength]=0;
		return s;
	#else
		char buffer[128];
		snprintf(buffer,128,"%f",f);
		int size=strlen(buffer);

		String s(mLength+size);
		wcsncpy((wchar_t*)s.mData,(wchar_t*)mData,mLength);
		stringchar *dest=s.mData+mLength;
		char *src=buffer;
		while((*dest++=*src++)!=0);
		s.mData[s.mLength]=0;
		return s;
	#endif
}

String String::operator+(double f) const{
	#if defined(TOADLET_SNWPRINTF)
		stringchar buffer[128];
		TOADLET_SNWPRINTF((wchar_t*)buffer,128,L"%Lf",f);
		int size=wcslen((wchar_t*)buffer);

		String s(mLength+size);
		wcsncpy((wchar_t*)s.mData,(wchar_t*)mData,mLength);
		wcsncpy((wchar_t*)s.mData+mLength,(wchar_t*)buffer,size);
		s.mData[s.mLength]=0;
		return s;
	#else
		char buffer[128];
		snprintf(buffer,128,"%Lf",f);
		int size=strlen(buffer);

		String s(mLength+size);
		wcsncpy((wchar_t*)s.mData,(wchar_t*)mData,mLength);
		stringchar *dest=s.mData+mLength;
		char *src=buffer;
		while((*dest++=*src++)!=0);
		s.mData[s.mLength]=0;
		return s;
	#endif
}

void String::operator+=(const String &string){
	clearExtraData();

	stringchar *data=new stringchar[mLength+string.mLength+1];
	wcsncpy((wchar_t*)data,(wchar_t*)mData,mLength);
	wcsncpy((wchar_t*)data+mLength,(wchar_t*)string.mData,string.mLength);
	int length=mLength+string.mLength;
	data[length]=0;

	delete[] mData;
	mData=data;
	mLength=length;
}

void String::operator+=(const char *text){
	if(text==NULL){
		return;
	}

	clearExtraData();

	int len2=strlen(text);

	stringchar *data=new stringchar[mLength+len2+1];
	wcsncpy((wchar_t*)data,(wchar_t*)mData,mLength);
	stringchar *dest=data+mLength;
	while((*dest++=*text++)!=0);
	int length=mLength+len2;
	data[length]=0;

	delete[] mData;
	mData=data;
	mLength=length;
}

void String::internal_addassign(const stringchar *text){
	if(text==NULL){
		return;
	}

	clearExtraData();

	int len2=wcslen((wchar_t*)text);

	stringchar *data=new stringchar[mLength+len2+1];
	wcsncpy((wchar_t*)data,(wchar_t*)mData,mLength);
	wcsncpy((wchar_t*)data+mLength,(wchar_t*)text,len2);
	int length=mLength+len2;
	data[length]=0;

	delete[] mData;
	mData=data;
	mLength=length;
}

bool String::equals(const String &string) const{
	return wcscmp((wchar_t*)mData,(wchar_t*)string.mData)==0;
}

bool String::operator!=(const String &string) const{
	return wcscmp((wchar_t*)mData,(wchar_t*)string.mData)!=0;
}

bool String::equals(const char *text) const{
	if(text==NULL){
		return mLength==0;
	}

	stringchar *data=mData;
	int i=0;
	while (data[i]==text[i] && data[i]!=0){
		i++;
	}
	return (data[i]-text[i])==0;
}

bool String::operator!=(const char *text) const{
	if(text==NULL){
		return mLength!=0;
	}

	stringchar *data=mData;
	int i=0;
	while (data[i]==text[i] && data[i]!=0){
		i++;
	}
	return (data[i]-text[i])!=0;
}

bool String::internal_equals(const stringchar *text) const{
	if(text==NULL){
		return mLength!=0;
	}

	return wcscmp((wchar_t*)mData,(wchar_t*)text)==0;
}

bool String::internal_notequals(const stringchar *text) const{
	if(text==NULL){
		return mLength!=0;
	}

	return wcscmp((wchar_t*)mData,(wchar_t*)text)!=0;
}

bool String::operator<(const String &string) const{
	return (wcscmp((wchar_t*)mData,(wchar_t*)string.mData)<0);
}

void String::clearExtraData(){
	if(mNarrowData!=NULL){
		delete[] mNarrowData;
		mNarrowData=NULL;
	}
}

String operator+(const char *text,const String &string){
	if(text==NULL){
		return string;
	}

	int len2=strlen(text);

	String s(string.mLength+len2);
	stringchar *dest=s.mData;
	while((*dest++=*text++)!=0);
	wcsncpy((wchar_t*)s.mData+len2,(wchar_t*)string.mData,string.mLength);
	s.mData[s.mLength]=0;
	return s;
}

String internal_add(const stringchar *text,const String &string){
	if(text==NULL){
		return string;
	}

	int len2=wcslen((wchar_t*)text);

	String s(string.mLength+len2);
	wcsncpy((wchar_t*)s.mData,(wchar_t*)text,len2);
	wcsncpy((wchar_t*)s.mData+len2,(wchar_t*)string.mData,string.mLength);
	s.mData[s.mLength]=0;
	return s;
}

String operator+(char c,const String &string){
	String s(string.mLength+1);
	s.mData[0]=c;
	wcsncpy((wchar_t*)s.mData+1,(wchar_t*)string.mData,string.mLength);
	s.mData[s.mLength]=0;
	return s;
}

String internal_add(stringchar c,const String &string){
	String s(string.mLength+1);
	s.mData[0]=c;
	wcsncpy((wchar_t*)s.mData+1,(wchar_t*)string.mData,string.mLength);
	s.mData[s.mLength]=0;
	return s;
}

String operator+(bool b,const String &string){
	return operator+((int32)b,string);
}

String operator+(int32 i,const String &string){
	#if defined(TOADLET_SNWPRINTF)
		stringchar buffer[128];
		TOADLET_SNWPRINTF((wchar_t*)buffer,128,L"%d",i);
		int size=wcslen((wchar_t*)buffer);

		String s(string.mLength+size);
		wcsncpy((wchar_t*)s.mData,(wchar_t*)buffer,size);
		wcsncpy((wchar_t*)s.mData+size,(wchar_t*)string.mData,string.mLength);
		s.mData[s.mLength]=0;
		return s;
	#else
		char buffer[128];
		snprintf(buffer,128,"%d",i);
		int size=strlen(buffer);

		String s(string.mLength+size);
		stringchar *dest=s.mData;
		char *src=buffer;
		while((*dest++=*src++)!=0);
		wcsncpy((wchar_t*)s.mData+size,(wchar_t*)string.mData,string.mLength);
		s.mData[s.mLength]=0;
		return s;
	#endif
}

String operator+(uint32 i,const String &string){
	#if defined(TOADLET_SNWPRINTF)
		stringchar buffer[128];
		TOADLET_SNWPRINTF((wchar_t*)buffer,128,L"%u",i);
		int size=wcslen((wchar_t*)buffer);

		String s(string.mLength+size);
		wcsncpy((wchar_t*)s.mData,(wchar_t*)buffer,size);
		wcsncpy((wchar_t*)s.mData+size,(wchar_t*)string.mData,string.mLength);
		s.mData[s.mLength]=0;
		return s;
	#else
		char buffer[128];
		snprintf(buffer,128,"%u",i);
		int size=strlen(buffer);

		String s(string.mLength+size);
		stringchar *dest=s.mData;
		char *src=buffer;
		while((*dest++=*src++)!=0);
		wcsncpy((wchar_t*)s.mData+size,(wchar_t*)string.mData,string.mLength);
		s.mData[s.mLength]=0;
		return s;
	#endif
}

String operator+(int64 i,const String &string){
	#if defined(TOADLET_SNWPRINTF)
		stringchar buffer[128];
		TOADLET_SNWPRINTF((wchar_t*)buffer,128,L"%lld",i);
		int size=wcslen((wchar_t*)buffer);

		String s(string.mLength+size);
		wcsncpy((wchar_t*)s.mData,(wchar_t*)buffer,size);
		wcsncpy((wchar_t*)s.mData+size,(wchar_t*)string.mData,string.mLength);
		s.mData[s.mLength]=0;
		return s;
	#else
		char buffer[128];
		snprintf(buffer,128,"%lld",i);
		int size=strlen(buffer);

		String s(string.mLength+size);
		stringchar *dest=s.mData;
		char *src=buffer;
		while((*dest++=*src++)!=0);
		wcsncpy((wchar_t*)s.mData+size,(wchar_t*)string.mData,string.mLength);
		s.mData[s.mLength]=0;
		return s;
	#endif
}

String operator+(uint64 i,const String &string){
	#if defined(TOADLET_SNWPRINTF)
		stringchar buffer[128];
		TOADLET_SNWPRINTF((wchar_t*)buffer,128,L"%llu",i);
		int size=wcslen((wchar_t*)buffer);

		String s(string.mLength+size);
		wcsncpy((wchar_t*)s.mData,(wchar_t*)buffer,size);
		wcsncpy((wchar_t*)s.mData+size,(wchar_t*)string.mData,string.mLength);
		s.mData[s.mLength]=0;
		return s;
	#else
		char buffer[128];
		snprintf(buffer,128,"%llu",i);
		int size=strlen(buffer);

		String s(string.mLength+size);
		stringchar *dest=s.mData;
		char *src=buffer;
		while((*dest++=*src++)!=0);
		wcsncpy((wchar_t*)s.mData+size,(wchar_t*)string.mData,string.mLength);
		s.mData[s.mLength]=0;
		return s;
	#endif
}

String operator+(float f,const String &string){
	#if defined(TOADLET_SNWPRINTF)
		stringchar buffer[128];
		TOADLET_SNWPRINTF((wchar_t*)buffer,128,L"%f",f);
		int size=wcslen((wchar_t*)buffer);

		String s(string.mLength+size);
		wcsncpy((wchar_t*)s.mData,(wchar_t*)buffer,size);
		wcsncpy((wchar_t*)s.mData+size,(wchar_t*)string.mData,string.mLength);
		s.mData[s.mLength]=0;
		return s;
	#else
		char buffer[128];
		snprintf(buffer,128,"%f",f);
		int size=strlen(buffer);

		String s(string.mLength+size);
		stringchar *dest=s.mData;
		char *src=buffer;
		while((*dest++=*src++)!=0);
		wcsncpy((wchar_t*)s.mData+size,(wchar_t*)string.mData,string.mLength);
		s.mData[s.mLength]=0;
		return s;
	#endif
}

String operator+(double f,const String &string){
	#if defined(TOADLET_SNWPRINTF)
		stringchar buffer[128];
		TOADLET_SNWPRINTF((wchar_t*)buffer,128,L"%Lf",f);
		int size=wcslen((wchar_t*)buffer);

		String s(string.mLength+size);
		wcsncpy((wchar_t*)s.mData,(wchar_t*)buffer,size);
		wcsncpy((wchar_t*)s.mData+size,(wchar_t*)string.mData,string.mLength);
		s.mData[s.mLength]=0;
		return s;
	#else
		char buffer[128];
		snprintf(buffer,128,"%Lf",f);
		int size=strlen(buffer);

		String s(string.mLength+size);
		stringchar *dest=s.mData;
		char *src=buffer;
		while((*dest++=*src++)!=0);
		wcsncpy((wchar_t*)s.mData+size,(wchar_t*)string.mData,string.mLength);
		s.mData[s.mLength]=0;
		return s;
	#endif
}

}
}
