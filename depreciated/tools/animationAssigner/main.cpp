#error "This is not updated to the most recent toadlet"

#include "AnimationAssigner.h"

#ifdef TOADLET_DEBUG
	int main(int argc,char **argv){
#else
	int __stdcall WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow){

	int    argc;
	char** argv;

	char*  arg;
	int    index;

    // count the arguments
	argc=1;
	arg=lpCmdLine;
	while(arg[0]!=0){
		while(arg[0]!=0 && arg[0]==' '){
			arg++;
		}
		if(arg[0]!=0){
			argc++;
			while(arg[0]!=0 && arg[0]!=' '){
				arg++;
			}
		}
	}    
    
	// tokenize the arguments
	argv=(char**)malloc(argc * sizeof(char*));
	arg=lpCmdLine;
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

	// put the program name into argv[0]
	char filename[_MAX_PATH];
	GetModuleFileName(NULL, filename, _MAX_PATH);
	argv[0] = filename;

#endif
	AnimationAssigner assigner;

	assigner.init(argc,argv);

	return assigner.run();
}

