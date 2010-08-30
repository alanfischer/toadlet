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

#include <toadlet/egg/image/BMPHandler.h>
#include <toadlet/egg/io/FileStream.h>
#include <toadlet/tadpole/Engine.h>
#include <toadlet/tadpole/Scene.h>
#include <toadlet/tadpole/Noise.h>
#include <toadlet/tadpole/terrain/TerrainNode.h>
#include <toadlet/tadpole/terrain/TerrainPatchNode.h>

using namespace toadlet::egg;
using namespace toadlet::egg::io;
using namespace toadlet::egg::image;
using namespace toadlet::peeper;
using namespace toadlet::tadpole::mesh;
using namespace toadlet::tadpole::node;
using namespace toadlet::tadpole::handler;

namespace toadlet{
namespace tadpole{
namespace terrain{

TOADLET_NODE_IMPLEMENT(TerrainNode,Categories::TOADLET_TADPOLE_TERRAIN+".TerrainNode");

TerrainNode::TerrainNode():super(){}

TerrainNode::~TerrainNode(){}

Node *TerrainNode::create(Scene *scene){
	super::create(scene);

	Noise noise(4,4,1,1,256);

	const int ps=64;
	float data[ps*ps];

	float scale=16;

	int tx=0,ty=0,px=0,py=0;
	for(ty=0;ty<ts;ty++){
		for(tx=0;tx<ts;tx++){
			TerrainPatchNode::ptr patch=mEngine->createNodeType(TerrainPatchNode::type(),mScene);

			for(px=0;px<ps;++px){
				for(py=0;py<ps;++py){
					int x=tx*ps+px;
					int width=ts*ps;
					int y=ty*ps+py;
					int height=ts*ps;
					float n=(noise.perlin2((float)x/(float)width,(float)y/(float)height)*0.5 + 0.5);
					data[py*ps+px]=-n*20;
				}
			}

			patch->setData(data,ps,ps,ps);
patch->setMaterial(mEngine->getMaterialManager()->findMaterial("grass.jpg"));
//patch->getRenderMaterial()->setFill(Renderer::Fill_LINE);
			patch->setTranslate(ps*tx*scale,ps*ty*scale,0);	
			patch->setScale(scale);
			attach(patch);
			patches[ty*ts+tx]=patch;

			if(tx>0){
				patches[ty*ts+(tx-1)]->stitchToRight(patches[ty*ts+tx]);
			}
			if(ty>0){
				patches[(ty-1)*ts+tx]->stitchToBottom(patches[ty*ts+tx]);
			}
		}
	}


	return this;
}

void TerrainNode::queueRenderables(CameraNode *camera,RenderQueue *queue){
	int tx=0,ty=0;
	for(ty=0;ty<ts;ty++){
		for(tx=0;tx<ts;tx++){
			patches[ty*ts+tx]->updateBlocks(camera);
		}
	}

	super::queueRenderables(camera,queue);
}

void TerrainNode::traceSegment(Collision &result,const Vector3 &position,const Segment &segment,const Vector3 &size){
	result.time=Math::ONE;

	int tx=0,ty=0;
	for(ty=0;ty<ts;ty++){
		for(tx=0;tx<ts;tx++){
			patches[ty*ts+tx]->traceSegment(result,patches[ty*ts+tx]->getWorldTranslate(),segment,size);
		}
	}
}

}
}
}
