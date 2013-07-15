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

#ifndef TOADLET_EGG_STRING_H
#define TOADLET_EGG_STRING_H

#include <toadlet/egg/Types.h>
#include <toadlet/egg/Collection.h>
#if defined(__OBJC__)
	#import <Foundation/Foundation.h>
#endif

namespace toadlet{
namespace egg{

// String externally deals in char & wchar, but internally in char & stringchar
// since we need to let toadlet work with both compilers where wchar_t is defined
// and those where it is undefined, without needing to recompile the library.
// So wchar references must reside in the header only, not be go through to the cpp.

#if TOADLET_SIZEOF_WCHAR==16
	typedef int16 stringchar;
#elif TOADLET_SIZEOF_WCHAR==32
	typedef int32 stringchar;
#else
	#error "Unknown wchar size"
#endif

class TOADLET_API String{
public:
	static const int npos=-1;

	String();
	void internal_String(const char *text);
	inline String(const char *text){internal_String(text);}
	void internal_String(const unsigned char *text);
	inline String(const unsigned char *text){internal_String(text);}
	void internal_String(const stringchar *text);
	inline String(const wchar *text){internal_String((const stringchar*)text);}
	void internal_String(const Collection<tbyte> &text);
	inline String(const Collection<tbyte> &text){internal_String(text);}
	#if defined(__OBJC__)
		inline String(NSString *string){internal_String([string UTF8String]);}
	#endif
	String(const String &string);
	explicit String(int length);

	~String();

	int find(char c) const;

	inline int find(unsigned char c) const{return find((char)c);}

	int internal_find(stringchar c) const;
	inline int find(wchar c) const{return internal_find((stringchar)c);}

	int find(char c,int i) const;

	inline int find(unsigned char c,int i) const{return find((char)c,i);}

	int internal_find(stringchar c,int i) const;
	inline int find(wchar c,int i) const{return internal_find((stringchar)c,i);}

	int rfind(char c) const;

	inline int rfind(unsigned char c) const{return rfind((char)c);}

	int internal_rfind(stringchar c) const;
	inline int rfind(wchar c) const{return internal_rfind((stringchar)c);}

	int rfind(char c,int i) const;

	inline int rfind(unsigned char c,int i) const{return rfind((char)c,i);}

	int internal_rfind(stringchar c,int i) const;
	inline int rfind(wchar c,int i) const{return internal_rfind((stringchar)c,i);}

	int find(const String &string) const;

	int find(const char *text) const;

	int internal_find(const stringchar *text) const;
	inline int find(const wchar *text) const{return internal_find((stringchar*)text);}

	int find(const String &string,int i) const;

	int find(const char *text,int i) const;

	int internal_find(const stringchar *text,int i) const;
	inline int find(const wchar *text,int i) const{return internal_find((stringchar*)text,i);}

	String substr(int start,int amt) const;

	bool startsWith(const String &string) const;
	bool endsWith(const String &string) const;

	inline int length() const{return mLength;}

	const char *c_str() const;
	inline operator const char *() const{return c_str();}

	const stringchar *internal_wc_str() const{return mData;}
	inline const wchar *wc_str() const{return (const wchar*)internal_wc_str();}
	inline operator const wchar *() const{return wc_str();}

	#if defined(__OBJC__)
		inline operator NSString *() const{return [NSString stringWithUTF8String:c_str()];}
	#endif

	Collection<tbyte> data() const;
	inline operator Collection<tbyte> () const{return data();}

	inline wchar at(int i) const{return mData[i];}

	String toLower() const;

	String toUpper() const;

	String trimLeft() const;

	String trimRight() const;

	int32 toInt32() const;

	uint32 toUInt32() const;

	int64 toInt64() const;

	uint64 toUInt64() const;

	float toFloat() const;

	double toDouble() const;

	const String &operator=(const String &string);

	String operator+(const String &string) const;

	String operator+(const char *text) const;

	inline String operator+(const unsigned char *text) const{return operator+((const char*)text);}

	String internal_add(const stringchar *text) const;
	inline String operator+(const wchar *text) const{return internal_add((const stringchar*)text);}

	String operator+(char c) const;

	inline String operator+(unsigned char c) const{return operator+((char)c);}

	String internal_add(stringchar c) const;
	inline String operator+(wchar c) const{return internal_add((stringchar)c);}

	String operator+(bool b) const;

	String operator+(int32 i) const;

	String operator+(uint32 i) const;

	String operator+(int64 i) const;

	String operator+(uint64 i) const;

	String operator+(float f) const;

	String operator+(double f) const;

	void operator+=(const String &string);

	void operator+=(const char *text);

	inline void operator+=(const unsigned char *text){operator+=((const char*)text);}

	void internal_addassign(const stringchar *text);
	inline void operator+=(const wchar *text){internal_addassign((const stringchar*)text);}

	friend TOADLET_API String operator+(const char *text,const String &string);

	friend TOADLET_API String operator+(const unsigned char *text,const String &string);

	friend TOADLET_API String internal_add(const stringchar *text,const String &string);

	friend TOADLET_API String operator+(char c,const String &string);

	friend TOADLET_API String operator+(unsigned char c,const String &string);

	friend TOADLET_API String internal_add(stringchar c,const String &string);

	friend TOADLET_API String operator+(bool b,const String &string);

	friend TOADLET_API String operator+(int32 i,const String &string);

	friend TOADLET_API String operator+(uint32 i,const String &string);

	friend TOADLET_API String operator+(int64 i,const String &string);

	friend TOADLET_API String operator+(uint64 i,const String &string);

	friend TOADLET_API String operator+(float f,const String &string);

	friend TOADLET_API String operator+(double f,const String &string);

	bool equals(const String &string) const;
	inline bool operator==(const String &string) const{return equals(string);}
	bool operator!=(const String &string) const;

	bool equals(const char *text) const;
	inline bool operator==(const char *text) const{return equals(text);}
	bool operator!=(const char *text) const;

	inline bool equals(const unsigned char *text) const{return equals((const char*)text);}
	inline bool operator==(const unsigned char *text) const{return equals((const char*)text);}
	inline bool operator!=(const unsigned char *text) const{return operator!=((const char*)text);}

	bool internal_equals(const stringchar *text) const;
	bool internal_notequals(const stringchar *text) const;
	inline bool equals(const wchar *text) const{return internal_equals((const stringchar*)text);}
	inline bool operator==(const wchar *text) const{return internal_equals((const stringchar*)text);}
	inline bool operator!=(const wchar *text) const{return internal_notequals((const stringchar*)text);}

	bool operator<(const String &string) const;

	inline wchar &operator[](int i){return (wchar&)mData[i];}
	inline wchar operator[](int i) const{return (wchar)mData[i];}

private:
	void clearExtraData();
	void update();

	stringchar *mData;
	int mLength;
	static stringchar mNull;

	mutable char *mNarrowData;
};

TOADLET_API String operator+(const char *text,const String &string);

inline String operator+(const unsigned char *text,const String &string){return operator+((const char*)text,string);}

TOADLET_API String internal_add(const stringchar *text,const String &string);
inline String operator+(const wchar *text,const String &string){return internal_add((const stringchar*)text,string);}

TOADLET_API String operator+(char c,const String &string);

inline String operator+(unsigned char c,const String &string){return operator+((char)c,string);}

TOADLET_API String internal_add(stringchar c,const String &string);
inline String operator+(wchar c,const String &string){return internal_add((stringchar)c,string);}

TOADLET_API String operator+(int i,const String &string);

TOADLET_API String operator+(unsigned int i,const String &string);

TOADLET_API String operator+(float f,const String &string);

}
}

#endif

