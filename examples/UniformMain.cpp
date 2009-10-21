#include <toadlet/Types.h>

#if defined(TOADLET_PLATFORM_WIN32)
#	include <windows.h>
#elif defined(TOADLET_PLATFORM_NDS)
#	include <nds.h>
#endif

int uniformMain(int argc,char **argv);

#if defined(TOADLET_PLATFORM_WIN32) 
#if defined(TOADLET_PLATFORM_WINCE)
INT WINAPI WinMain(HINSTANCE hInst,HINSTANCE,LPTSTR szCmd,INT cmdShow){
#else
INT WINAPI WinMain(HINSTANCE hInst,HINSTANCE,LPSTR szCmd,INT cmdShow){
#endif
	int argc=0;
    char **argv=NULL;

    char *argo=NULL,*arg=NULL;
    int index=0;

#	if defined(TOADLET_PLATFORM_WINCE)
		arg=argo=(char*)malloc(wcslen(szCmd)+1);
#	else
		arg=argo=(char*)malloc(strlen(szCmd)+1);
#	endif

    // Count the arguments
    argc=1;
    int i;
#	if defined(TOADLET_PLATFORM_WINCE)
	    for(i=0;i<=wcslen(szCmd);++i){
#	else
	    for(i=0;i<=strlen(szCmd);++i){
#	endif
		arg[i]=szCmd[i];
    }

    while(arg[0]!=0){
        while (arg[0]!=0 && arg[0]==' '){
            arg++;
        }

        if(arg[0]!=0){
            argc++;
            while(arg[0]!=0 && arg[0]!=' '){
                arg++;
            }
        }
    }
    arg=argo;

    // Tokenize the arguments
    argv=(char**)malloc(argc * sizeof(char*));
#	if defined(TOADLET_PLATFORM_WINCE)
	    for(i=0;i<=wcslen(szCmd);++i){
#	else
	    for(i=0;i<=strlen(szCmd);++i){
#	endif
		arg[i]=szCmd[i];
    }
    index=1;

    while(arg[0]!=0){
        while(arg[0]!=0 && arg[0]==' '){
            arg++;
        }

        if(arg[0]!=0){
            argv[index]=arg;
            index++;

            while(arg[0]!=0 && arg[0]!=' '){
                arg++;
            }

            if(arg[0]!=0){
                arg[0]=0;    
                arg++;
            }
        }
    }    

    // Put the program name into argv[0]
    TCHAR tfilename[_MAX_PATH];
    CHAR filename[_MAX_PATH];
    GetModuleFileName(NULL,tfilename,_MAX_PATH);
#	if defined(_UNICODE)
		for(i=0;i<=wcslen(tfilename);++i){
#	else
		for(i=0;i<=strlen(tfilename);++i){
#	endif
		filename[i]=tfilename[i];
    }
    
    argv[0]=filename;

	int result=uniformMain(argc,argv);

	free(argo);
	free(argv);

	return result;
}
#endif

int main(int argc,char **argv){
#	if defined(TOADLET_PLATFORM_NDS)
		// Turn on everything
		powerON(POWER_ALL);
		
		// Setup the Main screen for 3D 
		videoSetMode(MODE_0_3D);

		// IRQ basic setup (not required but nice)
		irqInit();
		irqSet(IRQ_VBLANK, 0);

		videoSetModeSub(MODE_0_2D | DISPLAY_BG0_ACTIVE);	//sub bg 0 will be used to print text
		vramSetBankC(VRAM_C_SUB_BG); 

		SUB_BG0_CR = BG_MAP_BASE(31);
		
		BG_PALETTE_SUB[255] = RGB15(31,31,31);	//by default font will be rendered with color 255
		
		//consoleInit() is a lot more flexible but this gets you up and running quick
		consoleInitDefault((u16*)SCREEN_BASE_BLOCK_SUB(31), (u16*)CHAR_BASE_BLOCK_SUB(0), 16);
#	endif

	return uniformMain(argc,argv);
}

#if defined(TOADLET_PLATFORM_NDS)
// Necessary because with toadlet built as a static library for NDS, gcc does not force
//  these dsgl functions to be linked.  This could probably be removed by either adjusting compile
//  parameters, or ideally removing the use of the dsgl functions completely, and calling into the DS
//  directly in the renderer
extern "C" void forceFunctionLink(){
	glInit();
}
#endif
