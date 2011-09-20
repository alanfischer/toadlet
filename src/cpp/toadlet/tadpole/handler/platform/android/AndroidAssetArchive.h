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

#ifndef TOADLET_TADPOLE_HANDLER_ANDROIDASSETARCHIVE_H
#define TOADLET_TADPOLE_HANDLER_ANDROIDASSETARCHIVE_H

#include <toadlet/egg/BaseResource.h>
#include <toadlet/egg/io/Archive.h>
#include <toadlet/tadpole/Types.h>
#include <jni.h>

namespace toadlet{
namespace tadpole{
namespace handler{

class TOADLET_API AndroidAssetArchive:public Archive,public BaseResource{
	TOADLET_BASERESOURCE_PASSTHROUGH(Archive);
public:
	TOADLET_SHARED_POINTERS(AndroidAssetArchive);

	AndroidAssetArchive(JNIEnv *jenv,jobject jassetManager);
	virtual ~AndroidAssetArchive();

	void destroy();

	Stream::ptr openStream(const String &name);
	Resource::ptr openResource(const String &name){return NULL;}

	Collection<String>::ptr getEntries();

protected:
	Collection<String>::ptr mEntries;
	JNIEnv *env;
	jobject assetManager;
	jmethodID closeManagerID,openManagerID,availableStreamID,readStreamID;
};

}
}
}

#endif