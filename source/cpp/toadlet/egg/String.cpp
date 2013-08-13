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

#include <toadlet/egg/Assert.h>
#include <toadlet/egg/String.h>
#include <stdio.h>
#include <ctype.h>

#if defined(TOADLET_PLATFORM_POSIX)
	#include <wchar.h>
	#include <wctype.h>
#endif

/// @todo: This crashes for some reason
//#define TOADLET_HAS_SWPRINTF
#if defined(TOADLET_HAS_SWPRINTF)
	#define TOADLET_SPRINTF(str,fmt,x) \
		str.mLength=swprintf((wchar_t*)(str).mData,(str).mLength,L##fmt,x)
	#define TOADLET_SSCANF(str,fmt,x) \
		swscanf((wchar_t*)(str).mData,L##fmt,x)
#else
	#define TOADLET_SPRINTF(str,fmt,x) \
		char temp[128]; \
		str.mLength=sprintf(temp,fmt,x); \
		char *src=temp; \
		stringchar *dst=(str).mData; \
		while((*dst++=*src++)!=0);
	#define TOADLET_SSCANF(str,fmt,x) \
		char temp[128]; \
		stringchar *src=(str).mData; \
		char *dst=temp; \
		while((*dst++=*src++)!=0); \
		sscanf(temp,fmt,x)
#endif

// wcstr functions seem to be broken on android
#if defined(TOADLET_PLATFORM_ANDROID)
	inline int toadlet_wcslen(wchar_t *str){
		int i=0;
		while(str[i]!=0) i++;
		return i;
	}
	#define TOADLET_WCSLEN(str) toadlet_wcslen(str)

	inline void toadlet_wcsncpy(wchar_t *dst,wchar_t *src,int len){
		int i;
		for(i=0;i<len;++i){
			dst[i]=src[i];
		}
	}
	#define TOADLET_WCSNCPY(dst,src,len) toadlet_wcsncpy(dst,src,len)

	inline int toadlet_wcscmp(wchar_t *s1,wchar_t *s2){
		while(*s1==*s2){
			if(*s1==0) return 0;
			++s1;++s2;
		}
		return *s1<*s2?-1:1;
	}
	#define TOADLET_WCSCMP(s1,s2) toadlet_wcscmp(s1,s2)
#else
	#define TOADLET_WCSLEN wcslen
	#define TOADLET_WCSNCPY wcsncpy
	#define TOADLET_WCSCMP wcscmp
#endif

namespace toadlet{
namespace egg{

stringchar String::mNull=0;

String::String(){
	mData=&mNull;
	mLength=0;
	mNarrowData=NULL;
}

void String::internal_String(const char *text){
	if(text==NULL){
		mLength=0;
		mData=&mNull;
		mNarrowData=NULL;
		return;
	}

	mLength=strlen(text);
	mData=new stringchar[mLength+1];
	stringchar *dest=mData;
	while((*dest++=*text++)!=0);
	mNarrowData=NULL;

	updatec();
}

void String::internal_String(const unsigned char *text){
	if(text==NULL){
		mLength=0;
		mData=&mNull;
		mNarrowData=NULL;
		return;
	}

	mLength=strlen((const char*)text);
	mData=new stringchar[mLength+1];
	stringchar *dest=mData;
	while((*dest++=*text++)!=0);
	mNarrowData=NULL;

	updatec();
}

void String::internal_String(const stringchar *text){
	if(text==NULL){
		mLength=0;
		mData=&mNull;
		mNarrowData=NULL;
		return;
	}

	mLength=TOADLET_WCSLEN((wchar_t*)text);
	mData=new stringchar[mLength+1];
	TOADLET_WCSNCPY((wchar_t*)mData,(wchar_t*)text,mLength+1);
	mNarrowData=NULL;

	updatec();
}

void String::internal_String(const Collection<tbyte> &text){
	mLength=text.size();
	mData=new stringchar[mLength+1];
	for(int i=0;i<mLength;++i)mData[i]=text[i];
	mData[mLength]=0;
	mNarrowData=NULL;

	updatec();
}

String::String(const String &string){
	mLength=string.mLength;
	mData=new stringchar[mLength+1];
	TOADLET_WCSNCPY((wchar_t*)mData,(wchar_t*)string.mData,mLength+1);
	mNarrowData=NULL;

	updatec();
}

String::String(int length){
	mLength=length;
	mData=new stringchar[mLength+1];
	mNarrowData=NULL;
}

String::~String(){
	delete[] mNarrowData;
	if(mData!=&mNull){
		delete[] mData;
	}
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
	if(start<0){
		start=0;
	}
	else if(start>=mLength){
		start=mLength;
	}

	if(amt<0){
		amt=mLength-start;
	}
	else if(start+amt>mLength){
		amt=mLength-start;
	}

	String s(amt);

	TOADLET_WCSNCPY((wchar_t*)s.mData,(wchar_t*)mData+start,s.mLength);
	s.mData[s.mLength]=0;

	s.updatec();

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

Collection<tbyte> String::data() const{
	Collection<tbyte> text(mLength);
	for(int i=0;i<mLength;++i)text[i]=mData[i];
	return text;
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
	TOADLET_SSCANF(*this,"%d",&i);
	return i;
}

uint32 String::toUInt32() const{
	uint32 i=0;
	TOADLET_SSCANF(*this,"%u",&i);
	return i;
}

int64 String::toInt64() const{
	int64 i=0;
	TOADLET_SSCANF(*this,"%lld",&i);
	return i;
}

uint64 String::toUInt64() const{
	uint64 i=0;
	TOADLET_SSCANF(*this,"%llu",&i);
	return i;
}

float String::toFloat() const{
	float f=0;
	TOADLET_SSCANF(*this,"%f",&f);
	return f;
}

double String::toDouble() const{
	long double f=0;
	TOADLET_SSCANF(*this,"%Lf",&f);
	return f;
}

const String &String::operator=(const String &string){
	if(this==&string){
		return *this;
	}

	clearc();

	mLength=string.mLength;
	if(mData!=&mNull){
		delete[] mData;
	}
	mData=new stringchar[mLength+1];
	TOADLET_WCSNCPY((wchar_t*)mData,(wchar_t*)string.mData,mLength+1);

	updatec();

	return *this;
}

String String::operator+(const String &string) const{
	String s(mLength+string.mLength);

	TOADLET_WCSNCPY((wchar_t*)s.mData,(wchar_t*)mData,mLength);
	TOADLET_WCSNCPY((wchar_t*)s.mData+mLength,(wchar_t*)string.mData,string.mLength);
	s.mData[s.mLength]=0;

	s.updatec();

	return s;
}

String String::operator+(const char *text) const{
	if(text==NULL){
		return *this;
	}

	int len2=strlen(text);

	String s(mLength+len2);

	TOADLET_WCSNCPY((wchar_t*)s.mData,(wchar_t*)mData,mLength);
	stringchar *dest=s.mData+mLength;
	while((*dest++=*text++)!=0);
	s.mData[s.mLength]=0;

	s.updatec();

	return s;
}

String String::internal_add(const stringchar *text) const{
	if(text==NULL){
		return *this;
	}

	int len2=TOADLET_WCSLEN((wchar_t*)text);

	String s(mLength+len2);

	TOADLET_WCSNCPY((wchar_t*)s.mData,(wchar_t*)mData,mLength);
	TOADLET_WCSNCPY((wchar_t*)s.mData+mLength,(wchar_t*)text,len2);
	s.mData[s.mLength]=0;

	s.updatec();

	return s;
}

String String::operator+(char c) const{
	String s(mLength+1);

	TOADLET_WCSNCPY((wchar_t*)s.mData,(wchar_t*)mData,mLength);
	s.mData[mLength]=c;
	s.mData[s.mLength]=0;

	s.updatec();

	return s;
}

String String::internal_add(stringchar c) const{
	String s(mLength+1);

	TOADLET_WCSNCPY((wchar_t*)s.mData,(wchar_t*)mData,mLength);
	s.mData[mLength]=c;
	s.mData[s.mLength]=0;
	
	s.updatec();

	return s;
}

String String::operator+(bool b) const{
	return operator+((int32)b);
}

String String::operator+(int32 i) const{
	String result(128);
	TOADLET_SPRINTF(result,"%d",i);
	return (*this)+result;
}

String String::operator+(uint32 i) const{
	String result(128);
	TOADLET_SPRINTF(result,"%u",i);
	return (*this)+result;
}

String String::operator+(int64 i) const{
	String result(128);
	TOADLET_SPRINTF(result,"%lld",i);
	return (*this)+result;
}

String String::operator+(uint64 i) const{
	String result(128);
	TOADLET_SPRINTF(result,"%llu",i);
	return (*this)+result;
}

String String::operator+(float f) const{
	String result(128);
	TOADLET_SPRINTF(result,"%f",f);
	return (*this)+result;
}

String String::operator+(double f) const{
	String result(128);
	TOADLET_SPRINTF(result,"%lf",f);
	return (*this)+result;
}

void String::operator+=(const String &string){
	clearc();

	stringchar *data=new stringchar[mLength+string.mLength+1];
	TOADLET_WCSNCPY((wchar_t*)data,(wchar_t*)mData,mLength);
	TOADLET_WCSNCPY((wchar_t*)data+mLength,(wchar_t*)string.mData,string.mLength);
	int length=mLength+string.mLength;
	data[length]=0;

	if(mData!=&mNull){
		delete[] mData;
	}
	mData=data;
	mLength=length;

	updatec();
}

void String::operator+=(const char *text){
	if(text==NULL){
		return;
	}

	clearc();

	int len2=strlen(text);

	stringchar *data=new stringchar[mLength+len2+1];
	TOADLET_WCSNCPY((wchar_t*)data,(wchar_t*)mData,mLength);
	stringchar *dest=data+mLength;
	while((*dest++=*text++)!=0);
	int length=mLength+len2;
	data[length]=0;

	if(mData!=&mNull){
		delete[] mData;
	}
	mData=data;
	mLength=length;

	updatec();
}

void String::internal_addassign(const stringchar *text){
	if(text==NULL){
		return;
	}

	clearc();

	int len2=TOADLET_WCSLEN((wchar_t*)text);

	stringchar *data=new stringchar[mLength+len2+1];
	TOADLET_WCSNCPY((wchar_t*)data,(wchar_t*)mData,mLength);
	TOADLET_WCSNCPY((wchar_t*)data+mLength,(wchar_t*)text,len2);
	int length=mLength+len2;
	data[length]=0;

	if(mData!=&mNull){
		delete[] mData;
	}
	mData=data;
	mLength=length;

	updatec();
}

bool String::equals(const String &string) const{
	if(mData==NULL){
		return string.mLength==0;
	}
	if(string.mData==NULL){
		return mLength==0;
	}

	return TOADLET_WCSCMP((wchar_t*)mData,(wchar_t*)string.mData)==0;
}

bool String::equals(const char *text) const{
	if(mData==NULL){
		return text==NULL || *text==0;
	}
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

bool String::internal_equals(const stringchar *text) const{
	if(mData==NULL){
		return text==NULL || *text==0;
	}
	if(text==NULL){
		return mLength==0;
	}

	return TOADLET_WCSCMP((wchar_t*)mData,(wchar_t*)text)==0;
}

bool String::operator<(const String &string) const{
	return (TOADLET_WCSCMP((wchar_t*)mData,(wchar_t*)string.mData)<0);
}

void String::clearc(){
	delete[] mNarrowData;
	mNarrowData=NULL;
}

void String::updatec(){
	mNarrowData=new char[mLength+1];
	const stringchar *source=mData;
	char *dest=mNarrowData;
	while((*dest++=*source++)!=0);
}

String operator+(const char *text,const String &string){
	if(text==NULL){
		return string;
	}

	int len2=strlen(text);

	String s(string.mLength+len2);

	stringchar *dest=s.mData;
	while((*dest++=*text++)!=0);
	TOADLET_WCSNCPY((wchar_t*)s.mData+len2,(wchar_t*)string.mData,string.mLength);
	s.mData[s.mLength]=0;

	s.updatec();

	return s;
}

String internal_add(const stringchar *text,const String &string){
	if(text==NULL){
		return string;
	}

	int len2=TOADLET_WCSLEN((wchar_t*)text);

	String s(string.mLength+len2);

	TOADLET_WCSNCPY((wchar_t*)s.mData,(wchar_t*)text,len2);
	TOADLET_WCSNCPY((wchar_t*)s.mData+len2,(wchar_t*)string.mData,string.mLength);
	s.mData[s.mLength]=0;

	s.updatec();

	return s;
}

String operator+(char c,const String &string){
	String s(string.mLength+1);

	s.mData[0]=c;
	TOADLET_WCSNCPY((wchar_t*)s.mData+1,(wchar_t*)string.mData,string.mLength);
	s.mData[s.mLength]=0;

	s.updatec();

	return s;
}

String internal_add(stringchar c,const String &string){
	String s(string.mLength+1);

	s.mData[0]=c;
	TOADLET_WCSNCPY((wchar_t*)s.mData+1,(wchar_t*)string.mData,string.mLength);
	s.mData[s.mLength]=0;

	s.updatec();

	return s;
}

String operator+(bool b,const String &string){
	return operator+((int32)b,string);
}

String operator+(int32 i,const String &string){
	String result(128);
	TOADLET_SPRINTF(result,"%d",i);
	return result+string;
}

String operator+(uint32 i,const String &string){
	String result(128);
	TOADLET_SPRINTF(result,"%u",i);
	return result+string;
}

String operator+(int64 i,const String &string){
	String result(128);
	TOADLET_SPRINTF(result,"%lld",i);
	return result+string;
}

String operator+(uint64 i,const String &string){
	String result(128);
	TOADLET_SPRINTF(result,"%llu",i);
	return result+string;
}

String operator+(float f,const String &string){
	String result(128);
	TOADLET_SPRINTF(result,"%f",f);
	return result+string;
}

String operator+(double f,const String &string){
	String result(128);
	TOADLET_SPRINTF(result,"%lf",f);
	return result+string;
}

}
}
