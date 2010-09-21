#ifndef STACKTRACELISTENER_H
#define STACKTRACELISTENER_H

namespace toadlet{
namespace egg{

class StackTraceListener{
public:
	virtual void startTrace()=0;
	virtual void traceFrame(const char *description)=0; ///@todo: Change this to file,function,line parameters
	virtual void endTrace()=0;
};

}
}

#endif

