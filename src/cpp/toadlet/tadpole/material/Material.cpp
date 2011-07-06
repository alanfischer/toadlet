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

Material::Material(MaterialManager *manager,Material *source,bool clone):BaseResource(),
	mSort(SortType_MATERIAL),
	mLayer(0)
{
	mManager=manager;

	if(source!=NULL){
		int i;
		for(i=0;i<source->getNumPaths();++i){
			RenderPath::ptr path(new RenderPath(manager,source->getPath(i),clone));
			mPaths.add(path);
		}
	}
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
/// @todo
//			DepthState depth;
//			return mRenderState->getDepthState(depth) && depth.write==false;
return false;
		}
	}
}

void Material::shareStates(Material *material){
	int i;
	for(i=0;i<mPaths.size() && i<material->getNumPaths();++i){
		mPaths[i]->shareStates(material->getPath(i));
	}
}

/// @todo: Change the RenderCaps into an interface where we can query for the items, would let us move the TextureSupport and ProfileSupport into it
bool Material::compile(){
	RenderDevice *device=mManager->getEngine()->getRenderDevice();
	RenderCaps caps;
	device->getRenderCaps(caps);
	int i,j;
	for(i=0;i<getNumPaths();++i){
		RenderPath::ptr path=getPath(i);
		for(j=0;j<path->getNumPasses();++j){
			RenderPass *pass=path->getPass(j);
			ShaderState *state=pass->getShaderState();

			/// @todo: Use a new RenderCaps interface to check all types of shader programs
			if(state->getShader(Shader::ShaderType_VERTEX)!=NULL){
				if(caps.vertexShaders==false){
					break;
				}
			}
			else{
				if(caps.vertexFixedFunction==false){
					break;
				}
			}

			if(state->getShader(Shader::ShaderType_FRAGMENT)!=NULL){
				if(caps.fragmentShaders==false){
					break;
				}
			}
			else{
				if(caps.fragmentFixedFunction==false){
					break;
				}
			}
		}
		if(j==path->getNumPasses()){
			mBestPath=path;
			return true;
		}
	}

	mBestPath=NULL;
	return false;
}

}
}
}
