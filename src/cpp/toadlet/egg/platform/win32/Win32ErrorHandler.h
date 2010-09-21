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

#ifndef TOADLET_EGG_WIN32ERRORHANDLER_H
#define TOADLET_EGG_WIN32ERRORHANDLER_H

#include <toadlet/Types.h>
#include <toadlet/egg/StackTraceListener.h>
#include <windows.h>
#include "Win32ErrorHandler_dbghelp.h"

#define TOADLET_DEFINEPROCEDURE(ret,name,sig) \
	typedef ret (__stdcall *proc_##name) sig; \
	proc_##name name;

namespace toadlet{
namespace egg{

class TOADLET_API Win32ErrorHandler{
public:
	Win32ErrorHandler();
	~Win32ErrorHandler();

	virtual void setStackTraceListener(StackTraceListener *listener){mListener=listener;}
	virtual StackTraceListener *getStackTraceListener(){return mListener;}

	virtual bool installHandler();
	virtual bool uninstallHandler();

	virtual void errorHandled();

	static Win32ErrorHandler *instance;

protected:
	const static int MAX_NAME_LENGTH=1024;

	struct SYMBOL:IMAGEHLP_SYMBOL64{
		char buffer[MAX_NAME_LENGTH];
	};

	static LONG __stdcall exceptionFilter(LPEXCEPTION_POINTERS e);

	int backtrace(STACKFRAME64 *stackFrames,int maxFrameCount,HANDLE process,HANDLE thread,CONTEXT *context);
	DWORD initializeStackFrame(STACKFRAME64 *s,CONTEXT *c);
	void handleFrames(STACKFRAME64 *stackFrames,int numFrames,HANDLE process,HANDLE thread);

	bool mInstalled;
	StackTraceListener *mListener;

	LPTOP_LEVEL_EXCEPTION_FILTER mOldFilter;
	HINSTANCE mDbgHelp;

	const static int MAX_STACKFRAMES=128;
	STACKFRAME64 mStackFrames[MAX_STACKFRAMES];

	SYMBOL mWorkingSymbol;
	CHAR mWorkingName[MAX_NAME_LENGTH];
	IMAGEHLP_LINE64 mWorkingLine;

	TOADLET_DEFINEPROCEDURE(BOOL,StackWalk64,(DWORD MachineType,HANDLE hProcess,HANDLE hThread,LPSTACKFRAME64 StackFrame,PVOID ContextRecord,PREAD_PROCESS_MEMORY_ROUTINE64 ReadMemoryRoutine,PFUNCTION_TABLE_ACCESS_ROUTINE64 FunctionTableAccessRoutine,PGET_MODULE_BASE_ROUTINE64 GetModuleBaseRoutine,PTRANSLATE_ADDRESS_ROUTINE64 TranslateAddress));
	TOADLET_DEFINEPROCEDURE(BOOL,SymInitialize,(IN HANDLE hProcess,IN PSTR UserSearchPath,IN BOOL fInvadeProcess));
	TOADLET_DEFINEPROCEDURE(DWORD,SymGetOptions,(VOID));
	TOADLET_DEFINEPROCEDURE(DWORD,SymSetOptions,(IN DWORD SymOptions));
	TOADLET_DEFINEPROCEDURE(BOOL,SymCleanup,(IN HANDLE hProcess));
	TOADLET_DEFINEPROCEDURE(PVOID,SymFunctionTableAccess64,(HANDLE hProcess,DWORD64 AddrBase));
	TOADLET_DEFINEPROCEDURE(DWORD64,SymGetModuleBase64,(IN HANDLE hProcess,IN DWORD64 dwAddr));
	TOADLET_DEFINEPROCEDURE(BOOL,SymGetSymFromAddr64,(IN HANDLE hProcess,IN DWORD64 dwAddr,OUT PDWORD64 pdwDisplacement,OUT PIMAGEHLP_SYMBOL64 Symbol));
	TOADLET_DEFINEPROCEDURE(DWORD,UnDecorateSymbolName,(PCSTR DecoratedName,PSTR UnDecoratedName,DWORD UndecoratedLength,DWORD Flags));
	TOADLET_DEFINEPROCEDURE(BOOL,SymGetLineFromAddr64,(IN HANDLE hProcess,IN DWORD64 dwAddr,OUT PDWORD pdwDisplacement,OUT PIMAGEHLP_LINE64 Line));
};

}
}

#undef TOADLET_DEFINEPROCEDURE

#endif

