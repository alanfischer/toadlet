#ifndef TOADLET_EGG_IO_MESSAGESTREAM_H
#define TOADLET_EGG_IO_MESSAGESTREAM_H

#include <toadlet/egg/io/Stream.h>

namespace toadlet{
namespace egg{
namespace io{

class TOADLET_API MessageStream:public Stream{
public:
	TOADLET_INTERFACE(MessageStream);

	virtual int readMessage()=0;
	virtual bool writeMessage(int message)=0;
};

}
}
}

#endif
