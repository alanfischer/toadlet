/********** Copyright header - do not remove **********
 *
 * The Toadlet Engine 
 *
 * Copyright (C) 2005, Lightning Toads Productions, LLC
 *
 * Author(s): Alan Fischer
 * 
 * All source code for the Toadlet Engine, including 
 * this file, is the sole property of Lightning Toads 
 * Productions, LLC. It has been developed on our own 
 * time, with our own tools, on our own hardware. None
 * of this code may be observed, used or changed 
 * without our explicit permission. 
 *
 *  - Lightning Toads Productions, LLC
 *
 ********** Copyright header - do not remove **********/

#include "LineInputStream.h"

using namespace toadlet::egg;

// Note: This class is very unoptimized
LineInputStream::LineInputStream(InputStream *in){
	mIn=in;
	mHasExtraChar=false;
	mExtraChar=0;
	mFinished=false;
}

LineInputStream::~LineInputStream(){
}

int LineInputStream::read(char *buffer,int length){
	int i;
	for(i=0;i<length && mFinished==false;++i){
		buffer[i]=getChar();
	}

	return i;
}

bool LineInputStream::reset(){
	return mIn->reset();
}

int LineInputStream::available(){
	return mIn->available();
}

bool LineInputStream::seek(int offs){
	return mIn->seek(offs);
}

void LineInputStream::close(){
	mIn->close();
}

String LineInputStream::readLine(){
	String s;
	char c=0;
	while(mFinished==false && c!='\n' && c!='\r'){
		c=getChar();
		if(c!='\n' && c!='\r'){
			s=s+c; // TODO: Replace this with a non destructive string method
		}
	}
	if(c=='\r'){
		c=getChar();
		if(c!='\n'){
			pushChar(c);
		}
	}
	return s;
}

void LineInputStream::pushChar(char c){
	mHasExtraChar=true;
	mExtraChar=c;
}

char LineInputStream::getChar(){
	if(mHasExtraChar){
		mHasExtraChar=false;
		return mExtraChar;
	}
	else{
		mExtraChar=0;
		int i=mIn->read(&mExtraChar,1);
		if(i==0){
			mFinished=true;
		}
		return mExtraChar;
	}
}

bool LineInputStream::finished(){
	return mFinished;
}
