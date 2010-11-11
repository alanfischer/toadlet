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
#include <toadlet/tadpole/RenderQueue.h>
#include <toadlet/tadpole/Scene.h>
#include <toadlet/tadpole/studio/StudioModelNode.h>
#include <toadlet/tadpole/studio/StudioHandler.h>

using namespace toadlet::egg;
using namespace toadlet::peeper;
using namespace toadlet::tadpole::node;

namespace toadlet{
namespace tadpole{
namespace studio{

TOADLET_NODE_IMPLEMENT(StudioModelNode,Categories::TOADLET_TADPOLE_STUDIO+".StudioModelNode");

StudioModelNode::SubModel::SubModel(StudioModelNode *modelNode,int bodypartIndex,int modelIndex,int meshIndex){
	this->modelNode=modelNode;
	this->bodypartIndex=bodypartIndex;
	this->modelIndex=modelIndex;
	this->meshIndex=meshIndex;

	model=modelNode->getModel();
	sbodyparts=model->bodyparts(bodypartIndex);
	smodel=model->model(sbodyparts,modelIndex);
	smesh=model->mesh(smodel,meshIndex);
	mdata=model->findmeshdata(bodypartIndex,modelIndex,meshIndex);
	material=model->materials[smesh->skinref];
}

void StudioModelNode::SubModel::render(Renderer *renderer) const{
	int i;
	for(i=0;i<mdata->indexdatas.size();++i){
		renderer->renderPrimitive(model->vertexdata,mdata->indexdatas[i]);
	}
}

StudioModelNode::StudioModelNode(){
}

StudioModelNode::~StudioModelNode(){
}

void StudioModelNode::setModel(StudioModel::ptr model){
	mModel=model;

	mSubModels.clear();

	int i,j,k;
	for(i=0;i<=model->header->numbodyparts;++i){
		studiobodyparts *sbodyparts=model->bodyparts(i);
		for(j=0;j<sbodyparts->nummodels;++j){
			studiomodel *smodel=model->model(sbodyparts,j);
			for(k=0;k<smodel->nummesh;++k){
				SubModel::ptr subModel(new SubModel(this,i,j,k));
				mSubModels.add(subModel);
			}
		}
	}
}

void StudioModelNode::queueRenderables(CameraNode *camera,RenderQueue *queue){
	super::queueRenderables(camera,queue);

	int i;
	for(i=0;i<mSubModels.size();++i){
		queue->queueRenderable(mSubModels[i]);
	}
}

}
}
}
