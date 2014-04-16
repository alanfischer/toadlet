#include "ContainerTest.h"
#include "MathTest.h"
#include "StreamTest.h"
#include "NodeTest.h"

int main(int argc,char **argv){
	ContainerTest().run();
	MathTest().run();
	StreamTest().run();
	NodeTest().run();

	Log::destroy();
}
