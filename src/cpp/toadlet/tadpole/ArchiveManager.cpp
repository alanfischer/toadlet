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

#include <toadlet/egg/System.h>
#include <toadlet/egg/io/InputStream.h>
#include <toadlet/egg/io/OutputStream.h>
#include <toadlet/egg/io/FileInputStream.h>
#include <toadlet/tadpole/ArchiveManager.h>

using namespace toadlet::egg;
using namespace toadlet::egg::io;

namespace toadlet{
namespace tadpole{

ArchiveManager::ArchiveManager():ResourceManager(NULL){
	setArchive(this);
}

ArchiveManager::~ArchiveManager(){
	destroy();
}

void ArchiveManager::destroy(){
}

void ArchiveManager::setDirectory(const String &directory){
	if(directory.length()>0){
		mDirectory=directory+"/";
	}
	else{
		mDirectory=directory;
	}
}

InputStream::ptr ArchiveManager::openStream(const String &name){
	InputStream::ptr in;

	int i;
	for(i=0;i<mResources.size();++i){
		Archive::ptr archive=shared_static_cast<Archive>(mResources[i]);
		in=archive->openStream(name);
		if(in!=NULL){
			break;
		}
	}

	if(in==NULL){
		FileInputStream::ptr fin;
		if(System::absolutePath(name)==false){
			fin=FileInputStream::ptr(new FileInputStream(mDirectory+name));
		}
		else{
			fin=FileInputStream::ptr(new FileInputStream(name));
		}
		if(fin->isOpen()){
			in=fin;
		}
	}

	return in;
}

}
}
