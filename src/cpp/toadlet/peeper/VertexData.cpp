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

#include <toadlet/egg/Error.h>
#include <toadlet/peeper/VertexData.h>

using namespace toadlet::egg;

namespace toadlet{
namespace peeper{

VertexData::VertexData(VertexFormat::ptr vertexFormat)
	//vertexFormat,
	//vertexBuffers
{
	vertexFormat=vertexFormat;
}

VertexData::VertexData(VertexBuffer::ptr vertexBuffer)
	//vertexFormat,
	//vertexBuffers
{
	vertexFormat=vertexBuffer->getVertexFormat();
	addVertexBuffer(vertexBuffer);
}

VertexData::~VertexData(){
	destroy();
}

void VertexData::destroy(){
	int i;
	for(i=0;i<vertexBuffers.size();++i){
		vertexBuffers[i]->destroy();
	}
	vertexBuffers.clear();
}

bool VertexData::addVertexBuffer(const VertexBuffer::ptr &vertexBuffer){
	if(vertexBuffers.size()>0 && vertexFormat==vertexBuffers[0]->getVertexFormat()){
		Error::unknown(Categories::TOADLET_PEEPER,"cannot add multiple buffers to a VertexData with an unspecified VertexFormat");
		return false;
	}

	vertexBuffers.add(vertexBuffer);

	return true;
}

}
}
