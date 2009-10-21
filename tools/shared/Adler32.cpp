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

#include "Adler32.h"

using namespace toadlet;

Adler32::Adler32(){
	mValue=1;
}

void Adler32::update(int bval) {
	int s1=mValue & 0xffff;
	int s2=((unsigned int)mValue)>> 16;

	s1 = (s1 + (bval & 0xFF)) % BASE;
	s2 = (s1 + s2) % BASE;

	mValue=(s2 << 16) + s1;
}

void Adler32::update(char *buf,int len){
	int s1=mValue & 0xffff;
	int s2=((unsigned int)mValue)>> 16;

	int off=0;
	while(len>0){
		int n=3800;
		if(n>len){
			n=len;
		}
		len-=n;
		while(--n>=0){
			s1 = s1 + (buf[off++] & 0xFF);
			s2 = s2 + s1;
		}
		s1 %= BASE;
		s2 %= BASE;
	}

	mValue=(s2 << 16) | s1;
}

void Adler32::reset() {
    mValue=1;
}

int64 Adler32::getValue() {
	return (int64)mValue & TOADLET_MAKE_INT64(0xffffffff);
}
