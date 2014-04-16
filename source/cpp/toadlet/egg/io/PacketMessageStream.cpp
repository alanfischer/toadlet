#include "PacketMessageStream.h"
#include <toadlet/egg/Log.h>

using namespace toadlet;
using namespace toadlet::egg;
using namespace toadlet::egg::io;

PacketMessageStream::PacketMessageStream(Stream *stream,int maxID,int maxLength):
	mStream(stream),
	mMaxID(maxID),
	mMaxLength(maxLength),
	mGroup(0),
	message(NULL)
{
	mReadMessages.resize(1);
	mWriteMessages.resize(1);
}

int PacketMessageStream::read(tbyte *buffer,int length){
	return message->read(buffer,length);
}

int PacketMessageStream::readMessage(){
	Header header;

	if(mStream->read((tbyte*)&header,sizeof(header)) != sizeof(header)){
		return -1;
	}

	if(header.id>mMaxID || header.length>mMaxLength){
		return -1;
	}

	if((header.sequence & (Sequence_START | Sequence_END)) != (Sequence_START | Sequence_END)){
		return -1;
	}

	message=&mReadMessages[0];
	message->reset();

	memcpy(message->data,&header,sizeof(header));
	message->header=header;

	if(message->maxLength==0 && message->data.size() < message->header.length + sizeof(message->header)){
		message->data.resize(sizeof(message->header) + message->header.length);
	}

	int total=0,amount=1;
	for(total=0;total<message->header.length && amount>0;total+=amount){
		amount=mStream->read(message->data + sizeof(message->header) + total,message->header.length - total);
	}
	return message->header.id;
}

int PacketMessageStream::write(const tbyte *buffer,int length){
	return message->write(buffer,length);
}

bool PacketMessageStream::writeMessage(int id){
	message=&mWriteMessages[0];
	message->reset();

	message->header.id=id;
	return true;
}

bool PacketMessageStream::reset(){
	message->position=0;
	return true;
}

int PacketMessageStream::length(){
	return message->header.length;
}

int PacketMessageStream::position(){
	return message->position;
}

bool PacketMessageStream::seek(int offs){
	message->position+=offs;
	return true;
}

bool PacketMessageStream::flush(){
	message->header.length=message->position;
	message->header.sequence |= (Sequence_START | Sequence_END);

	if(message->header.id>mMaxID || message->header.length>mMaxLength){
		return false;
	}

	memcpy(message->data,&message->header,sizeof(message->header));

	int totalLength=sizeof(message->header) + message->header.length;
	int amount=mStream->write(message->data,totalLength);

	return amount==totalLength;
}

PacketMessageStream::Message::Message(int maxlen):
	maxLength(maxlen),
	data(sizeof(header) + maxlen)
{
	reset();
}

void PacketMessageStream::Message::reset(){
	header.id=0;
	header.length=maxLength;
	position=0;
}

int PacketMessageStream::Message::read(tbyte *buffer,int length){
	length=toadlet::egg::math::Math::intMinVal(length,header.length - position);
	memcpy(buffer,data + sizeof(header) + position,length);
	position+=length;
	return length;
}

int PacketMessageStream::Message::write(const tbyte *buffer,int length){
	if(maxLength==0 && (header.length - position) < length){
		int extra = length - (header.length - position);
		header.length += extra;
		data.resize(sizeof(header) + header.length);
	}
	length=toadlet::egg::math::Math::intMinVal(length,header.length - position);
	memcpy(data + sizeof(header) + position,buffer,length);
	position+=length;
	return length;
}
