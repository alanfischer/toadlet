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

#include "alem.h"

#ifdef _WIN32
#	include <windows.h>
#	include <stdio.h>
#	ifdef _DEBUG
#		define ALEM_DEBUG(x) {TCHAR buffer[1024];TCHAR *dest=buffer;char *src=x;while((*dest++=*src++)!=0);OutputDebugString(buffer);}
#		define ALEM_DEBUGC(x) OutputDebugString(x);
#	else
#		define ALEM_DEBUG
#		define ALEM_DEBUGC
#	endif
	typedef HINSTANCE alem_Library;
	typedef TCHAR alem_Char;
#else
#	include <dlfcn.h>
#	include <wchar.h>
#	include <stdio.h>
#	define ALEM_DEBUG printf
#	define ALEM_DEBUGC printf
	typedef void *alem_Library;
	typedef char alem_Char;
#endif

#ifdef _WIN32
	alem_Library alem_loadLibrary(const alem_Char *name){return LoadLibrary(name);}
	void alem_freeLibrary(alem_Library lib){FreeLibrary(lib);}
	void *alem_getProc(alem_Library lib,const alem_Char *name){return GetProcAddress(lib,name);}
#else
	alem_Library alem_loadLibrary(const alem_Char *name){return dlopen(name,RTLD_LAZY);}
	void alem_freeLibrary(alem_Library lib){dlclose(lib);}
	void *alem_getProc(alem_Library lib,const alem_Char *name){return dlsym(lib,name);}
#endif

int alem_toChar(alem_Char *dest,const char *src,int length){
	while((*dest++=*src++)!=0 && (length--)>0);
	if(*(dest-1)!=0){
		*(dest-1)=0;
		return -1;
	}
	return 1;
}

int alem_pathJoin(alem_Char *dest,const alem_Char *p1,const alem_Char *p2,int length){
	/// @todo  Make this truely length aware, not just sorta-kinda like it is now
	length--; // For the /
	while((*dest++=*p1++)!=0 && (length--)>0);
	*(dest-1)='/';
	while((*dest++=*p2++)!=0 && (length--)>0);
	return 1;
}

int alem_accelerated_requested=-1;
int alem_accelerated=0;
int alem_initialized=0;
alem_Library alem_alLibrary=0;
int alem_implementationIndex=-1;
alem_Char alem_softwareLibraryPath[1024]={0};

#ifdef _WIN32
#	define ALEM_NUM_LIBRARYNAMES 1
	alem_Char *alem_alLibraryNames[ALEM_NUM_LIBRARYNAMES]={TEXT("OpenAL32.dll"),};

#	define ALEM_NUM_LIBRARIESANDPREFIXES 1
	const int alem_prefixLibraryNames[ALEM_NUM_LIBRARIESANDPREFIXES*2]={
		// libraryNameIndex, useSoftwareLibraryPath
		0,0,
	};
#else
#	define ALEM_NUM_LIBRARYNAMES 1
	alem_Char *alem_alLibraryNames[ALEM_NUM_LIBRARYNAMES]={"openal.so",};

#	define ALEM_NUM_LIBRARIESANDPREFIXES 1
	const int alem_prefixLibraryNames[ALEM_NUM_LIBRARIESANDPREFIXES*2]={
		// libraryNameIndex, useSoftwareLibraryPath
		0,0,
	};
#endif

#define ALEM_SOFTWARE_LIBRARYNAME_START 1

#define ALEM_GETALPROCEDURE(name) name=(alem_proc_##name)alem_getProc(alem_alLibrary,#name)
#define ALEM_IMPLEMENTPROCEDURE(name) alem_proc_##name name

int alem_getProcedures();

int alem_setSoftwareLibraryPath(char *path){
	if(alem_toChar(alem_softwareLibraryPath,path,sizeof(alem_softwareLibraryPath)/sizeof(alem_Char))<0){
		return -1;
	}

	return 1;
}

int alem_init(int accelerated){
	int i;

	if(alem_initialized==1 && alem_accelerated_requested==accelerated){
		return 2;
	}

	alem_deinit();

	for(i=accelerated?0:ALEM_SOFTWARE_LIBRARYNAME_START;i<ALEM_NUM_LIBRARIESANDPREFIXES;++i){
		if(alem_prefixLibraryNames[i*2+1]>0 && alem_softwareLibraryPath[0]!=0){
			alem_Char fullLibraryPath[2048];
			alem_pathJoin(fullLibraryPath,alem_softwareLibraryPath,alem_alLibraryNames[alem_prefixLibraryNames[i*2+0]],sizeof(fullLibraryPath)/sizeof(alem_Char));
			ALEM_DEBUG("alem: Full library path for al library is:");
			ALEM_DEBUG(fullLibraryPath);
			ALEM_DEBUG("\n");
			alem_alLibrary=alem_loadLibrary(fullLibraryPath);
		}
		else{
			ALEM_DEBUG("alem: Name for al library is:");
			ALEM_DEBUG(alem_alLibraryNames[alem_prefixLibraryNames[i*2+0]]);
			ALEM_DEBUG("\n");
			alem_alLibrary=alem_loadLibrary(alem_alLibraryNames[alem_prefixLibraryNames[i*2+0]]);
		}

		if(alem_alLibrary!=0){
			ALEM_DEBUG("alem: Success\n");

			alem_implementationIndex=i;
			break;
		}

		if(alem_alLibrary!=0){
			alem_freeLibrary(alem_alLibrary);
			alem_alLibrary=0;
		}
	}

	if(alem_alLibrary==0){
		return -1;
	}

	alem_accelerated_requested=accelerated;
	alem_accelerated=(alem_implementationIndex<ALEM_SOFTWARE_LIBRARYNAME_START);
	alem_initialized=1;

	return alem_getProcedures();
}

int alem_initWithLibraryName(char *alLibraryName){
	alem_Char libraryName[2048];

	if(alem_alLibrary!=0){
		alem_freeLibrary(alem_alLibrary);
		alem_alLibrary=0;
	}

	alem_toChar(libraryName,alLibraryName,sizeof(libraryName)/sizeof(alem_Char));
	alem_alLibrary=alem_loadLibrary(libraryName);

	alem_accelerated_requested=-1;
	alem_accelerated=0;
	alem_initialized=1;

	return alem_getProcedures();
}

int alem_deinit(){
	if(alem_alLibrary!=0){
		alem_freeLibrary(alem_alLibrary);
		alem_alLibrary=0;
	}

	alem_accelerated=0;
	alem_initialized=0;

	return 1;
}

int alem_getInitialized(){
	return alem_initialized;
}

int alem_getAccelerated(){
	return alem_accelerated;
}

int alem_getProcedures(){
	/* ALC */

	ALEM_GETALPROCEDURE(alcCreateContext);
	ALEM_GETALPROCEDURE(alcMakeContextCurrent);
	ALEM_GETALPROCEDURE(alcProcessContext);
	ALEM_GETALPROCEDURE(alcSuspendContext);
	ALEM_GETALPROCEDURE(alcDestroyContext);
	ALEM_GETALPROCEDURE(alcGetCurrentContext);

	ALEM_GETALPROCEDURE(alcGetError);

	ALEM_GETALPROCEDURE(alcOpenDevice);
	ALEM_GETALPROCEDURE(alcCloseDevice);

	ALEM_GETALPROCEDURE(alcIsExtensionPresent);
	ALEM_GETALPROCEDURE(alcGetProcAddress);
	ALEM_GETALPROCEDURE(alcGetEnumValue);

	ALEM_GETALPROCEDURE(alcGetContextsDevice);

	ALEM_GETALPROCEDURE(alcGetString);
	ALEM_GETALPROCEDURE(alcGetIntegerv);

	/* AL */
	
	ALEM_GETALPROCEDURE(alEnable);
	ALEM_GETALPROCEDURE(alDisable);
	ALEM_GETALPROCEDURE(alIsEnabled);

	ALEM_GETALPROCEDURE(alHint);

	ALEM_GETALPROCEDURE(alGetBooleanv);
	ALEM_GETALPROCEDURE(alGetIntegerv);
	ALEM_GETALPROCEDURE(alGetIntegerv);
	ALEM_GETALPROCEDURE(alGetFloatv);
	ALEM_GETALPROCEDURE(alGetString);

	ALEM_GETALPROCEDURE(alGetBoolean);
	ALEM_GETALPROCEDURE(alGetInteger);
	ALEM_GETALPROCEDURE(alGetFloat);
	ALEM_GETALPROCEDURE(alGetDouble);

	ALEM_GETALPROCEDURE(alGetError);
	ALEM_GETALPROCEDURE(alIsExtensionPresent);
	ALEM_GETALPROCEDURE(alGetProcAddress);
	ALEM_GETALPROCEDURE(alGetEnumValue);

	ALEM_GETALPROCEDURE(alListenerf);
	ALEM_GETALPROCEDURE(alListeneri);
	ALEM_GETALPROCEDURE(alListener3f);
	ALEM_GETALPROCEDURE(alListenerfv);

	ALEM_GETALPROCEDURE(alGetListeneri);
	ALEM_GETALPROCEDURE(alGetListenerf);
	ALEM_GETALPROCEDURE(alGetListeneriv);
	ALEM_GETALPROCEDURE(alGetListenerfv);
	ALEM_GETALPROCEDURE(alGetListener3f);

	ALEM_GETALPROCEDURE(alGenSources);
	ALEM_GETALPROCEDURE(alDeleteSources);
	ALEM_GETALPROCEDURE(alIsSource);

	ALEM_GETALPROCEDURE(alSourcef);
	ALEM_GETALPROCEDURE(alSourcei);
	ALEM_GETALPROCEDURE(alSource3f);
	ALEM_GETALPROCEDURE(alSourcefv);

	ALEM_GETALPROCEDURE(alGetSourcei);
	ALEM_GETALPROCEDURE(alGetSourcef);
	ALEM_GETALPROCEDURE(alGetSourceiv);
	ALEM_GETALPROCEDURE(alGetSourcefv);
	ALEM_GETALPROCEDURE(alGetSource3f);

	ALEM_GETALPROCEDURE(alSourcePlayv);
	ALEM_GETALPROCEDURE(alSourceStopv);
	ALEM_GETALPROCEDURE(alSourceRewindv);
	ALEM_GETALPROCEDURE(alSourcePausev);

	ALEM_GETALPROCEDURE(alSourcePlay);
	ALEM_GETALPROCEDURE(alSourcePause);
	ALEM_GETALPROCEDURE(alSourceRewind);
	ALEM_GETALPROCEDURE(alSourceStop);

	ALEM_GETALPROCEDURE(alGenBuffers);
	ALEM_GETALPROCEDURE(alDeleteBuffers);
	ALEM_GETALPROCEDURE(alIsBuffer);
	ALEM_GETALPROCEDURE(alBufferData);

	ALEM_GETALPROCEDURE(alGetBufferi);
	ALEM_GETALPROCEDURE(alGetBufferf);
	ALEM_GETALPROCEDURE(alGetBufferiv);
	ALEM_GETALPROCEDURE(alGetBufferfv);

	ALEM_GETALPROCEDURE(alSourceQueueBuffers);
	ALEM_GETALPROCEDURE(alSourceUnqueueBuffers);
	ALEM_GETALPROCEDURE(alQueuei);

	ALEM_GETALPROCEDURE(alDopplerFactor);
	ALEM_GETALPROCEDURE(alDopplerVelocity);
	ALEM_GETALPROCEDURE(alDistanceModel);

	return 1;
}

/* ALC */

ALEM_IMPLEMENTPROCEDURE(alcCreateContext);
ALEM_IMPLEMENTPROCEDURE(alcMakeContextCurrent);
ALEM_IMPLEMENTPROCEDURE(alcProcessContext);
ALEM_IMPLEMENTPROCEDURE(alcSuspendContext);
ALEM_IMPLEMENTPROCEDURE(alcDestroyContext);
ALEM_IMPLEMENTPROCEDURE(alcGetCurrentContext);

ALEM_IMPLEMENTPROCEDURE(alcGetError);

ALEM_IMPLEMENTPROCEDURE(alcOpenDevice);
ALEM_IMPLEMENTPROCEDURE(alcCloseDevice);

ALEM_IMPLEMENTPROCEDURE(alcIsExtensionPresent);
ALEM_IMPLEMENTPROCEDURE(alcGetProcAddress);
ALEM_IMPLEMENTPROCEDURE(alcGetEnumValue);

ALEM_IMPLEMENTPROCEDURE(alcGetContextsDevice);

ALEM_IMPLEMENTPROCEDURE(alcGetString);
ALEM_IMPLEMENTPROCEDURE(alcGetIntegerv);

/* AL */

ALEM_IMPLEMENTPROCEDURE(alEnable);
ALEM_IMPLEMENTPROCEDURE(alDisable);
ALEM_IMPLEMENTPROCEDURE(alIsEnabled);

ALEM_IMPLEMENTPROCEDURE(alHint);

ALEM_IMPLEMENTPROCEDURE(alGetBooleanv);
ALEM_IMPLEMENTPROCEDURE(alGetIntegerv);
ALEM_IMPLEMENTPROCEDURE(alGetIntegerv);
ALEM_IMPLEMENTPROCEDURE(alGetFloatv);
ALEM_IMPLEMENTPROCEDURE(alGetString);

ALEM_IMPLEMENTPROCEDURE(alGetBoolean);
ALEM_IMPLEMENTPROCEDURE(alGetInteger);
ALEM_IMPLEMENTPROCEDURE(alGetFloat);
ALEM_IMPLEMENTPROCEDURE(alGetDouble);

ALEM_IMPLEMENTPROCEDURE(alGetError);
ALEM_IMPLEMENTPROCEDURE(alIsExtensionPresent);
ALEM_IMPLEMENTPROCEDURE(alGetProcAddress);
ALEM_IMPLEMENTPROCEDURE(alGetEnumValue);

ALEM_IMPLEMENTPROCEDURE(alListenerf);
ALEM_IMPLEMENTPROCEDURE(alListeneri);
ALEM_IMPLEMENTPROCEDURE(alListener3f);
ALEM_IMPLEMENTPROCEDURE(alListenerfv);

ALEM_IMPLEMENTPROCEDURE(alGetListeneri);
ALEM_IMPLEMENTPROCEDURE(alGetListenerf);
ALEM_IMPLEMENTPROCEDURE(alGetListeneriv);
ALEM_IMPLEMENTPROCEDURE(alGetListenerfv);
ALEM_IMPLEMENTPROCEDURE(alGetListener3f);

ALEM_IMPLEMENTPROCEDURE(alGenSources);
ALEM_IMPLEMENTPROCEDURE(alDeleteSources);
ALEM_IMPLEMENTPROCEDURE(alIsSource);

ALEM_IMPLEMENTPROCEDURE(alSourcef);
ALEM_IMPLEMENTPROCEDURE(alSourcei);
ALEM_IMPLEMENTPROCEDURE(alSource3f);
ALEM_IMPLEMENTPROCEDURE(alSourcefv);

ALEM_IMPLEMENTPROCEDURE(alGetSourcei);
ALEM_IMPLEMENTPROCEDURE(alGetSourcef);
ALEM_IMPLEMENTPROCEDURE(alGetSourceiv);
ALEM_IMPLEMENTPROCEDURE(alGetSourcefv);
ALEM_IMPLEMENTPROCEDURE(alGetSource3f);

ALEM_IMPLEMENTPROCEDURE(alSourcePlayv);
ALEM_IMPLEMENTPROCEDURE(alSourceStopv);
ALEM_IMPLEMENTPROCEDURE(alSourceRewindv);
ALEM_IMPLEMENTPROCEDURE(alSourcePausev);

ALEM_IMPLEMENTPROCEDURE(alSourcePlay);
ALEM_IMPLEMENTPROCEDURE(alSourcePause);
ALEM_IMPLEMENTPROCEDURE(alSourceRewind);
ALEM_IMPLEMENTPROCEDURE(alSourceStop);

ALEM_IMPLEMENTPROCEDURE(alGenBuffers);
ALEM_IMPLEMENTPROCEDURE(alDeleteBuffers);
ALEM_IMPLEMENTPROCEDURE(alIsBuffer);
ALEM_IMPLEMENTPROCEDURE(alBufferData);

ALEM_IMPLEMENTPROCEDURE(alGetBufferi);
ALEM_IMPLEMENTPROCEDURE(alGetBufferf);
ALEM_IMPLEMENTPROCEDURE(alGetBufferiv);
ALEM_IMPLEMENTPROCEDURE(alGetBufferfv);

ALEM_IMPLEMENTPROCEDURE(alSourceQueueBuffers);
ALEM_IMPLEMENTPROCEDURE(alSourceUnqueueBuffers);
ALEM_IMPLEMENTPROCEDURE(alQueuei);

ALEM_IMPLEMENTPROCEDURE(alDopplerFactor);
ALEM_IMPLEMENTPROCEDURE(alDopplerVelocity);
ALEM_IMPLEMENTPROCEDURE(alDistanceModel);

