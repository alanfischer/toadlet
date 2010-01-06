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

#include "glesem.h"

#ifdef _WIN32
#	include <windows.h>
#	include <stdio.h>
#	ifdef _DEBUG
#		define GLESEM_DEBUG(x) {TCHAR buffer[1024];TCHAR *dest=buffer;char *src=x;while((*dest++=*src++)!=0);OutputDebugString(buffer);}
#		define GLESEM_DEBUGC(x) OutputDebugString(x);
#	else
#		define GLESEM_DEBUG
#		define GLESEM_DEBUGC
#	endif
	typedef HINSTANCE glesem_Library;
	typedef TCHAR glesem_Char;
#else
#	include <dlfcn.h>
#	include <wchar.h>
#	include <stdio.h>
#	define GLESEM_DEBUG printf
#	define GLESEM_DEBUGC printf
	typedef void *glesem_Library;
	typedef char glesem_Char;
#endif

#ifdef _WIN32
	glesem_Library glesem_loadLibrary(const glesem_Char *name){return LoadLibrary(name);}
	void glesem_freeLibrary(glesem_Library lib){FreeLibrary(lib);}
	void *glesem_getProc(glesem_Library lib,const glesem_Char *name){return GetProcAddress(lib,name);}
#else
	glesem_Library glesem_loadLibrary(const glesem_Char *name){return dlopen(name,RTLD_LAZY);}
	void glesem_freeLibrary(glesem_Library lib){dlclose(lib);}
	void *glesem_getProc(glesem_Library lib,const glesem_Char *name){return dlsym(lib,name);}
#endif

int glesem_toChar(glesem_Char *dest,const char *src,int length){
	while((*dest++=*src++)!=0 && (length--)>0);
	if(*(dest-1)!=0){
		*(dest-1)=0;
		return -1;
	}
	return 1;
}

int glesem_pathJoin(glesem_Char *dest,const glesem_Char *p1,const glesem_Char *p2,int length){
	/// @todo  Make this truely length aware, not just sorta-kinda like it is now
	length--; // For the /
	while((*dest++=*p1++)!=0 && (length--)>0);
	*(dest-1)='/';
	while((*dest++=*p2++)!=0 && (length--)>0);
	return 1;
}

int glesem_egl_majorVersion=0;
int glesem_egl_minorVersion=0;
int glesem_egl_version_1_1=0;
int glesem_egl_version_1_2=0;

int glesem_gl_majorVersion=0;
int glesem_gl_minorVersion=0;
int glesem_gl_version_1_1=0;

int glesem_vincent=0;

int glesem_accelerated_requested=-1;
int glesem_accelerated=0;
int glesem_initialized=0;
HINSTANCE glesem_eglLibrary=0;
HINSTANCE glesem_glLibrary=0;
int glesem_implementationIndex=-1;
glesem_Char glesem_softwareLibraryPath[1024]={0};

#ifdef _WIN32
#	define GLESEM_NUM_LIBRARYNAMES 3
	const glesem_Char *glesem_eglLibraryNames[GLESEM_NUM_LIBRARYNAMES]={
		TEXT("libGLES_CL.dll"),
		TEXT("libGLES_CM.dll"),
		TEXT("libEGL.dll"),
	};
	const glesem_Char *glesem_glLibraryNames[GLESEM_NUM_LIBRARYNAMES]={
		TEXT("libGLES_CL.dll"),
		TEXT("libGLES_CM.dll"),
		TEXT("libGLES_CM_NoE.dll"),
	};

#	define GLESEM_NUM_LIBRARIESANDPREFIXES 3
	const int glesem_prefixLibraryNames[GLESEM_NUM_LIBRARIESANDPREFIXES*2]={
		// libraryNameIndex, useSoftwareLibraryPath
		0,0,
		1,0,
		2,1,
	};
#else
#	define GLESEM_NUM_LIBRARYNAMES 1
	const glesem_Char *glesem_eglLibraryNames[GLESEM_NUM_LIBRARYNAMES]={
		TEXT("opengles.so"),
	};
	const glesem_Char *glesem_glLibraryNames[GLESEM_NUM_LIBRARYNAMES]={
		TEXT("opengles.so"),
	};

#	define GLESEM_NUM_LIBRARIESANDPREFIXES 1
	const int glesem_prefixLibraryNames[GLESEM_NUM_LIBRARIESANDPREFIXES*2]={
		// libraryNameIndex, useSoftwareLibraryPath
		0,0,
	};
#endif

#define GLESEM_SOFTWARE_LIBRARYNAME_START 2

#define GLESEM_GETEGLPROCEDURE(name) name=(glesem_proc_##name)glesem_getProc(glesem_eglLibrary,TEXT(#name))
#define GLESEM_GETGLPROCEDURE(name) name=(glesem_proc_##name)glesem_getProc(glesem_glLibrary,TEXT(#name))
#define GLESEM_IMPLEMENTPROCEDURE(name) glesem_proc_##name name

int glesem_getProcedures();

int glesem_setSoftwareLibraryPath(char *path){
	glesem_Char *dest=glesem_softwareLibraryPath;
	int available=sizeof(glesem_softwareLibraryPath)/sizeof(glesem_Char);
	while((*dest++=*path++)!=0 && (available--)>0);

	if(glesem_softwareLibraryPath[(sizeof(glesem_softwareLibraryPath)/sizeof(glesem_Char))-1]!=0){
		glesem_softwareLibraryPath[0]=0;
		return -1;
	}

	return 1;
}

int glesem_init(int accelerated){
	int i;

	if(glesem_initialized==1 && glesem_accelerated_requested==accelerated){
		return 2;
	}

	glesem_deinit();

	for(i=accelerated?0:GLESEM_SOFTWARE_LIBRARYNAME_START;i<GLESEM_NUM_LIBRARIESANDPREFIXES;++i){
		if(glesem_prefixLibraryNames[i*2+1]>0 && glesem_softwareLibraryPath[0]!=0){
			glesem_Char fullLibraryPath[2048];
			glesem_pathJoin(fullLibraryPath,glesem_softwareLibraryPath,glesem_eglLibraryNames[glesem_prefixLibraryNames[i*2+0]],sizeof(fullLibraryPath)/sizeof(glesem_Char));
			GLESEM_DEBUG("glesem: Full library path for egl library is:");
			GLESEM_DEBUGC(fullLibraryPath);
			GLESEM_DEBUG("\n");
			glesem_eglLibrary=glesem_loadLibrary(fullLibraryPath);

			glesem_pathJoin(fullLibraryPath,glesem_softwareLibraryPath,glesem_glLibraryNames[glesem_prefixLibraryNames[i*2+0]],sizeof(fullLibraryPath)/sizeof(glesem_Char));
			GLESEM_DEBUG("glesem: Full library path for gl library is:");
			GLESEM_DEBUGC(fullLibraryPath);
			GLESEM_DEBUG("\n");
			glesem_glLibrary=glesem_loadLibrary(fullLibraryPath);
		}
		else{
			GLESEM_DEBUG("glesem: Name for egl library is:");
			GLESEM_DEBUGC(glesem_eglLibraryNames[glesem_prefixLibraryNames[i*2+0]]);
			GLESEM_DEBUG("\n");
			glesem_eglLibrary=glesem_loadLibrary(glesem_eglLibraryNames[glesem_prefixLibraryNames[i*2+0]]);

			GLESEM_DEBUG("glesem: Name for gl library is:");
			GLESEM_DEBUGC(glesem_glLibraryNames[glesem_prefixLibraryNames[i*2+0]]);
			GLESEM_DEBUG("\n");
			glesem_glLibrary=glesem_loadLibrary(glesem_glLibraryNames[glesem_prefixLibraryNames[i*2+0]]);
		}

		if(glesem_eglLibrary!=0 && glesem_glLibrary!=0){
			GLESEM_DEBUG("glesem: Success\n");
			glesem_implementationIndex=i;
			break;
		}

		if(glesem_eglLibrary!=0){
			glesem_freeLibrary(glesem_eglLibrary);
			glesem_eglLibrary=0;
		}

		if(glesem_glLibrary!=0){
			glesem_freeLibrary(glesem_glLibrary);
			glesem_glLibrary=0;
		}
	}

	if(glesem_eglLibrary==0 || glesem_glLibrary==0){
		return -1;
	}

	glesem_accelerated_requested=accelerated;
	glesem_accelerated=(glesem_implementationIndex<GLESEM_SOFTWARE_LIBRARYNAME_START);
	glesem_initialized=1;

	return glesem_getProcedures();
}

int glesem_initWithLibraryNames(char *eglLibraryName,char *glLibraryName){
	glesem_Char libraryName[2048];

	if(glesem_eglLibrary!=0){
		glesem_freeLibrary(glesem_eglLibrary);
		glesem_eglLibrary=0;
	}
	
	if(glesem_glLibrary!=0){
		glesem_freeLibrary(glesem_glLibrary);
		glesem_glLibrary=0;
	}

	glesem_toChar(libraryName,eglLibraryName,sizeof(libraryName)/sizeof(glesem_Char));
	glesem_eglLibrary=glesem_loadLibrary(libraryName);

	glesem_toChar(libraryName,glLibraryName,sizeof(libraryName)/sizeof(glesem_Char));
	glesem_glLibrary=glesem_loadLibrary(libraryName);

	if(glesem_eglLibrary==0 || glesem_glLibrary==0){
		if(glesem_eglLibrary!=0){
			glesem_freeLibrary(glesem_eglLibrary);
			glesem_eglLibrary=0;
		}

		if(glesem_glLibrary!=0){
			glesem_freeLibrary(glesem_glLibrary);
			glesem_glLibrary=0;
		}

		return -1;
	}

	glesem_accelerated_requested=-1;
	glesem_accelerated=0;
	glesem_initialized=1;

	return glesem_getProcedures();
}

int glesem_deinit(){
	if(glesem_eglLibrary!=0){
		glesem_freeLibrary(glesem_eglLibrary);
		glesem_eglLibrary=0;
	}
	
	if(glesem_glLibrary!=0){
		glesem_freeLibrary(glesem_glLibrary);
		glesem_glLibrary=0;
	}

	glesem_accelerated=0;
	glesem_initialized=0;

	return 1;
}

int glesem_getInitialized(){
	return glesem_initialized;
}

int glesem_getAccelerated(){
	return glesem_accelerated;
}

GLESEMAPI int GLESEMAPIENTRY glesem_eglInitialize(EGLDisplay display){
	EGLBoolean result=eglInitialize(display,&glesem_egl_majorVersion,&glesem_egl_minorVersion);
	if(result==0){
		return 0;
	}

	if(glesem_egl_majorVersion>=1){
		if(glesem_egl_minorVersion>=1){
			glesem_egl_version_1_1=1;
		}
		if(glesem_egl_minorVersion>=2){
			glesem_egl_version_1_2=1;
		}
	}

	return 1;
}

int glesem_glInitialize(){
	const char *version=glGetString(GL_VERSION);
	if(version==0){
		return 0;
	}

	while(version!=0){
		if(*(version)=='.'){
			glesem_gl_majorVersion=(*(version-1))-'0';
			glesem_gl_minorVersion=(*(version+1))-'0';
			break;
		}
		version++;
	}

	if(glesem_gl_majorVersion>=1){
		if(glesem_gl_minorVersion>=1){
			glesem_gl_version_1_1=1;
		}
	}

	return 1;
}

int glesem_getProcedures(){
	/* EGL */

	GLESEM_GETEGLPROCEDURE(eglGetError);
	GLESEM_GETEGLPROCEDURE(eglGetDisplay);
	GLESEM_GETEGLPROCEDURE(eglInitialize);
	GLESEM_GETEGLPROCEDURE(eglTerminate);

	GLESEM_GETEGLPROCEDURE(eglQueryString);

	GLESEM_GETEGLPROCEDURE(eglGetConfigs);
	GLESEM_GETEGLPROCEDURE(eglChooseConfig);
	GLESEM_GETEGLPROCEDURE(eglGetConfigAttrib);

	GLESEM_GETEGLPROCEDURE(eglCreateWindowSurface);
	GLESEM_GETEGLPROCEDURE(eglCreatePbufferSurface);
	GLESEM_GETEGLPROCEDURE(eglCreatePixmapSurface);
	GLESEM_GETEGLPROCEDURE(eglDestroySurface);
	GLESEM_GETEGLPROCEDURE(eglQuerySurface);

	#if (GLESEM_EGL_BUILD_VERSION >= 110)
		/* EGL 1.1 render-to-texture APIs */
		GLESEM_GETEGLPROCEDURE(eglSurfaceAttrib);
		GLESEM_GETEGLPROCEDURE(eglBindTexImage);
		GLESEM_GETEGLPROCEDURE(eglReleaseTexImage);

		/* EGL 1.1 swap control API */
		GLESEM_GETEGLPROCEDURE(eglSwapInterval);
	#endif /* GLESEM_EGL_BUILD_VERSION >= 110 */

	GLESEM_GETEGLPROCEDURE(eglCreateContext);
	GLESEM_GETEGLPROCEDURE(eglDestroyContext);
	GLESEM_GETEGLPROCEDURE(eglMakeCurrent);

	GLESEM_GETEGLPROCEDURE(eglGetCurrentContext);
	GLESEM_GETEGLPROCEDURE(eglGetCurrentSurface);
	GLESEM_GETEGLPROCEDURE(eglGetCurrentDisplay);
	GLESEM_GETEGLPROCEDURE(eglQueryContext);

	GLESEM_GETEGLPROCEDURE(eglWaitGL);
	GLESEM_GETEGLPROCEDURE(eglWaitNative);
	GLESEM_GETEGLPROCEDURE(eglSwapBuffers);

	GLESEM_GETEGLPROCEDURE(eglCopyBuffers);

	GLESEM_GETEGLPROCEDURE(eglGetProcAddress);

	#if (GLESEM_EGL_BUILD_VERSION >= 120)
		GLESEM_GETEGLPROCEDURE(eglCreatePbufferFromClientBuffer);
		GLESEM_GETEGLPROCEDURE(eglWaitClient);
		GLESEM_GETEGLPROCEDURE(eglBindAPI);
		GLESEM_GETEGLPROCEDURE(eglQueryAPI);
		GLESEM_GETEGLPROCEDURE(eglReleaseThread);
	#endif

	/* GL */

	GLESEM_GETGLPROCEDURE(glActiveTexture);
	GLESEM_GETGLPROCEDURE(glAlphaFunc);
	GLESEM_GETGLPROCEDURE(glAlphaFuncx);
	GLESEM_GETGLPROCEDURE(glBindTexture);
	GLESEM_GETGLPROCEDURE(glBlendFunc);
	GLESEM_GETGLPROCEDURE(glClear);
	GLESEM_GETGLPROCEDURE(glClearColor);
	GLESEM_GETGLPROCEDURE(glClearColorx);
	GLESEM_GETGLPROCEDURE(glClearDepthf);
	GLESEM_GETGLPROCEDURE(glClearDepthx);
	GLESEM_GETGLPROCEDURE(glClearStencil);
	GLESEM_GETGLPROCEDURE(glClientActiveTexture);
	GLESEM_GETGLPROCEDURE(glColor4f);
	GLESEM_GETGLPROCEDURE(glColor4x);
	GLESEM_GETGLPROCEDURE(glColorMask);
	GLESEM_GETGLPROCEDURE(glColorPointer);
	GLESEM_GETGLPROCEDURE(glCompressedTexImage2D);
	GLESEM_GETGLPROCEDURE(glCompressedTexSubImage2D);
	GLESEM_GETGLPROCEDURE(glCopyTexImage2D);
	GLESEM_GETGLPROCEDURE(glCopyTexSubImage2D);
	GLESEM_GETGLPROCEDURE(glCullFace);
	GLESEM_GETGLPROCEDURE(glDeleteTextures);
	GLESEM_GETGLPROCEDURE(glDepthFunc);
	GLESEM_GETGLPROCEDURE(glDepthMask);
	GLESEM_GETGLPROCEDURE(glDepthRangef);
	GLESEM_GETGLPROCEDURE(glDepthRangex);
	GLESEM_GETGLPROCEDURE(glDisable);
	GLESEM_GETGLPROCEDURE(glDisableClientState);
	GLESEM_GETGLPROCEDURE(glDrawArrays);
	GLESEM_GETGLPROCEDURE(glDrawElements);
	GLESEM_GETGLPROCEDURE(glEnable);
	GLESEM_GETGLPROCEDURE(glEnableClientState);
	GLESEM_GETGLPROCEDURE(glFinish);
	GLESEM_GETGLPROCEDURE(glFlush);
	GLESEM_GETGLPROCEDURE(glFogf);
	GLESEM_GETGLPROCEDURE(glFogfv);
	GLESEM_GETGLPROCEDURE(glFogx);
	GLESEM_GETGLPROCEDURE(glFogxv);
	GLESEM_GETGLPROCEDURE(glFrontFace);
	GLESEM_GETGLPROCEDURE(glFrustumf);
	GLESEM_GETGLPROCEDURE(glFrustumx);
	GLESEM_GETGLPROCEDURE(glGenTextures);
	GLESEM_GETGLPROCEDURE(glGetError);
	GLESEM_GETGLPROCEDURE(glGetIntegerv);
	GLESEM_GETGLPROCEDURE(glGetString);
	GLESEM_GETGLPROCEDURE(glHint);
	GLESEM_GETGLPROCEDURE(glLightf);
	GLESEM_GETGLPROCEDURE(glLightfv);
	GLESEM_GETGLPROCEDURE(glLightx);
	GLESEM_GETGLPROCEDURE(glLightxv);
	GLESEM_GETGLPROCEDURE(glLightModelf);
	GLESEM_GETGLPROCEDURE(glLightModelfv);
	GLESEM_GETGLPROCEDURE(glLightModelx);
	GLESEM_GETGLPROCEDURE(glLightModelxv);
	GLESEM_GETGLPROCEDURE(glLineWidth);
	GLESEM_GETGLPROCEDURE(glLineWidthx);
	GLESEM_GETGLPROCEDURE(glLoadIdentity);
	GLESEM_GETGLPROCEDURE(glLoadMatrixf);
	GLESEM_GETGLPROCEDURE(glLoadMatrixx);
	GLESEM_GETGLPROCEDURE(glLogicOp);
	GLESEM_GETGLPROCEDURE(glMaterialf);
	GLESEM_GETGLPROCEDURE(glMaterialfv);
	GLESEM_GETGLPROCEDURE(glMaterialx);
	GLESEM_GETGLPROCEDURE(glMaterialxv);
	GLESEM_GETGLPROCEDURE(glMatrixMode);
	GLESEM_GETGLPROCEDURE(glMultiTexCoord4f);
	GLESEM_GETGLPROCEDURE(glMultiTexCoord4x);
	GLESEM_GETGLPROCEDURE(glMultMatrixf);
	GLESEM_GETGLPROCEDURE(glMultMatrixx);
	GLESEM_GETGLPROCEDURE(glNormal3f);
	GLESEM_GETGLPROCEDURE(glNormal3x);
	GLESEM_GETGLPROCEDURE(glNormalPointer);
	GLESEM_GETGLPROCEDURE(glOrthof);
	GLESEM_GETGLPROCEDURE(glOrthox);
	GLESEM_GETGLPROCEDURE(glPixelStorei);
	GLESEM_GETGLPROCEDURE(glPointSize);
	GLESEM_GETGLPROCEDURE(glPointSizex);
	GLESEM_GETGLPROCEDURE(glPolygonOffset);
	GLESEM_GETGLPROCEDURE(glPolygonOffsetx);
	GLESEM_GETGLPROCEDURE(glPopMatrix);
	GLESEM_GETGLPROCEDURE(glPushMatrix);
	GLESEM_GETGLPROCEDURE(glReadPixels);
	GLESEM_GETGLPROCEDURE(glRotatef);
	GLESEM_GETGLPROCEDURE(glRotatex);
	GLESEM_GETGLPROCEDURE(glSampleCoverage);
	GLESEM_GETGLPROCEDURE(glSampleCoveragex);
	GLESEM_GETGLPROCEDURE(glScalef);
	GLESEM_GETGLPROCEDURE(glScalex);
	GLESEM_GETGLPROCEDURE(glScissor);
	GLESEM_GETGLPROCEDURE(glShadeModel);
	GLESEM_GETGLPROCEDURE(glStencilFunc);
	GLESEM_GETGLPROCEDURE(glStencilMask);
	GLESEM_GETGLPROCEDURE(glStencilOp);
	GLESEM_GETGLPROCEDURE(glTexCoordPointer);
	GLESEM_GETGLPROCEDURE(glTexEnvf);
	GLESEM_GETGLPROCEDURE(glTexEnvfv);
	GLESEM_GETGLPROCEDURE(glTexEnvx);
	GLESEM_GETGLPROCEDURE(glTexEnvxv);
	GLESEM_GETGLPROCEDURE(glTexImage2D);
	GLESEM_GETGLPROCEDURE(glTexParameterf);
	GLESEM_GETGLPROCEDURE(glTexParameterx);
	GLESEM_GETGLPROCEDURE(glTexSubImage2D);
	GLESEM_GETGLPROCEDURE(glTranslatef);
	GLESEM_GETGLPROCEDURE(glTranslatex);
	GLESEM_GETGLPROCEDURE(glVertexPointer);
	GLESEM_GETGLPROCEDURE(glViewport);

	#if (GLESEM_GL_BUILD_VERSION >= 110)
		GLESEM_GETGLPROCEDURE(glBindBuffer);
		GLESEM_GETGLPROCEDURE(glBufferData);
		GLESEM_GETGLPROCEDURE(glBufferSubData);
		GLESEM_GETGLPROCEDURE(glColor4ub);
		GLESEM_GETGLPROCEDURE(glCurrentPaletteMatrixOES);
		GLESEM_GETGLPROCEDURE(glDeleteBuffers);
		GLESEM_GETGLPROCEDURE(glGenBuffers);
		GLESEM_GETGLPROCEDURE(glGetBooleanv);
		GLESEM_GETGLPROCEDURE(glGetBufferParameteriv);
		GLESEM_GETGLPROCEDURE(glGetClipPlanef);
		GLESEM_GETGLPROCEDURE(glGetClipPlanex);
		GLESEM_GETGLPROCEDURE(glGetFloatv);
		GLESEM_GETGLPROCEDURE(glGetFixedv);
		GLESEM_GETGLPROCEDURE(glGetLightfv);
		GLESEM_GETGLPROCEDURE(glGetLightxv);
		GLESEM_GETGLPROCEDURE(glGetMaterialfv);
		GLESEM_GETGLPROCEDURE(glGetMaterialxv);
		GLESEM_GETGLPROCEDURE(glGetPointerv);
		GLESEM_GETGLPROCEDURE(glGetTexEnvfv);
		GLESEM_GETGLPROCEDURE(glGetTexEnviv);
		GLESEM_GETGLPROCEDURE(glGetTexEnvxv);
		GLESEM_GETGLPROCEDURE(glGetTexParameteriv);
		GLESEM_GETGLPROCEDURE(glGetTexParameterfv);
		GLESEM_GETGLPROCEDURE(glGetTexParameterxv);
		GLESEM_GETGLPROCEDURE(glIsEnabled);
		GLESEM_GETGLPROCEDURE(glIsTexture);
		GLESEM_GETGLPROCEDURE(glIsBuffer);
		GLESEM_GETGLPROCEDURE(glLoadPaletteFromModelViewMatrixOES);
		GLESEM_GETGLPROCEDURE(glMatrixIndexPointerOES);
		GLESEM_GETGLPROCEDURE(glWeightPointerOES);
		GLESEM_GETGLPROCEDURE(glClipPlanef);
		GLESEM_GETGLPROCEDURE(glClipPlanex);
		GLESEM_GETGLPROCEDURE(glPointSizePointerOES);
		GLESEM_GETGLPROCEDURE(glPointParameterfv);
		GLESEM_GETGLPROCEDURE(glPointParameterxv);
		GLESEM_GETGLPROCEDURE(glPointParameterf);
		GLESEM_GETGLPROCEDURE(glPointParameterx);
		GLESEM_GETGLPROCEDURE(glDrawTexfOES);
		GLESEM_GETGLPROCEDURE(glDrawTexxOES);
		GLESEM_GETGLPROCEDURE(glDrawTexiOES);
		GLESEM_GETGLPROCEDURE(glDrawTexsOES);
		GLESEM_GETGLPROCEDURE(glDrawTexfvOES);
		GLESEM_GETGLPROCEDURE(glDrawTexxvOES);
		GLESEM_GETGLPROCEDURE(glDrawTexivOES);
		GLESEM_GETGLPROCEDURE(glDrawTexsvOES);
		GLESEM_GETGLPROCEDURE(glTexEnvi);
		GLESEM_GETGLPROCEDURE(glTexEnviv);
		GLESEM_GETGLPROCEDURE(glTexParameteri);
		GLESEM_GETGLPROCEDURE(glTexParameterfv);
		GLESEM_GETGLPROCEDURE(glTexParameterxv);
		GLESEM_GETGLPROCEDURE(glTexParameteriv);
	#endif /* GLESEM_GL_BUILD_VERSION >= 110 */

	GLESEM_GETGLPROCEDURE(glQueryMatrixxOES);

	return 1;
}

/* EGL */

GLESEM_IMPLEMENTPROCEDURE(eglGetError);
GLESEM_IMPLEMENTPROCEDURE(eglGetDisplay);
GLESEM_IMPLEMENTPROCEDURE(eglInitialize);
GLESEM_IMPLEMENTPROCEDURE(eglTerminate);

GLESEM_IMPLEMENTPROCEDURE(eglQueryString);

GLESEM_IMPLEMENTPROCEDURE(eglGetConfigs);
GLESEM_IMPLEMENTPROCEDURE(eglChooseConfig);
GLESEM_IMPLEMENTPROCEDURE(eglGetConfigAttrib);

GLESEM_IMPLEMENTPROCEDURE(eglCreateWindowSurface);
GLESEM_IMPLEMENTPROCEDURE(eglCreatePbufferSurface);
GLESEM_IMPLEMENTPROCEDURE(eglCreatePixmapSurface);
GLESEM_IMPLEMENTPROCEDURE(eglDestroySurface);
GLESEM_IMPLEMENTPROCEDURE(eglQuerySurface);

#if (GLESEM_EGL_BUILD_VERSION >= 110)
	/* EGL 1.1 render-to-texture APIs */
	GLESEM_IMPLEMENTPROCEDURE(eglSurfaceAttrib);
	GLESEM_IMPLEMENTPROCEDURE(eglBindTexImage);
	GLESEM_IMPLEMENTPROCEDURE(eglReleaseTexImage);

	/* EGL 1.1 swap control API */
	GLESEM_IMPLEMENTPROCEDURE(eglSwapInterval);
#endif /* GLESEM_EGL_BUILD_VERSION >= 110 */

GLESEM_IMPLEMENTPROCEDURE(eglCreateContext);
GLESEM_IMPLEMENTPROCEDURE(eglDestroyContext);
GLESEM_IMPLEMENTPROCEDURE(eglMakeCurrent);

GLESEM_IMPLEMENTPROCEDURE(eglGetCurrentContext);
GLESEM_IMPLEMENTPROCEDURE(eglGetCurrentSurface);
GLESEM_IMPLEMENTPROCEDURE(eglGetCurrentDisplay);
GLESEM_IMPLEMENTPROCEDURE(eglQueryContext);

GLESEM_IMPLEMENTPROCEDURE(eglWaitGL);
GLESEM_IMPLEMENTPROCEDURE(eglWaitNative);
GLESEM_IMPLEMENTPROCEDURE(eglSwapBuffers);

GLESEM_IMPLEMENTPROCEDURE(eglCopyBuffers);

GLESEM_IMPLEMENTPROCEDURE(eglGetProcAddress);

#if (GLESEM_EGL_BUILD_VERSION >= 120)
	GLESEM_IMPLEMENTPROCEDURE(eglCreatePbufferFromClientBuffer);
	GLESEM_IMPLEMENTPROCEDURE(eglWaitClient);
	GLESEM_IMPLEMENTPROCEDURE(eglBindAPI);
	GLESEM_IMPLEMENTPROCEDURE(eglQueryAPI);
	GLESEM_IMPLEMENTPROCEDURE(eglReleaseThread);
#endif

/* GL */

GLESEM_IMPLEMENTPROCEDURE(glActiveTexture);
GLESEM_IMPLEMENTPROCEDURE(glAlphaFunc);
GLESEM_IMPLEMENTPROCEDURE(glAlphaFuncx);
GLESEM_IMPLEMENTPROCEDURE(glBindTexture);
GLESEM_IMPLEMENTPROCEDURE(glBlendFunc);
GLESEM_IMPLEMENTPROCEDURE(glClear);
GLESEM_IMPLEMENTPROCEDURE(glClearColor);
GLESEM_IMPLEMENTPROCEDURE(glClearColorx);
GLESEM_IMPLEMENTPROCEDURE(glClearDepthf);
GLESEM_IMPLEMENTPROCEDURE(glClearDepthx);
GLESEM_IMPLEMENTPROCEDURE(glClearStencil);
GLESEM_IMPLEMENTPROCEDURE(glClientActiveTexture);
GLESEM_IMPLEMENTPROCEDURE(glColor4f);
GLESEM_IMPLEMENTPROCEDURE(glColor4x);
GLESEM_IMPLEMENTPROCEDURE(glColorMask);
GLESEM_IMPLEMENTPROCEDURE(glColorPointer);
GLESEM_IMPLEMENTPROCEDURE(glCompressedTexImage2D);
GLESEM_IMPLEMENTPROCEDURE(glCompressedTexSubImage2D);
GLESEM_IMPLEMENTPROCEDURE(glCopyTexImage2D);
GLESEM_IMPLEMENTPROCEDURE(glCopyTexSubImage2D);
GLESEM_IMPLEMENTPROCEDURE(glCullFace);
GLESEM_IMPLEMENTPROCEDURE(glDeleteTextures);
GLESEM_IMPLEMENTPROCEDURE(glDepthFunc);
GLESEM_IMPLEMENTPROCEDURE(glDepthMask);
GLESEM_IMPLEMENTPROCEDURE(glDepthRangef);
GLESEM_IMPLEMENTPROCEDURE(glDepthRangex);
GLESEM_IMPLEMENTPROCEDURE(glDisable);
GLESEM_IMPLEMENTPROCEDURE(glDisableClientState);
GLESEM_IMPLEMENTPROCEDURE(glDrawArrays);
GLESEM_IMPLEMENTPROCEDURE(glDrawElements);
GLESEM_IMPLEMENTPROCEDURE(glEnable);
GLESEM_IMPLEMENTPROCEDURE(glEnableClientState);
GLESEM_IMPLEMENTPROCEDURE(glFinish);
GLESEM_IMPLEMENTPROCEDURE(glFlush);
GLESEM_IMPLEMENTPROCEDURE(glFogf);
GLESEM_IMPLEMENTPROCEDURE(glFogfv);
GLESEM_IMPLEMENTPROCEDURE(glFogx);
GLESEM_IMPLEMENTPROCEDURE(glFogxv);
GLESEM_IMPLEMENTPROCEDURE(glFrontFace);
GLESEM_IMPLEMENTPROCEDURE(glFrustumf);
GLESEM_IMPLEMENTPROCEDURE(glFrustumx);
GLESEM_IMPLEMENTPROCEDURE(glGenTextures);
GLESEM_IMPLEMENTPROCEDURE(glGetError);
GLESEM_IMPLEMENTPROCEDURE(glGetIntegerv);
GLESEM_IMPLEMENTPROCEDURE(glGetString);
GLESEM_IMPLEMENTPROCEDURE(glHint);
GLESEM_IMPLEMENTPROCEDURE(glLightf);
GLESEM_IMPLEMENTPROCEDURE(glLightfv);
GLESEM_IMPLEMENTPROCEDURE(glLightx);
GLESEM_IMPLEMENTPROCEDURE(glLightxv);
GLESEM_IMPLEMENTPROCEDURE(glLightModelf);
GLESEM_IMPLEMENTPROCEDURE(glLightModelfv);
GLESEM_IMPLEMENTPROCEDURE(glLightModelx);
GLESEM_IMPLEMENTPROCEDURE(glLightModelxv);
GLESEM_IMPLEMENTPROCEDURE(glLineWidth);
GLESEM_IMPLEMENTPROCEDURE(glLineWidthx);
GLESEM_IMPLEMENTPROCEDURE(glLoadIdentity);
GLESEM_IMPLEMENTPROCEDURE(glLoadMatrixf);
GLESEM_IMPLEMENTPROCEDURE(glLoadMatrixx);
GLESEM_IMPLEMENTPROCEDURE(glLogicOp);
GLESEM_IMPLEMENTPROCEDURE(glMaterialf);
GLESEM_IMPLEMENTPROCEDURE(glMaterialfv);
GLESEM_IMPLEMENTPROCEDURE(glMaterialx);
GLESEM_IMPLEMENTPROCEDURE(glMaterialxv);
GLESEM_IMPLEMENTPROCEDURE(glMatrixMode);
GLESEM_IMPLEMENTPROCEDURE(glMultiTexCoord4f);
GLESEM_IMPLEMENTPROCEDURE(glMultiTexCoord4x);
GLESEM_IMPLEMENTPROCEDURE(glMultMatrixf);
GLESEM_IMPLEMENTPROCEDURE(glMultMatrixx);
GLESEM_IMPLEMENTPROCEDURE(glNormal3f);
GLESEM_IMPLEMENTPROCEDURE(glNormal3x);
GLESEM_IMPLEMENTPROCEDURE(glNormalPointer);
GLESEM_IMPLEMENTPROCEDURE(glOrthof);
GLESEM_IMPLEMENTPROCEDURE(glOrthox);
GLESEM_IMPLEMENTPROCEDURE(glPixelStorei);
GLESEM_IMPLEMENTPROCEDURE(glPointSize);
GLESEM_IMPLEMENTPROCEDURE(glPointSizex);
GLESEM_IMPLEMENTPROCEDURE(glPolygonOffset);
GLESEM_IMPLEMENTPROCEDURE(glPolygonOffsetx);
GLESEM_IMPLEMENTPROCEDURE(glPopMatrix);
GLESEM_IMPLEMENTPROCEDURE(glPushMatrix);
GLESEM_IMPLEMENTPROCEDURE(glReadPixels);
GLESEM_IMPLEMENTPROCEDURE(glRotatef);
GLESEM_IMPLEMENTPROCEDURE(glRotatex);
GLESEM_IMPLEMENTPROCEDURE(glSampleCoverage);
GLESEM_IMPLEMENTPROCEDURE(glSampleCoveragex);
GLESEM_IMPLEMENTPROCEDURE(glScalef);
GLESEM_IMPLEMENTPROCEDURE(glScalex);
GLESEM_IMPLEMENTPROCEDURE(glScissor);
GLESEM_IMPLEMENTPROCEDURE(glShadeModel);
GLESEM_IMPLEMENTPROCEDURE(glStencilFunc);
GLESEM_IMPLEMENTPROCEDURE(glStencilMask);
GLESEM_IMPLEMENTPROCEDURE(glStencilOp);
GLESEM_IMPLEMENTPROCEDURE(glTexCoordPointer);
GLESEM_IMPLEMENTPROCEDURE(glTexEnvf);
GLESEM_IMPLEMENTPROCEDURE(glTexEnvfv);
GLESEM_IMPLEMENTPROCEDURE(glTexEnvx);
GLESEM_IMPLEMENTPROCEDURE(glTexEnvxv);
GLESEM_IMPLEMENTPROCEDURE(glTexImage2D);
GLESEM_IMPLEMENTPROCEDURE(glTexParameterf);
GLESEM_IMPLEMENTPROCEDURE(glTexParameterx);
GLESEM_IMPLEMENTPROCEDURE(glTexSubImage2D);
GLESEM_IMPLEMENTPROCEDURE(glTranslatef);
GLESEM_IMPLEMENTPROCEDURE(glTranslatex);
GLESEM_IMPLEMENTPROCEDURE(glVertexPointer);
GLESEM_IMPLEMENTPROCEDURE(glViewport);

#if (GLESEM_GL_BUILD_VERSION >= 110)
	GLESEM_IMPLEMENTPROCEDURE(glBindBuffer);
	GLESEM_IMPLEMENTPROCEDURE(glBufferData);
	GLESEM_IMPLEMENTPROCEDURE(glBufferSubData);
	GLESEM_IMPLEMENTPROCEDURE(glColor4ub);
	GLESEM_IMPLEMENTPROCEDURE(glCurrentPaletteMatrixOES);
	GLESEM_IMPLEMENTPROCEDURE(glDeleteBuffers);
	GLESEM_IMPLEMENTPROCEDURE(glGenBuffers);
	GLESEM_IMPLEMENTPROCEDURE(glGetBooleanv);
	GLESEM_IMPLEMENTPROCEDURE(glGetBufferParameteriv);
	GLESEM_IMPLEMENTPROCEDURE(glGetClipPlanef);
	GLESEM_IMPLEMENTPROCEDURE(glGetClipPlanex);
	GLESEM_IMPLEMENTPROCEDURE(glGetFloatv);
	GLESEM_IMPLEMENTPROCEDURE(glGetFixedv);
	GLESEM_IMPLEMENTPROCEDURE(glGetLightfv);
	GLESEM_IMPLEMENTPROCEDURE(glGetLightxv);
	GLESEM_IMPLEMENTPROCEDURE(glGetMaterialfv);
	GLESEM_IMPLEMENTPROCEDURE(glGetMaterialxv);
	GLESEM_IMPLEMENTPROCEDURE(glGetPointerv);
	GLESEM_IMPLEMENTPROCEDURE(glGetTexEnvfv);
	GLESEM_IMPLEMENTPROCEDURE(glGetTexEnviv);
	GLESEM_IMPLEMENTPROCEDURE(glGetTexEnvxv);
	GLESEM_IMPLEMENTPROCEDURE(glGetTexParameteriv);
	GLESEM_IMPLEMENTPROCEDURE(glGetTexParameterfv);
	GLESEM_IMPLEMENTPROCEDURE(glGetTexParameterxv);
	GLESEM_IMPLEMENTPROCEDURE(glIsEnabled);
	GLESEM_IMPLEMENTPROCEDURE(glIsTexture);
	GLESEM_IMPLEMENTPROCEDURE(glIsBuffer);
	GLESEM_IMPLEMENTPROCEDURE(glLoadPaletteFromModelViewMatrixOES);
	GLESEM_IMPLEMENTPROCEDURE(glMatrixIndexPointerOES);
	GLESEM_IMPLEMENTPROCEDURE(glWeightPointerOES);
	GLESEM_IMPLEMENTPROCEDURE(glClipPlanef);
	GLESEM_IMPLEMENTPROCEDURE(glClipPlanex);
	GLESEM_IMPLEMENTPROCEDURE(glPointSizePointerOES);
	GLESEM_IMPLEMENTPROCEDURE(glPointParameterfv);
	GLESEM_IMPLEMENTPROCEDURE(glPointParameterxv);
	GLESEM_IMPLEMENTPROCEDURE(glPointParameterf);
	GLESEM_IMPLEMENTPROCEDURE(glPointParameterx);
	GLESEM_IMPLEMENTPROCEDURE(glDrawTexfOES);
	GLESEM_IMPLEMENTPROCEDURE(glDrawTexxOES);
	GLESEM_IMPLEMENTPROCEDURE(glDrawTexiOES);
	GLESEM_IMPLEMENTPROCEDURE(glDrawTexsOES);
	GLESEM_IMPLEMENTPROCEDURE(glDrawTexfvOES);
	GLESEM_IMPLEMENTPROCEDURE(glDrawTexxvOES);
	GLESEM_IMPLEMENTPROCEDURE(glDrawTexivOES);
	GLESEM_IMPLEMENTPROCEDURE(glDrawTexsvOES);
	GLESEM_IMPLEMENTPROCEDURE(glTexEnvi);
	GLESEM_IMPLEMENTPROCEDURE(glTexEnviv);
	GLESEM_IMPLEMENTPROCEDURE(glTexParameteri);
	GLESEM_IMPLEMENTPROCEDURE(glTexParameterfv);
	GLESEM_IMPLEMENTPROCEDURE(glTexParameterxv);
	GLESEM_IMPLEMENTPROCEDURE(glTexParameteriv);
#endif /* GLESEM_GL_BUILD_VERSION >= 110 */

GLESEM_IMPLEMENTPROCEDURE(glQueryMatrixxOES);
