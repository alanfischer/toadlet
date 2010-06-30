To convert a Win32 project to WinCE:

1. Add Debug & Release targets for PocketPC 2003 type.
2. Add the following definitions to the compile step and the resource step: ARM,_WIN32_WCE,
3. If you have a WinMain function, make sure it takes an LPSTR, not an LPTSTR
4. Change the Subsystem to WINDOWSCE
5. If you receive a linker error about "void __cdecl `eh vector destructor iterator'(void *,unsigned int,int,void (__cdecl*)(void *))",
	then disable exceptions.
6. Add the following to your .rc file:
	HI_RES_AWARE CEUX 
	BEGIN
		0x0001
	END