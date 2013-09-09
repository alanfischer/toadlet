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

int PacketMessageStream::readMessage(){
	mMessage.reset();

	if(mStream->read(mMessage.data,sizeof(mMessage.id) + sizeof(mMessage.length)) != sizeof(mMessage.id) + sizeof(mMessage.length)){
		return -1;
	}

	memcpy(&mMessage.id,mMessage.data,sizeof(mMessage.id));
	memcpy(&mMessage.length,mMessage.data + sizeof(mMessage.id),sizeof(mMessage.length));

	if(mMessage.id>mMaxID || mMessage.length>mMaxLength){
		return -1;
	}

	if(mMessage.maxLength==0 && mMessage.data.size() < mMessage.length + sizeof(mMessage.id) + sizeof(mMessage.length)){
		mMessage.data.resize(sizeof(mMessage.id) + sizeof(mMessage.length) + mMessage.length);
	}

	int total=0,amount=1;
	for(total=0;total<mMessage.length && amount>0;total+=amount){
		amount=mStream->read(mMessage.data + sizeof(mMessage.id) + sizeof(mMessage.length) + total,mMessage.length - total);
	}
	return mMessage.id;
}

bool PacketMessageStream::writeMessage(int message){
	mMessage.reset();
	mMessage.id=message;
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

	if(mMessage.id>mMaxID || mMessage.length>mMaxLength){
		return false;
	}

	memcpy(mMessage.data,&mMessage.id,sizeof(mMessage.id));
	memcpy(mMessage.data + sizeof(mMessage.id),&mMessage.length,sizeof(mMessage.length));

	int totalLength=sizeof(mMessage.id) + sizeof(mMessage.length) + mMessage.length;
	int amount=mStream->write(mMessage.data,totalLength);

	return amount==totalLength;
}
	
PacketMessageStream::Message::Message(int maxlen):
	id(0),
	length(maxlen),
	maxLength(maxlen),
	position(0),
	data(sizeof(id) + sizeof(length) + maxlen)
{}

void PacketMessageStream::Message::reset(){
	id=0;
	length=maxLength;
	position=0;
}

int PacketMessageStream::Message::read(tbyte *buffer,int length){
	length=toadlet::egg::math::Math::intMinVal(length,this->length - this->position);
	memcpy(buffer,this->data + sizeof(this->id) + sizeof(this->length) + this->position,length);
	this->position+=length;
	return length;
}

int PacketMessageStream::Message::write(const tbyte *buffer,int length){
	if(this->maxLength==0 && (this->length - this->position) < length){
		int extra = length - (this->length - this->position);
		this->length += extra;
		data.resize(sizeof(id) + sizeof(length) + this->length);
	}
	length=toadlet::egg::math::Math::intMinVal(length,this->length - this->position);
	memcpy(this->data + sizeof(this->id) + sizeof(this->length) + this->position,buffer,length);
	this->position+=length;
	return length;
}
