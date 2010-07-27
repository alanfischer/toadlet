#ifndef STACKTRACELISTENER_H
#define STACKTRACELISTENER_H

namespace toadlet{
namespace egg{

class StackTraceListener{
public:
	virtual void backtrace(char **stack,int count)=0;
};

}
}

#endif

