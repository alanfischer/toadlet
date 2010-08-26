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

void TerrainNode::setBaseName(const String &name){
	mBaseName=name;

	BMPHandler handler;

	TerrainPatchNode::ptr patches[3][3];
	int i,j;
	for(i=0;i<3;++i){
		for(j=0;j<3;++j){
			patches[i][j]=mEngine->createNodeType(TerrainPatchNode::type(),mScene);
			FileStream stream(name+":"+i+":"+j+".bmp",FileStream::Open_READ_BINARY);
			Image::ptr image(handler.loadImage(&stream));
			attach(patches[i][j]);
		}
	}

	patches[0][0]->stitchTop(patches[0][1]);
	patches[0][1]->stitchTop(patches[0][2]);
	patches[1][0]->stitchTop(patches[1][1]);
	patches[1][1]->stitchTop(patches[1][2]);
	patches[2][0]->stitchTop(patches[2][1]);
	patches[2][1]->stitchTop(patches[2][2]);

	patches[0][0]->stitchRight(patches[1][0]);
	patches[1][0]->stitchRight(patches[2][0]);
	patches[0][1]->stitchRight(patches[1][1]);
	patches[1][1]->stitchRight(patches[2][1]);
	patches[0][2]->stitchRight(patches[1][2]);
	patches[1][2]->stitchRight(patches[2][2]);
}

}
}
}
