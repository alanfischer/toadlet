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

#ifndef TOADLET_PEEPER_VARIABLEBUFFER_H
#define TOADLET_PEEPER_VARIABLEBUFFER_H

#include <toadlet/peeper/Buffer.h>
#include <toadlet/peeper/VariableBufferFormat.h>
#include <toadlet/peeper/Texture.h>
#include <toadlet/peeper/SamplerState.h>

namespace toadlet{
namespace peeper{

class TOADLET_API VariableBuffer:public Buffer{
public:
	TOADLET_INTERFACE(VariableBuffer);
	
	virtual ~VariableBuffer(){}

	virtual VariableBuffer *getRootVariableBuffer()=0;

	virtual bool create(int usage,int access,VariableBufferFormat::ptr format)=0;
	virtual void destroy()=0;

	virtual void resetCreate()=0;
	virtual void resetDestroy()=0;

	virtual VariableBufferFormat::ptr getVariableBufferFormat() const=0;
};

}
}

#endif
