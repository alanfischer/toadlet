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
