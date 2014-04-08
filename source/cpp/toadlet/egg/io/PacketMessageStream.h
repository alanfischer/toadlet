#ifndef TOADLET_EGG_IO_PACKETMESSAGESTREAM_H
#define TOADLET_EGG_IO_PACKETMESSAGESTREAM_H

#include <toadlet/egg/Object.h>
#include <toadlet/egg/Extents.h>
#include <toadlet/egg/Collection.h>
#include <toadlet/egg/io/MessageStream.h>

namespace toadlet{
namespace egg{
namespace io{

class TOADLET_API PacketMessageStream:public Object,public MessageStream{
public:
	TOADLET_IOBJECT(PacketMessageStream);

	PacketMessageStream(Stream *stream,int maxID=Extents::MAX_INT,int maxMessageLength=Extents::MAX_INT);
	
	void close(){mStream->close();}
	bool closed(){return mStream->closed();}

	bool readable(){return mStream->readable();}
	int read(tbyte *buffer,int length);
	int readMessage();

	bool writeable(){return mStream->writeable();}
	int write(const tbyte *buffer,int length);
	bool writeMessage(int message);

	bool reset();
	int length();
	int position();
	bool seek(int offs);
	bool flush();

	void setMaxID(int maxID){mMaxID=maxID;}
	int getMaxID() const{return mMaxID;}

	void setMaxMessageLength(int maxLength){mMaxLength=maxLength;}
	int getMaxLength() const{return mMaxLength;}

protected:
	class Header{
	public:
		Header():header(0),id(0),sequence(0),length(0){}

		uint16 header;
		uint16 id;
		uint16 sequence;
		uint16 length;
	};

	class TOADLET_API Message{
	public:
		Message(int maxlen);
		void reset();
		int read(tbyte *buffer,int length);
		int write(const tbyte *buffer,int length);

		Header header;

		int length;
		int maxLength;
		int position;

		Collection<tbyte> data;
	};

	Stream::ptr mStream;
	int mMaxID;
	int mMaxLength;
	Message mMessage;
};

}
}
}

#endif
