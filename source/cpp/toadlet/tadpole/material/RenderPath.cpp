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

#include <toadlet/tadpole/material/RenderPath.h>

namespace toadlet{
namespace tadpole{
namespace material{

RenderPath::RenderPath(MaterialManager *manager):
	mManager(NULL)
	//mPasses
{
	mManager=manager;
}

void RenderPath::destroy(){
	int i;
	for(i=0;i<mPasses.size();++i){
		mPasses[i]->destroy();
	}
	mPasses.clear();
}

RenderPass *RenderPath::addPass(RenderState *renderState,ShaderState *shaderState){
	RenderPass::ptr pass=new RenderPass(mManager,renderState,shaderState);
	mPasses.add(pass);
	return pass;
}

RenderPass *RenderPath::addPass(RenderPass *pass){
	mPasses.add(pass);
	return pass;
}

bool RenderPath::isDepthSorted() const{
	if(mPasses.size()>0 && mPasses[0]->isDepthSorted()){
		return true;
	}
	return false;
}

RenderPass *RenderPath::findTexture(Shader::ShaderType &type,int &index,const String &name){
	int i;
	for(i=0;i<mPasses.size();++i){
		if(mPasses[i]->findTexture(type,index,name)){
			return mPasses[i];
		}
	}
	return NULL;
}

void RenderPath::compile(){
	int i;
	for(i=0;i<mPasses.size();++i){
		mPasses[i]->compile();
	}
}

}
}
}
