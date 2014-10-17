#include "PacketMessageStream.h"
#include <toadlet/egg/Categories.h>
#include <toadlet/egg/Error.h>
#include <toadlet/egg/Log.h>

using namespace toadlet;
using namespace toadlet::egg;
using namespace toadlet::egg::io;

PacketMessageStream::PacketMessageStream(Stream *stream,int maxID,int maxLength):
	mStream(stream),
	mMaxID(maxID),
	mMaxLength(maxLength),
	mGroup(0),
	mMessages(NULL),
	message(NULL)
{
	mReadMessages.resize(1,Message(maxLength));
	mWriteMessages.resize(1,Message(maxLength));
}

int PacketMessageStream::read(tbyte *buffer,int length){
	int amount=0,total=0;
	while(true){
		amount=message->read(buffer + total,length - total);
		total+=amount;
		int sequence=(message->header.sequence & Sequence_MASK);
		if(total < length && sequence+1<mMessages->size()){
			message=&mMessages->at(sequence+1);
		}
		else{
			break;
		}
	}
	return total;
}

int PacketMessageStream::readMessage(){
	Header header;

	if(mStream->read((tbyte*)&header,sizeof(header)) != sizeof(header)){
		return -1;
	}

	if(header.id>mMaxID || (mMaxLength>0 && header.length>mMaxLength)){
		return -1;
	}

	if((header.sequence & Sequence_START)==Sequence_START){
		mReadMessages.resize(1);
	}
	else{
		Message *previous=&mMessages->at(mMessages->size()-1);
		if(	previous->header.group!=header.group ||
			(previous->header.sequence & Sequence_END)==Sequence_END ||
			(previous->header.sequence & Sequence_MASK)+1 != (header.sequence & Sequence_MASK)){
			return -1;
		}
		mMessages->resize(mMessages->size()+1,Message(mMaxLength));
	}

	mMessages=&mReadMessages;
	message=&mReadMessages[mReadMessages.size()-1];
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

	if((message->header.sequence & Sequence_END)==Sequence_END){
		message=&mMessages->at(0);
		return message->header.id;
	}
	else{
		return 0;
	}
}

int PacketMessageStream::write(const tbyte *buffer,int length){
	int amount=0,total=0;
	while(true){
		amount=message->write(buffer + total,length - total);
		total+=amount;
		if(total < length){
			int id=message->header.id;
			int group=message->header.group;
			int sequence=(message->header.sequence & Sequence_MASK);
			mMessages->resize(mMessages->size()+1,Message(mMaxLength));
			message=&mMessages->at(mMessages->size()-1);
			message->reset(id);
			message->header.group=group;
			message->header.sequence |= sequence+1;
		}
		else{
			break;
		}
	}
	return total;
}

bool PacketMessageStream::writeMessage(int id){
	mWriteMessages.resize(1);
	mMessages=&mWriteMessages;
	message=&mWriteMessages[0];
	message->reset(id);
	message->header.group=mGroup++;
	message->header.sequence |= Sequence_START;
	return true;
}

bool PacketMessageStream::reset(){
	if(mMessages!=NULL){
		message=&mMessages->at(0);
	}
	message->position=0;
	return true;
}

int PacketMessageStream::length(){
	int length=0;
	if(mMessages!=NULL){
		for(int i=0;i<mMessages->size();++i){
			length+=mMessages->at(i).header.length;
		}
	}
	return length;
}

int PacketMessageStream::position(){
	int position=0;
	if(mMessages!=NULL){
		for(int i=0;i<mMessages->size() && message!=&mMessages->at(i);++i){
			position+=mMessages->at(i).header.length;
		}
	}
	position+=message->position;
	return position;
}

bool PacketMessageStream::seek(int offs){
	if(mMessages!=NULL){
		for(int i=0;i<mMessages->size();++i){
			message=&mMessages->at(i);
			if(offs>=message->header.length){
				offs-=message->header.length;
			}
			else{
				message->position=offs;
				break;
			}
		}
	}
	return true;
}

bool PacketMessageStream::flush(){
	message->header.length=message->position;
	message->header.sequence |= Sequence_END;

	for(int i=0;i<mMessages->size();++i){
		Message *message=&mMessages->at(i);

		if(message->header.id>mMaxID || (mMaxLength>0 && message->header.length>mMaxLength)){
			return false;
		}

		memcpy(message->data,&message->header,sizeof(message->header));

		int totalLength=sizeof(message->header) + message->header.length;
		int amount=mStream->write(message->data,totalLength);

		if(amount != totalLength){
			return false;
		}
	}

	return true;
}

PacketMessageStream::Message::Message(int maxlen):
	maxLength(maxlen),
	data(sizeof(header) + maxlen)
{
	reset();
}

void PacketMessageStream::Message::reset(int id){
	header.id=id;
	header.group=0;
	header.sequence=0;
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
