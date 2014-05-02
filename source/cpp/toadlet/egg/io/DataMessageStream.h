#ifndef TOADLET_EGG_IO_DATAMESSAGESTREAM_H
#define TOADLET_EGG_IO_DATAMESSAGESTREAM_H

#include <toadlet/egg/io/DataStream.h>
#include <toadlet/egg/io/MessageStream.h>

namespace toadlet{
namespace egg{
namespace io{

class TOADLET_API DataMessageStream:public DataStream,public MessageStream{
public:
	TOADLET_IOBJECT(DataMessageStream);

	DataMessageStream(MessageStream *stream):DataStream(stream),mStream(stream){}
	
	void close(){mStream->close();}
	bool closed(){return mStream->closed();}

	bool readable(){return mStream->readable();}
	int read(tbyte *buffer,int length){return mStream->read(buffer,length);}
	int readMessage(){return mStream->readMessage();}

	bool writeable(){return mStream->writeable();}
	int write(const tbyte *buffer,int length){return mStream->write(buffer,length);}
	bool writeMessage(int message){return mStream->writeMessage(message);}

	bool reset(){return mStream->reset();}
	int length(){return mStream->length();}
	int position(){return mStream->position();}
	bool seek(int offs){return mStream->seek(offs);}
	bool flush(){return mStream->flush();}
	
protected:
	MessageStream::ptr mStream;
};

}
}
}

#endif
