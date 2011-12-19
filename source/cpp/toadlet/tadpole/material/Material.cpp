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

#include <toadlet/egg/Logger.h>
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

Material::~Material(){
	destroy();
}

void Material::destroy(){
	int i;
	for(i=0;i<mPaths.size();++i){
		mPaths[i]->destroy();
	}
	mPaths.clear();
}

RenderPath::ptr Material::addPath(){
	RenderPath::ptr path(new RenderPath(mManager));
	mPaths.add(path);
	return path;
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

RenderPath::ptr Material::findFixedPath(){
	int i,j,k;
	for(i=0;i<mPaths.size();++i){
		RenderPath::ptr path=mPaths[i];
		for(j=0;j<path->getNumPasses();++j){
			ShaderState *state=path->getPass(j)->getShaderState();
			for(k=0;k<Shader::ShaderType_MAX;++k){
				if(state->getShader((Shader::ShaderType)k)!=NULL){
					break;
				}
			}
			if(k<Shader::ShaderType_MAX){
				break;
			}
		}
		if(j==path->getNumPasses()){
			return path;
		}
	}
	return NULL;
}

bool Material::compile(){
	return mManager->compileMaterial(this);
}

}
}
}
