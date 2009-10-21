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

#include <toadlet/peeper/VertexData.h>

namespace toadlet{
namespace peeper{

VertexData::VertexData(int numVertexBuffers)
	//mVertexFormat,
	//mVertexBuffers
{
	mVertexBuffers.reserve(numVertexBuffers);
}

VertexData::VertexData(const VertexBuffer::ptr &vertexBuffer)
	//mVertexFormat,
	//mVertexBuffers
{
	addVertexBuffer(vertexBuffer);
}

void VertexData::addVertexBuffer(const VertexBuffer::ptr &vertexBuffer){
	if(mVertexBuffers.size()==0){
		mVertexFormat=vertexBuffer->getVertexFormat();
	}
	else{
		if(mVertexBuffers.size()==1){
			mVertexFormat=VertexFormat::ptr(new VertexFormat());
		}

		VertexFormat::ptr vertexFormat=vertexBuffer->getVertexFormat();

		int i;
		for(i=0;i<vertexFormat->getNumVertexElements();++i){
			mVertexFormat->addVertexElement(vertexFormat->getVertexElement(i));
		}
	}

	mVertexBuffers.add(vertexBuffer);
}

}
}
