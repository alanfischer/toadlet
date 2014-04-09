#include "PacketMessageStream.h"

using namespace toadlet;
using namespace toadlet::egg;
using namespace toadlet::egg::io;

PacketMessageStream::PacketMessageStream(Stream *stream,int maxID,int maxLength):
	mStream(stream),
	mMaxID(maxID),
	mMaxLength(maxLength),
	mMessage(0)
{}

int PacketMessageStream::read(tbyte *buffer,int length){
	return mMessage.read(buffer,length);
}

int PacketMessageStream::readMessage(){
	mMessage.reset();

	if(mStream->read(mMessage.data,sizeof(mMessage.header)) != sizeof(mMessage.header)){
		return -1;
	}

	memcpy(&mMessage.header,mMessage.data,sizeof(mMessage.header));

	if(mMessage.header.id>mMaxID || mMessage.header.length>mMaxLength){
		return -1;
	}

	if(mMessage.maxLength==0 && mMessage.data.size() < mMessage.header.length + sizeof(mMessage.header)){
		mMessage.data.resize(sizeof(mMessage.header) + mMessage.header.length);
	}

	int total=0,amount=1;
	for(total=0;total<mMessage.header.length && amount>0;total+=amount){
		amount=mStream->read(mMessage.data + sizeof(mMessage.header) + total,mMessage.header.length - total);
	}
	return mMessage.header.id;
}

int PacketMessageStream::write(const tbyte *buffer,int length){
	return mMessage.write(buffer,length);
}

bool PacketMessageStream::writeMessage(int message){
	mMessage.reset();
	mMessage.header.id=message;
	return true;
}

bool PacketMessageStream::reset(){
	mMessage.position=0;
	return true;
}

int PacketMessageStream::length(){
	return mMessage.header.length;
}

int PacketMessageStream::position(){
	return mMessage.position;
}

bool PacketMessageStream::seek(int offs){
	mMessage.position+=offs;
	return true;
}

bool PacketMessageStream::flush(){
	mMessage.header.length=mMessage.position;

	if(mMessage.header.id>mMaxID || mMessage.header.length>mMaxLength){
		return false;
	}

	memcpy(mMessage.data,&mMessage.header,sizeof(mMessage.header));

	int totalLength=sizeof(mMessage.header) + mMessage.header.length;
	int amount=mStream->write(mMessage.data,totalLength);

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
