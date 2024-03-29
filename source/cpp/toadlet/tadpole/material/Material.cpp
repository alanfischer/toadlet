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

#include <toadlet/peeper/RenderCaps.h>
#include <toadlet/tadpole/Engine.h>
#include <toadlet/tadpole/material/Material.h>

namespace toadlet{
namespace tadpole{
namespace material{

Material::Material(MaterialManager *manager):BaseResource(),
	mSort(SortType_AUTO),
	mLayer(0)
{
	mManager=manager;
}

void Material::destroy(){
	tforeach(PathCollection::iterator,it,mPaths){
		RenderPath *path=*it;
		path->destroy();
	}
	mPaths.clear();

	BaseResource::destroy();
}

RenderPath::ptr Material::addPath(const String name){
	RenderPath::ptr path=new RenderPath(mManager);
	path->setName(name);
	mPaths.push_back(path);
	return path;
}

RenderPath::ptr Material::getPath(const String name) const{
	tforeach(PathCollection::const_iterator,it,mPaths){
		RenderPath *path=*it;
		if(path->getName()==name){
			return path;
		}
	}
	return NULL;
}

RenderPass::ptr Material::getPass(int pathIndex,int passIndex) const{
	RenderPath::ptr path;
	if(pathIndex==-1){
		if(mBestPath!=NULL){
			path=mBestPath;
		}
		else if(mPaths.size()>0){
			path=mPaths[0];
		}
		else{
			return NULL;
		}
	}
	else{
		if(pathIndex<mPaths.size()){
			path=mPaths[pathIndex];
		}
		else{
			return NULL;
		}
	}

	if(passIndex==-1){
		if(path->getPasses().size()>0){
			return path->getPasses()[0];
		}
		else{
			return path->addPass();
		}
	}
	else{
		if(passIndex<path->getPasses().size()){
			return path->getPasses()[passIndex];
		}
		else{
			return NULL;
		}
	}
}

RenderPass::ptr Material::getPass(int pathIndex,int passIndex){
	RenderPath::ptr path;
	if(pathIndex==-1){
		if(mBestPath!=NULL){
			path=mBestPath;
		}
		else if(mPaths.size()>0){
			path=mPaths[0];
		}
		else{
			path=addPath();
		}
	}
	else{
		if(pathIndex<mPaths.size()){
			path=mPaths[pathIndex];
		}
		else{
			return NULL;
		}
	}

	if(passIndex==-1){
		if(path->getPasses().size()>0){
			return path->getPasses()[0];
		}
		else{
			return path->addPass();
		}
	}
	else{
		if(passIndex<path->getPasses().size()){
			return path->getPasses()[passIndex];
		}
		else{
			return NULL;
		}
	}
}

bool Material::isDepthSorted() const{
	switch(mSort){
		case SortType_DEPTH:
			return true;
		case SortType_MATERIAL:
			return false;
		default:{
			return mBestPath!=NULL?mBestPath->isDepthSorted():false;
		}
	}
}

bool Material::compile(){
	return mManager!=NULL?mManager->compileMaterial(this):false;
}

}
}
}
