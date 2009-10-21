 /********** Copyright header - do not remove **********
 *
 * The Toadlet Engine 
 *
 * Copyright (C) 2005, Lightning Toads Productions, LLC
 *
 * Author(s): Alan Fischer
 * 
 * All source code for the Toadlet Engine, including 
 * this file, is the sole property of Lightning Toads 
 * Productions, LLC. It has been developed on our own 
 * time, with our own tools, on our own hardware. None
 * of this code may be observed, used or changed 
 * without our explicit permission. 
 *
 *  - Lightning Toads Productions, LLC
 *
 ********** Copyright header - do not remove **********/

#ifndef TOADLET_EGG_NET_BCINCLUDES_H
#define TOADLET_EGG_NET_BCINCLUDES_H

#include <toadlet/Types.h>
#include <toadlet/egg/String.h>

#ifndef WIN32_LEAN_AND_MEAN
#	define WIN32_LEAN_AND_MEAN 1
#endif
#include <windows.h>
#include <objbase.h>

#define _BTWLIB
#include "BtIfDefinitions.h"
#include "BtIfClasses.h"
#include "BtIfObexHeaders.h"

// These linker commands are from BtSdkCE and BtwLib respectivly
#if defined(TOADLET_PLATFORM_WINCE)
	#if (_WIN32_WCE >= 0x501)
		#pragma comment(lib, "btsdkce50.lib")
	#else
		#pragma comment(lib, "btsdkce30.lib")
	#endif
#else
	#if (_MSC_FULL_VER >= 140050727)
		// use this lib for VS2005 wchar definition 
		// Note: using _MSC_VER >= 1400 is not sufficient, MSFT VC++ 6.0 under AMD64 
		// configuration February 2003 Platform SDK, the compiler (incorrectly) identifies
		// itself as 1400, so this is the accurate method.
		//
		// See Programmer's Guide - must redistribute btwapi.dll to use BtWdSdkLib.lib on
		// older BTW stack targets
		#ifdef _WIN64
			#pragma comment(lib, "x64\\BtWdSdkLib.lib")
		#else
			#pragma comment(lib, "BtWdSdkLib.lib")
		#endif
	#else
		// use this for previous compilers
		#ifdef _WIN64
	        #pragma comment(lib, "amd64\\WidcommSdklib.lib")
	    #else
			#pragma comment(lib, "WidcommSdklib.lib")
		#endif
	#endif
	#pragma comment(lib, "ws2_32.lib")
	#pragma comment(lib, "Version.lib")
#endif

namespace toadlet{
namespace egg{
namespace net{

#if defined(TOADLET_PLATFORM_WINCE)
	inline BT_CHAR *toBT_CHAR(const String &string){return (BT_CHAR*)string.wc_str();}
#else
	inline BT_CHAR *toBT_CHAR(const String &string){return (BT_CHAR*)string.c_str();}
#endif

}
}
}

#endif
