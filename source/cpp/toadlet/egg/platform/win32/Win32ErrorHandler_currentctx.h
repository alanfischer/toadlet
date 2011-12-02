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

#include <windows.h>

#if defined(_M_IX86) && (_WIN32_WINNT <= 0x0500) && (_MSC_VER < 1400)
	#ifdef CURRENT_THREAD_VIA_EXCEPTION
		/// @todo: The following is not a "good" implementation, 
		// because the callstack is only valid in the "__except" block...
		#define GET_CURRENT_CONTEXT(c, contextFlags) \
			do { \
				memset(&c, 0, sizeof(CONTEXT)); \
				EXCEPTION_POINTERS *pExp = NULL; \
				__try { \
					throw 0; \
				} __except( ( (pExp = GetExceptionInformation()) ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_EXECUTE_HANDLER)) {} \
				if (pExp != NULL) \
					memcpy(&c, pExp->ContextRecord, sizeof(CONTEXT)); \
				c.ContextFlags = contextFlags; \
			} while(0);
	#else
		// The following should be enough for walking the callstack...
		#define GET_CURRENT_CONTEXT(c, contextFlags) \
			do { \
				memset(&c, 0, sizeof(CONTEXT)); \
				c.ContextFlags = contextFlags; \
				__asm    call x \
				__asm x: pop eax \
				__asm    mov c.Eip, eax \
				__asm    mov c.Ebp, ebp \
				__asm    mov c.Esp, esp \
			} while(0);
	#endif
#else
	#define GET_CURRENT_CONTEXT(c, contextFlags) \
		do { \
			memset(&(c), 0, sizeof(CONTEXT)); \
			(c).ContextFlags = contextFlags; \
			RtlCaptureContext(&(c)); \
		} while(0);
#endif
