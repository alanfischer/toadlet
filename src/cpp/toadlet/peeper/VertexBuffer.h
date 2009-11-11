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

#ifndef TOADLET_PEEPER_VERTEXBUFFER_H
#define TOADLET_PEEPER_VERTEXBUFFER_H

#include <toadlet/egg/Resource.h>
#include <toadlet/peeper/Buffer.h>
#include <toadlet/peeper/VertexFormat.h>

namespace toadlet{
namespace peeper{

class TOADLET_API VertexBuffer:public egg::Resource,public Buffer{
public:
	TOADLET_SHARED_POINTERS(VertexBuffer);

	virtual ~VertexBuffer(){}

	virtual VertexBuffer *getRootVertexBuffer()=0;

	virtual bool create(int usageFlags,AccessType accessType,VertexFormat::ptr vertexFormat,int size)=0;
	virtual void destroy()=0;

	virtual bool createContext()=0;
	virtual void destroyContext(bool backData)=0;
	virtual bool contextNeedsReset()=0;

	virtual VertexFormat::ptr getVertexFormat()=0;
	virtual int getSize() const=0;
};

}
}

#endif
