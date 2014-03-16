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

#ifndef TOADLET_EGG_ANDROIDASSETARCHIVE_H
#define TOADLET_EGG_ANDROIDASSETARCHIVE_H

#include <toadlet/egg/BaseResource.h>
#include <toadlet/egg/io/BaseArchive.h>
#include "JStream.h"
#include <jni.h>

namespace toadlet{
namespace egg{

class TOADLET_API AndroidAssetArchive:public BaseArchive{
public:
	TOADLET_OBJECT(AndroidAssetArchive);

	AndroidAssetArchive(JNIEnv *jenv,jobject jassetManager);
	virtual ~AndroidAssetArchive();

	void destroy(){}

	bool openStream(const String &name,StreamRequest *request);
	bool openResource(const String &name,ResourceRequest *request){return false;}

	const Collection<String> &getEntries(){return mEntries;}

protected:
	JNIEnv *getEnv() const;

	JStream::ptr mStream;
	Collection<String> mEntries;
	JavaVM *vm;
	jobject obj;
	jmethodID closeManagerID,openManagerID,availableStreamID,readStreamID,closeStreamID;
};

}
}

#endif
