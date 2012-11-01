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

#include "Win32ErrorHandler.h"
#include "Win32ErrorHandler_currentctx.h"
#include <toadlet/egg/String.h>
#include "tchar.h"

#include <stdio.h>

#define TOADLET_GETPROCEDURE(library,name) name=(proc_##name)GetProcAddress(library,TEXT(#name))

namespace toadlet{
namespace egg{

Win32ErrorHandler *Win32ErrorHandler::instance=NULL;

Win32ErrorHandler::Win32ErrorHandler():
	mInstalled(false),
	mListener(NULL)
{
	instance=this;

	TCHAR name[MAX_PATH];
	// First check if the ".local" file exists
	if(GetModuleFileName(NULL,name,MAX_PATH)>0){
		_tcscat(name,TEXT(".local"));
		if(GetFileAttributes(name)==INVALID_FILE_ATTRIBUTES){
			// ".local" file does not exist, so we can try to load the dbghelp.dll from the "Debugging Tools for Windows"
			TCHAR programFiles[MAX_PATH];
			if(GetEnvironmentVariable(TEXT("ProgramFiles"),programFiles,MAX_PATH)>0){
				if(mDbgHelp==NULL){
					_tcscpy(name,programFiles);
					_tcscat(name,TEXT("\\Debugging Tools for Windows\\dbghelp.dll"));
					if(GetFileAttributes(name)!=INVALID_FILE_ATTRIBUTES){
						mDbgHelp=LoadLibrary(name);
					}
				}
				if(mDbgHelp==NULL){
					_tcscpy(name,programFiles);
					_tcscat(name,TEXT("\\Debugging Tools for Windows 64-Bit\\dbghelp.dll"));
					if(GetFileAttributes(name)!=INVALID_FILE_ATTRIBUTES){
						mDbgHelp=LoadLibrary(name);
					}
				}
			}
		}
	}
	if(mDbgHelp==NULL){
		mDbgHelp=LoadLibrary(TEXT("dbghelp.dll"));
	}

	if(mDbgHelp!=NULL){
		TOADLET_GETPROCEDURE(mDbgHelp,StackWalk64);
		TOADLET_GETPROCEDURE(mDbgHelp,SymInitialize);
		TOADLET_GETPROCEDURE(mDbgHelp,SymGetOptions);
		TOADLET_GETPROCEDURE(mDbgHelp,SymSetOptions);
		TOADLET_GETPROCEDURE(mDbgHelp,SymCleanup);
		TOADLET_GETPROCEDURE(mDbgHelp,SymFunctionTableAccess64);
		TOADLET_GETPROCEDURE(mDbgHelp,SymGetModuleBase64);
		TOADLET_GETPROCEDURE(mDbgHelp,SymGetSymFromAddr64);
		TOADLET_GETPROCEDURE(mDbgHelp,UnDecorateSymbolName);
		TOADLET_GETPROCEDURE(mDbgHelp,SymGetLineFromAddr64);
	}
}

Win32ErrorHandler::~Win32ErrorHandler(){ 
	uninstallHandler();

	if(mDbgHelp!=NULL){
		FreeLibrary(mDbgHelp);
		mDbgHelp=NULL;
	}

	instance=NULL;
}

bool Win32ErrorHandler::installHandler(){
	if(mInstalled) return true;

	if(mDbgHelp!=NULL){
		mOldFilter=SetUnhandledExceptionFilter(exceptionFilter);
		mInstalled=true;
	}
	else{
		mInstalled=false;
	}

	return mInstalled;
}

bool Win32ErrorHandler::uninstallHandler(){
	if(mInstalled==false) return true;

	SetUnhandledExceptionFilter(mOldFilter);
	mInstalled=false;

	return mInstalled;
}

LONG Win32ErrorHandler::exceptionFilter(LPEXCEPTION_POINTERS e){
	STACKFRAME64 *stackFrames=instance->mStackFrames;
	int frameCount=instance->backtrace(stackFrames,MAX_STACKFRAMES,GetCurrentProcess(),GetCurrentThread(),e->ContextRecord);
	instance->handleFrames(stackFrames,frameCount,GetCurrentProcess(),GetCurrentThread());
	instance->errorHandled();
	return EXCEPTION_EXECUTE_HANDLER;
}

void Win32ErrorHandler::errorHandled(){
	abort();
}

int Win32ErrorHandler::backtrace(STACKFRAME64 *stackFrames,int maxFrameCount,HANDLE process,HANDLE thread,CONTEXT *context){
	bool suspendedThread=false;
	CONTEXT currentContext;
	if(context==NULL){
		context=&currentContext;

		if(thread==GetCurrentThread()){
			GET_CURRENT_CONTEXT(*context,CONTEXT_FULL);
		}
		else{
			suspendedThread=true;
			SuspendThread(thread);
			memset(context,0,sizeof(CONTEXT));
			context->ContextFlags=CONTEXT_FULL;
			if(GetThreadContext(thread,context)==FALSE){
				ResumeThread(thread);
				return -1;
			}
		}
	}

	DWORD imageType=initializeStackFrame(&stackFrames[0],context);
	int frameCount;
	for(frameCount=0;frameCount<maxFrameCount;++frameCount){
		if(frameCount>0){
			stackFrames[frameCount]=stackFrames[frameCount-1];
		}
		if(!StackWalk64(imageType,process,thread,&stackFrames[frameCount],context,NULL,SymFunctionTableAccess64,SymGetModuleBase64,NULL)){
			break;
		}
	}

	if(suspendedThread){
		ResumeThread(thread);
		suspendedThread=false;
	}

	return frameCount;
}

DWORD Win32ErrorHandler::initializeStackFrame(STACKFRAME64 *s,CONTEXT *c){
	memset(s,0,sizeof(STACKFRAME64));
	DWORD imageType=0;
	#ifdef _M_IX86
		imageType = IMAGE_FILE_MACHINE_I386;
		s->AddrPC.Offset = c->Eip;
		s->AddrPC.Mode = AddrModeFlat;
		s->AddrFrame.Offset = c->Ebp;
		s->AddrFrame.Mode = AddrModeFlat;
		s->AddrStack.Offset = c->Esp;
		s->AddrStack.Mode = AddrModeFlat;
	#elif _M_X64
		imageType = IMAGE_FILE_MACHINE_AMD64;
		s->AddrPC.Offset = c->Rip;
		s->AddrPC.Mode = AddrModeFlat;
		s->AddrFrame.Offset = c->Rsp;
		s->AddrFrame.Mode = AddrModeFlat;
		s->AddrStack.Offset = c->Rsp;
		s->AddrStack.Mode = AddrModeFlat;
	#elif _M_IA64
		imageType = IMAGE_FILE_MACHINE_IA64;
		s->AddrPC.Offset = c->StIIP;
		s->AddrPC.Mode = AddrModeFlat;
		s->AddrFrame.Offset = c->IntSp;
		s->AddrFrame.Mode = AddrModeFlat;
		s->AddrBStore.Offset = c->RsBSP;
		s->AddrBStore.Mode = AddrModeFlat;
		s->AddrStack.Offset = c->IntSp;
		s->AddrStack.Mode = AddrModeFlat;
	#else
		#error "Platform not supported!"
	#endif
	return imageType;
}

void Win32ErrorHandler::handleFrames(STACKFRAME64 *stackFrames,int numFrames,HANDLE process,HANDLE thread){
	SYMBOL *symbol=&mWorkingSymbol;
	memset(symbol,0,sizeof(mWorkingSymbol));
	symbol->SizeOfStruct=sizeof(IMAGEHLP_SYMBOL64);
	symbol->MaxNameLength=MAX_NAME_LENGTH;

	CHAR *name=mWorkingName;
	memset(name,0,sizeof(mWorkingName));

	DWORD64 symbolOffset=0;
	DWORD lineOffset=0;

	IMAGEHLP_LINE64 *line=&mWorkingLine;
	memset(line,0,sizeof(mWorkingLine));
	line->SizeOfStruct=sizeof(IMAGEHLP_LINE64);

	CHAR path[MAX_PATH];
	memset(path,0,sizeof(path));
	CHAR symbolPath[4096];
	memset(symbolPath,0,sizeof(symbolPath));
	if(GetCurrentDirectoryA(MAX_PATH,path)>0){
		strcat(symbolPath,path);
		strcat(symbolPath,";");
	}

	if(GetModuleFileNameA(NULL,path,MAX_PATH)>0){
		char *p;
		for(p=(path+strlen(path)-1);p>=path;--p){
			if((*p=='\\') || (*p=='/') || (*p==':')){
				*p=0;
				break;
			}
		}
		if(strlen(path)>0){
			strcat(symbolPath,path);
			strcat(symbolPath,";");
		}
	}

	BOOL invadeProcess=true; // If false, then we would have to use SymLoadModule64 to load any modules we wanted
	BOOL result=SymInitialize(process,symbolPath,invadeProcess);

	DWORD options=SymGetOptions();
	options|=SYMOPT_LOAD_LINES;
	options|=SYMOPT_FAIL_CRITICAL_ERRORS;
	#if defined(TOADLET_DEBUG)
		options|=SYMOPT_DEBUG;
	#endif
	options&=~SYMOPT_UNDNAME;
	options&=~SYMOPT_DEFERRED_LOADS;
	options=SymSetOptions(options);

	String descriptions[MAX_STACKFRAMES];

	if(mListener!=NULL){
		mListener->startTrace();
	}

	int i;
	for(i=0;i<numFrames;++i){
		STACKFRAME64 *s=&stackFrames[i];
		if(s->AddrPC.Offset!=0){
			if(SymGetSymFromAddr64!=NULL && SymGetSymFromAddr64(process,s->AddrPC.Offset,&symbolOffset,symbol)){
				UnDecorateSymbolName(symbol->Name,name,MAX_NAME_LENGTH,UNDNAME_COMPLETE);
			}

			if(SymGetLineFromAddr64!=NULL){
				SymGetLineFromAddr64(process,s->AddrPC.Offset,&lineOffset,line);
			}

			descriptions[i]=String(name)+":"+(int)line->LineNumber;
			if(mListener!=NULL){
				mListener->traceFrame(descriptions[i]);
			}
		}
	}

	if(mListener!=NULL){
		mListener->endTrace();
	}

	SymCleanup(process);
}

}
}
