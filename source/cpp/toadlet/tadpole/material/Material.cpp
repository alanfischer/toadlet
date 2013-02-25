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

#include <toadlet/egg/Log.h>
#include <toadlet/peeper/BackableRenderState.h>
#include <toadlet/peeper/BackableShaderState.h>
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
	int i;

	for(i=0;i<mPaths.size();++i){
		mPaths[i]->destroy();
	}
	mPaths.clear();

	BaseResource::destroy();
}

RenderPath::ptr Material::addPath(const String name){
	RenderPath::ptr path(new RenderPath(mManager));
	path->setName(name);
	mPaths.add(path);
	return path;
}

RenderPath::ptr Material::getPath(const String name) const{
	int i;
	for(i=0;i<mPaths.size();++i){
		RenderPath::ptr path=mPaths[i];
		if(path->getName()==name){
			return path;
		}
	}
	return NULL;
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
		if(path->getNumPasses()>0){
			return path->getPass(0);
		}
		else{
			return path->addPass();
		}
	}
	else{
		if(passIndex<path->getNumPasses()){
			return path->getPass(passIndex);
		}
		else{
			return NULL;
		}
	}
}

void Material::setModelMatrixFlags(int flags){
	int i,j;
	for(i=0;i<mPaths.size();++i){
		RenderPath *path=mPaths[i];
		for(j=0;j<path->getNumPasses();++j){
			path->getPass(j)->setModelMatrixFlags(flags);
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
