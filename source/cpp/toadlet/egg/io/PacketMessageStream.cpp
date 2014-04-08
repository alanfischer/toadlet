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
	mMessage.length=mMessage.header.length;

	if(mMessage.header.id>mMaxID || mMessage.header.length>mMaxLength){
		return -1;
	}

	if(mMaxLength==0 && mMessage.data.size() < mMessage.length + sizeof(mMessage.header)){
		mMessage.data.resize(sizeof(mMessage.header) + mMessage.length);
	}

	int total=0,amount=1;
	for(total=0;total<mMessage.length && amount>0;total+=amount){
		amount=mStream->read(mMessage.data + sizeof(mMessage.header) + total,mMessage.length - total);
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
	return mMessage.length;
}

int PacketMessageStream::position(){
	return mMessage.position;
}

bool PacketMessageStream::seek(int offs){
	mMessage.position+=offs;
	return true;
}

bool PacketMessageStream::flush(){
	mMessage.length=mMessage.position;
	mMessage.header.length=mMessage.length;

	if(mMessage.header.id>mMaxID || mMessage.header.length>mMaxLength){
		return false;
	}

	memcpy(mMessage.data,&mMessage.header,sizeof(mMessage.header));

	int totalLength=sizeof(mMessage.header) + mMessage.length;
	int amount=mStream->write(mMessage.data,totalLength);

	return amount==totalLength;
}
	
PacketMessageStream::Message::Message(int maxlen):
	length(maxlen),
	position(0),
	data(sizeof(header) + maxlen)
{}

void PacketMessageStream::Message::reset(){
	position=0;
}

int PacketMessageStream::Message::read(tbyte *buffer,int length){
	length=toadlet::egg::math::Math::intMinVal(length,this->length - this->position);
	memcpy(buffer,this->data + sizeof(this->header) + this->position,length);
	this->position+=length;
	return length;
}

int PacketMessageStream::Message::write(const tbyte *buffer,int length){
	if((this->length - this->position) < length){
		int extra = length - (this->length - this->position);
		this->length += extra;
		data.resize(sizeof(header) + this->length);
	}
	length=toadlet::egg::math::Math::intMinVal(length,this->length - this->position);
	memcpy(this->data + sizeof(this->header) + this->position,buffer,length);
	this->position+=length;
	return length;
}
