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

#include <toadlet/egg/Categories.h>
#include <toadlet/egg/Error.h>
#include <toadlet/egg/io/FileStream.h>
#include "OSXBundleArchive.h"
#include <Foundation/Foundation.h>
#include <Foundation/NSBundle.h>

namespace toadlet{
namespace egg{

OSXBundleArchive::OSXBundleArchive():
	mBundle(nil)
{
}

OSXBundleArchive::~OSXBundleArchive(){
	destroy();
}

void OSXBundleArchive::destroy(){
	mBundle=nil;
}

bool OSXBundleArchive::open(void *bundle){
	mBundle=bundle;

	return true;
}

Stream::ptr OSXBundleArchive::openStream(const String &name){
	NSString *filePath=[((NSBundle*)mBundle) pathForResource:name ofType:nil];
	FileStream::ptr stream(new FileStream(filePath,FileStream::Open_READ_BINARY));
	if(stream->closed()){
		Error::unknown(Categories::TOADLET_EGG,"unable to find stream");
		return NULL;
	}
	return stream;
}

}
}
