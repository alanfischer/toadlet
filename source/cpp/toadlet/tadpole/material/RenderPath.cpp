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

RenderPath::~RenderPath(){
	destroy();
}

void RenderPath::destroy(){
	int i;
	for(i=0;i<mPasses.size();++i){
		mPasses[i]->destroy();
	}
	mPasses.clear();
}

RenderPass::ptr RenderPath::addPass(RenderState::ptr renderState,ShaderState::ptr shaderState){
	RenderPass::ptr pass(new RenderPass(mManager,renderState,shaderState));
	mPasses.add(pass);
	return pass;
}

RenderPass::ptr RenderPath::addPass(RenderPass::ptr pass){
	mPasses.add(pass);
	return pass;
}

bool RenderPath::isDepthSorted() const{
	if(mPasses.size()>0 && mPasses[0]->isDepthSorted()){
		return true;
	}
	return false;
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
