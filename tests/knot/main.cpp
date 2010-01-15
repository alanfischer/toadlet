#include <toadlet/egg/Logger.h>
#include "../quicktest.h"

int main(int argc,char **argv){
	toadlet::egg::Logger::getInstance()->setMasterReportingLevel(toadlet::egg::Logger::Level_ALERT);
	QT_SET_OUTPUT(&std::cout);
	QT_RUN_TESTS;
	getchar();
}
