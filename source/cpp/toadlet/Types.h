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

#ifndef TOADLET_TYPES_H
#define TOADLET_TYPES_H

#include <toadlet/Config.h>

#if !defined(TOADLET_DEBUG) && (defined(DEBUG) || defined(_DEBUG))
	#define TOADLET_DEBUG 1
#endif

#if defined(TOADLET_DEBUG)
//	#define TOADLET_PROFILE 1
#endif

// Set platform specific defines
#if defined(TOADLET_PLATFORM_WIN32)
	#if defined(_MSC_VER)
		#define TOADLET_COMPILER_VC 1
		#if(_MSC_VER>=1400)
			#define TOADLET_COMPILER_VC8 1
		#elif(_MSC_VER>=1310)
			#define TOADLET_COMPILER_VC71 1
		#elif(_MSC_VER>=1300)
			#define TOADLET_COMPILER_VC7 1
		#elif(_MSC_VER>=1200)
			#define TOADLET_COMPILER_VC6 1
		#elif(_MSC_VER>=1100)
			#define TOADLET_COMPILER_VC5 1
		#endif
	#endif

	#pragma warning(disable:4244)
	#pragma warning(disable:4786) // identifier was truncated to '255' characters in the debug information
	#pragma warning(disable:4305) // truncation from 'const double' to 'float'
	#pragma warning(disable:4503) // decorated name length exceeded, name was truncated
	#pragma warning(disable:4250) // inherits via dominance
	#pragma warning(disable:4251) // base class not declared with the __declspec(dllexport) keyword
	#pragma warning(disable:4275) // exported class derived from a class that was not exported.
	#pragma warning(disable:4290) // C++ exception specification ignored except to indicate a function is not __declspec(nothrow)
	#pragma warning(1:4296) // expression is always true/false
	#pragma warning(1:4668) // a symbol that was not defined was used with a preprocessor directive. The symbol will evaluate to false.
	#pragma warning(1:4287 4289 4302)
	#pragma warning(disable:4018) // signed/unsigned mismatch
	#pragma warning(disable:4668) // is not defined as a preprocessor macro
	#pragma warning(disable:4284) // return type is _ not _ (ie; not a UDT or reference to a UDT.  Will produce errors if applied using infix notation)
	#pragma warning(disable:4201) // nonstandard extension is used, nameless struct or union
	#pragma warning(disable:4127) // conditional expression is constant
	#pragma warning(disable:4100) // unreferenced formal parameter
	#pragma warning(disable:4005) // macro redefinition
	#if(_MSC_VER>=1300)
		#define _CRT_SECURE_NO_WARNINGS 1
	#endif
	#include <new>
	#define TOADLET_LITTLE_ENDIAN 1
	#define TOADLET_C_API extern "C" __declspec(dllexport)
	#if defined(TOADLET_BUILD_DYNAMIC)
		#define TOADLET_API __declspec(dllexport)
	#elif defined(TOADLET_USE_DYNAMIC)
		#define TOADLET_API __declspec(dllimport)
	#else
		#define TOADLET_API
	#endif
	#define NOMINMAX
	#include <stdlib.h>
	#include <string.h>
	#if	defined(_WCHAR_T_DEFINED) || defined(_WCHAR_T_DEFINED_)
		namespace toadlet{
			typedef wchar_t wchar;
		}
	#else
		namespace toadlet{
			typedef unsigned short wchar;
		}
	#endif
	#define TOADLET_SIZEOF_WCHAR 16
	#define TOADLET_ALIGNOF(Type) __alignof(Type)
	#define TOADLET_ALIGN(a) __declspec(align(a))
	#if defined(TOADLET_PLATFORM_WINCE)
		#define TOADLET_ALIGNED_MALLOC(size,a) malloc(size)
		#define TOADLET_ALIGNED_FREE(pointer) free(pointer)
	#else
		#define TOADLET_ALIGNED_MALLOC(size,a) _aligned_malloc(size,a)
		#define TOADLET_ALIGNED_FREE(pointer) _aligned_free(pointer)
	#endif
	#define TOADLET_ALIGNED_SIZE 16
	#define TOADLET_THREADSAFE
	#define TOADLET_BACKABLE
#elif defined(TOADLET_PLATFORM_POSIX)
	#define TOADLET_COMPILER_GCC 1
	#include <sys/param.h>
	#include <stdlib.h>
	#include <string.h>
	#include <new>
	#if defined(TOADLET_PLATFORM_ANDROID)
		#define BYTE_ORDER _BYTE_ORDER
	#endif
	#if defined(__BYTE_ORDER)
		#if __BYTE_ORDER == __LITTLE_ENDIAN
			#define TOADLET_LITTLE_ENDIAN 1
		#elif __BYTE_ORDER == __BIG_ENDIAN
			#define TOADLET_BIG_ENDIAN 1
		#else
			#error "Unknown byte order!"
		#endif
	#elif defined(BYTE_ORDER)
		#if BYTE_ORDER == LITTLE_ENDIAN
			#define TOADLET_LITTLE_ENDIAN 1
		#elif BYTE_ORDER == BIG_ENDIAN
			#define TOADLET_BIG_ENDIAN 1
		#else
			#error "Unknown byte order!"
		#endif
	#else
		#error "Unknown byte order!"
	#endif
	#define TOADLET_C_API extern "C"
	#if defined(TOADLET_PLATFORM_OSX)
		#define TOADLET_API __attribute__ ((visibility("default")))
	#else
		#define TOADLET_API
	#endif
	namespace toadlet{
		typedef wchar_t wchar;
		template<typename T> struct alignment_trick{char c; T member;};
	}
	#define TOADLET_SIZEOF_WCHAR 32
	#define TOADLET_ALIGNOF(Type) offsetof(alignment_trick<Type>,member)
	#define TOADLET_ALIGN(a) __attribute__((aligned(a)))
	#if defined (TOADLET_PLATFORM_ANDROID)
		inline void *toadlet_malloc(int size,int a){return memalign(a,size);}
	#else
		inline void *toadlet_malloc(int size,int a){void *r=NULL;posix_memalign(&r,a,size);return r;}
	#endif
	#define TOADLET_ALIGNED_MALLOC(size,a) toadlet_malloc(size,a);
	#define TOADLET_ALIGNED_FREE(pointer) free(pointer)
	#if defined(__ARM_NEON__)
		#define TOADLET_HAS_NEON 1
	#endif
	#define TOADLET_THREADSAFE
	#define TOADLET_BACKABLE
#elif defined(TOADLET_PLATFORM_EMSCRIPTEN)
	#include <stdlib.h>
	#include <endian.h>
	#include <string.h>
	#if defined(__BYTE_ORDER)
		#if __BYTE_ORDER == __LITTLE_ENDIAN
			#define TOADLET_LITTLE_ENDIAN 1
		#elif __BYTE_ORDER == __BIG_ENDIAN
			#define TOADLET_BIG_ENDIAN 1
		#else
			#error "Unknown byte order!"
		#endif
	#elif defined(BYTE_ORDER)
		#if BYTE_ORDER == LITTLE_ENDIAN
			#define TOADLET_LITTLE_ENDIAN 1
		#elif BYTE_ORDER == BIG_ENDIAN
			#define TOADLET_BIG_ENDIAN 1
		#else
			#error "Unknown byte order!"
		#endif
	#else
		#error "Unknown byte order!"
	#endif
	#define TOADLET_C_API extern "C"
	#define TOADLET_API
	namespace toadlet{
		typedef wchar_t wchar;
		template<typename T> struct alignment_trick{char c; T member;};
	}
	#define TOADLET_SIZEOF_WCHAR 32
	#define TOADLET_ALIGNOF(Type) offsetof(alignment_trick<Type>,member)
	#define TOADLET_ALIGN(a) __attribute__((aligned(a)))
	#define TOADLET_BACKABLE
#else
	#error "Unknown platform"
#endif

#if !defined(NULL)
	#define NULL 0
#endif

#define TOADLET_QUOTE_(x) #x
#define TOADLET_QUOTE(x) TOADLET_QUOTE_(x)

#if defined(TOADLET_EXCEPTIONS)
	#define TOADLET_TRY try{
	#define TOADLET_CATCH_ANONYMOUS() }catch(const toadlet::egg::Exception &)
	#define TOADLET_CATCH(exception) }catch(exception)
#else
	#define TOADLET_TRY {
	#define TOADLET_CATCH_ANONYMOUS() }if(0)
	#define TOADLET_CATCH(exception) }for(exception=toadlet::egg::Exception();false;)
#endif

#if defined(TOADLET_FIXED_POINT)
	#define TOADLET_VERSION_FIXEDSTRING "-fixed" 
	#define TOADLET_VERSION_FIXEDWSTRING L"-fixed" 
#else
	#define TOADLET_VERSION_FIXEDSTRING
	#define TOADLET_VERSION_FIXEDWSTRING
#endif
#define TOADLET_VERSION_ITOA(x) #x
#define TOADLET_VERSION_MAKESTRING(major,minor,micro) \
	TOADLET_VERSION_ITOA(major) "." TOADLET_VERSION_ITOA(minor) "." TOADLET_VERSION_ITOA(micro) TOADLET_VERSION_FIXEDSTRING
#define TOADLET_VERSION_ITOW(x) L#x
#define TOADLET_VERSION_MAKEWSTRING(major,minor,micro) \
	TOADLET_VERSION_ITOW(major) L"." TOADLET_VERSION_ITOW(minor) L"." TOADLET_VERSION_ITOW(micro) TOADLET_VERSION_FIXEDWSTRING

template<typename Type> inline void TOADLET_IGNORE_UNUSED_VARIABLE_WARNING(Type t){}

template<typename Type> Type &TOADLET_REMOVE_POINTER(Type *type){return *type;}
template<typename Type> Type &TOADLET_REMOVE_POINTER(Type &type){return type;}
template<typename Type> const Type &TOADLET_REMOVE_POINTER(const Type *type){return *type;}
template<typename Type> const Type &TOADLET_REMOVE_POINTER(const Type &type){return type;}

#define TOADLET_MERGE_(x,y) x##y
#define TOADLET_MERGE(x,y) TOADLET_MERGE_(x,y)
#define TOADLET_ENDLINE TOADLET_MERGE(end,__LINE__)
#define tforeach(Type,item,collection)\
	for(Type item=TOADLET_REMOVE_POINTER(collection).begin(),TOADLET_ENDLINE=TOADLET_REMOVE_POINTER(collection).end();item!=TOADLET_ENDLINE;++item)

namespace toadlet{

typedef signed char int8;
typedef unsigned char uint8;
typedef uint8 tbyte;

typedef signed short int int16;
typedef unsigned short int uint16;

typedef signed int int32;
typedef unsigned int uint32;

#if defined(TOADLET_PLATFORM_WIN32)
	typedef signed __int64 int64;
	typedef unsigned __int64 uint64;
	#define TOADLET_MAKE_INT64(x) x
	#define TOADLET_MAKE_UINT64(x) x
#else
	typedef signed long long int int64;
	typedef unsigned long long int uint64;
	#define TOADLET_MAKE_INT64(x) x##LL
	#define TOADLET_MAKE_UINT64(x) x##ULL
#endif

#if defined(TOADLET_FIXED_POINT)
	typedef int scalar;
#else
	typedef float scalar;
#endif

#if defined(TOADLET_DEBUG)
	static char tbyteSizeTest [sizeof(tbyte)==  1 ?1:-1];
	static char int8SizeTest  [sizeof(int8)==   1 ?1:-1];
	static char uint8SizeTest [sizeof(uint8)==  1 ?1:-1];
	static char int16SizeTest [sizeof(int16)==  2 ?1:-1];
	static char uint16SizeTest[sizeof(uint16)== 2 ?1:-1];
	static char int32SizeTest [sizeof(int32)==  4 ?1:-1];
	static char uint32SizeTest[sizeof(uint32)== 4 ?1:-1];
	static char int64SizeTest [sizeof(int64)==  8 ?1:-1];
	static char uint64SizeTest[sizeof(uint64)== 8 ?1:-1];

	inline void silenceUnusedVariableWarnings(){
		TOADLET_IGNORE_UNUSED_VARIABLE_WARNING(tbyteSizeTest);
		TOADLET_IGNORE_UNUSED_VARIABLE_WARNING(int8SizeTest);
		TOADLET_IGNORE_UNUSED_VARIABLE_WARNING(uint8SizeTest);
		TOADLET_IGNORE_UNUSED_VARIABLE_WARNING(int16SizeTest);
		TOADLET_IGNORE_UNUSED_VARIABLE_WARNING(uint16SizeTest);
		TOADLET_IGNORE_UNUSED_VARIABLE_WARNING(int32SizeTest);
		TOADLET_IGNORE_UNUSED_VARIABLE_WARNING(uint32SizeTest);
		TOADLET_IGNORE_UNUSED_VARIABLE_WARNING(int64SizeTest);
		TOADLET_IGNORE_UNUSED_VARIABLE_WARNING(uint64SizeTest);
	}
#endif

}

#define TOADLET_ALIGNED_SIZE 16
#define TOADLET_ALIGNED TOADLET_ALIGN(TOADLET_ALIGNED_SIZE)
#define TOADLET_IS_ALIGNED(x) ((((uint64)&x)&0xF)==0)
#define TOADLET_ALIGNED_NEW \
	inline void *operator new(size_t size){return TOADLET_ALIGNED_MALLOC(size,TOADLET_ALIGNED_SIZE);} \
	inline void operator delete(void *pointer){TOADLET_ALIGNED_FREE(pointer);} \
	inline void *operator new[](size_t size){return TOADLET_ALIGNED_MALLOC(size,TOADLET_ALIGNED_SIZE);} \
	inline void operator delete[](void *pointer){TOADLET_ALIGNED_FREE(pointer);}

#endif

