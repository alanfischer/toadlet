/********** Copyright header - do not remove **********
 *
 * The Toadlet Engine
 *
 * Copyright 2009, Lightning Toads Productions, LLC
 *
 * Author(s): Alan Fischer, Andrew Fischer
 *
 * This file is part of The Toadlet Engine.
 *
 * The Toadlet Engine is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * The Toadlet Engine is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.

 * You should have received a copy of the GNU Lesser General Public License
 * along with The Toadlet Engine.  If not, see <http://www.gnu.org/licenses/>.
 *
 ********** Copyright header - do not remove **********/

#ifndef TOADLET_EGG_JSTREAM_H
#define TOADLET_EGG_JSTREAM_H

#include <toadlet/egg/Object.h>
#include <toadlet/egg/io/Stream.h>
#include <jni.h>

namespace toadlet{
namespace egg{

class TOADLET_API JStream:public Object,public Stream{
public:
	TOADLET_JOBJECT(JStream);

	JStream(JNIEnv *env,jobject streamObj=NULL);
	virtual ~JStream();

	bool open(jobject streamObj);
	void close();
	bool closed(){return istreamObj==NULL && ostreamObj==NULL;}
	
	bool readable(){return istreamObj!=NULL;}
	int read(tbyte *buffer,int length);

	bool writeable(){return ostreamObj!=NULL;}
	int write(const tbyte *buffer,int length);

	bool reset();
	int length();
	int position();
	bool seek(int offs);
	bool flush();

protected:
	JNIEnv *env;
	jclass istreamClass,ostreamClass;
	jobject istreamObj,ostreamObj;
	jmethodID closeIStreamID,readIStreamID,availableIStreamID,markIStreamID,resetIStreamID,skipIStreamID;
	jmethodID closeOStreamID,writeOStreamID,flushOStreamID;

	int bufferLength;
	jbyteArray bufferObj;
	
	int current;
};

}
}

#endif
