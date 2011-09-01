#include <toadlet/Types.h>

#if defined(TOADLET_PLATFORM_WIN32)
	#include <windows.h>
#endif

extern int toadletMain(int argc,char **argv);

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

	#if defined(TOADLET_PLATFORM_WINCE)
		arg=argo=(char*)malloc(wcslen(szCmd)+1);
	#else
		arg=argo=(char*)malloc(strlen(szCmd)+1);
	#endif

    // Count the arguments
    argc=1;
    int i;
	#if defined(TOADLET_PLATFORM_WINCE)
	    for(i=0;i<=wcslen(szCmd);++i){
	#else
	    for(i=0;i<=strlen(szCmd);++i){
	#endif
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
	#if defined(TOADLET_PLATFORM_WINCE)
	    for(i=0;i<=wcslen(szCmd);++i){
	#else
	    for(i=0;i<=strlen(szCmd);++i){
	#endif
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
	#if defined(_UNICODE)
		for(i=0;i<=wcslen(tfilename);++i){
	#else
		for(i=0;i<=strlen(tfilename);++i){
	#endif
		filename[i]=tfilename[i];
    }
    
    argv[0]=filename;

	int result=toadletMain(argc,argv);

	free(argo);
	free(argv);

	return result;
}
#endif

int main(int argc,char **argv){
	return toadletMain(argc,argv);
}
