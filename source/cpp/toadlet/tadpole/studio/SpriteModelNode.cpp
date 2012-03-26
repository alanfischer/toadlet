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

#include <toadlet/peeper/VertexFormat.h>
#include <toadlet/tadpole/Engine.h>
#include <toadlet/tadpole/RenderableSet.h>
#include <toadlet/tadpole/Scene.h>
#include <toadlet/tadpole/studio/SpriteModelNode.h>
#include <toadlet/tadpole/studio/SpriteStreamer.h>

namespace toadlet{
namespace tadpole{
namespace studio{

TOADLET_NODE_IMPLEMENT(SpriteModelNode,Categories::TOADLET_TADPOLE_STUDIO+".SpriteModelNode");

SpriteModelNode::SpriteModelNode():super(),
	TOADLET_GIB_IMPLEMENT()

	//mModel,
	mRendered(false),
	mSequenceTime(0)
	//mMaterials,
	//mVertexData,
	//mIndexData,
	//mSharedRenderState
{
}

Node *SpriteModelNode::create(Scene *scene){
	super::create(scene);

	mRendered=true;
	mIndexData=IndexData::ptr(new IndexData(IndexData::Primitive_TRISTRIP,NULL,0,4));

	return this;
}

void SpriteModelNode::destroy(){
	mMaterials.clear();

	if(mSharedRenderState!=NULL){
		mSharedRenderState->destroy();
		mSharedRenderState=NULL;
	}

	if(mVertexData!=NULL){
		mVertexData->destroy();
		mVertexData=NULL;
	}
	
	if(mIndexData!=NULL){
		mIndexData->destroy();
		mIndexData=NULL;
	}

	mModel=NULL;

	super::destroy();
}

Node *SpriteModelNode::set(Node *node){
	super::set(node);

	SpriteModelNode *modelNode=(SpriteModelNode*)node;
	setModel(modelNode->getModel());

	return this;
}

void *SpriteModelNode::hasInterface(int type){
	switch(type){
		case InterfaceType_RENDERABLE:
			return (Renderable*)this;
		case InterfaceType_VISIBLE:
			return (Visible*)this;
		default:
			return NULL;
	}
}

void SpriteModelNode::setModel(const String &name){
	Stream::ptr stream=mEngine->openStream(name);
	if(stream==NULL){
		Error::unknown("Unable to find model");
		return;
	}

	SpriteStreamer::ptr streamer=new SpriteStreamer(mEngine);
	SpriteModel::ptr model=shared_static_cast<SpriteModel>(streamer->load(stream,NULL,NULL));
	model->setName(name);
	setModel(model);
}

void SpriteModelNode::setModel(SpriteModel::ptr model){
	mModel=model;

	if(mModel==NULL){
		return;
	}

	mBound.set(mModel->header->boundingradius);

	mMaterials.clear();

	int i;
	for(i=0;i<mModel->materials.size();++i){
		mMaterials.add(mModel->materials[i]);
		if(mSharedRenderState!=NULL){
			mMaterials[i]=mEngine->getMaterialManager()->createSharedMaterial(mMaterials[i],mSharedRenderState);
		}
	}

	if(mVertexData!=NULL){
		mVertexData->destroy();
	}

	VertexBuffer::ptr vertexBuffer=mEngine->getBufferManager()->createVertexBuffer(Buffer::Usage_BIT_STATIC,Buffer::Access_BIT_WRITE,mEngine->getVertexFormats().POSITION_TEX_COORD,4);
	mVertexData=VertexData::ptr(new VertexData(vertexBuffer));
	{
		VertexBufferAccessor vba(vertexBuffer,Buffer::Access_BIT_WRITE);

		float width=mModel->header->width,height=mModel->header->height;

		vba.set3(0,0, -width/2,-height/2,0);
		vba.set2(0,1, 0,0);

		vba.set3(1,0, width/2,-height/2,0);
		vba.set2(1,1, Math::ONE,0);

		vba.set3(2,0, -width/2,height/2,0);
		vba.set2(2,1, 0,Math::ONE);

		vba.set3(3,0, width/2,height/2,0);
		vba.set2(3,1, Math::ONE,Math::ONE);

		vba.unlock();
	}
}

RenderState::ptr SpriteModelNode::getSharedRenderState(){
	if(mSharedRenderState==NULL){
		mSharedRenderState=mEngine->getMaterialManager()->createRenderState();
		int i;
		for(i=0;i<mMaterials.size();++i){
			mMaterials[i]=mEngine->getMaterialManager()->createSharedMaterial(mMaterials[i],mSharedRenderState);
		}
	}

	return mSharedRenderState;
}

void SpriteModelNode::gatherRenderables(CameraNode *camera,RenderableSet *set){
	super::gatherRenderables(camera,set);

	if(mModel==NULL || mRendered==false){
		return;
	}

#if defined(TOADLET_GCC_INHERITANCE_BUG)
	set->queueRenderable(&renderable);
#else
	set->queueRenderable(this);
#endif
}

void SpriteModelNode::render(SceneRenderer *renderer) const{
	renderer->getDevice()->renderPrimitive(mVertexData,mIndexData);
}

}
}
}
