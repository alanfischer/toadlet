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

#ifndef LINEINPUTSTREAM_H
#define LINEINPUTSTREAM_H

#include <toadlet/egg/io/InputStream.h>
#include <toadlet/egg/String.h>

class LineInputStream:public toadlet::egg::io::InputStream{
public:
	LineInputStream(toadlet::egg::io::InputStream *in);
	virtual ~LineInputStream();

	virtual int read(char *buffer,int length);
	virtual bool reset();
	virtual int available();
	virtual bool seek(int offs);
	virtual void close();

	virtual toadlet::egg::String readLine();
	virtual bool finished();

protected:
	void pushChar(char c);
	char getChar();

	toadlet::egg::io::InputStream *mIn;
	char mExtraChar;
	int mHasExtraChar;
	bool mFinished;
};

#endif
