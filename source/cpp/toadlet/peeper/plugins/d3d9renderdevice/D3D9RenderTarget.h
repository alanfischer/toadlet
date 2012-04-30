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

#ifndef TOADLET_PEEPER_D3D9RENDERTARGET_H
#define TOADLET_PEEPER_D3D9RENDERTARGET_H

#include "D3D9Includes.h"
#include <toadlet/peeper/RenderTarget.h>

namespace toadlet{
namespace peeper{

class TOADLET_API D3D9RenderTarget:public RenderTarget{
public:
	TOADLET_IPTR(D3D9RenderTarget);

	virtual ~D3D9RenderTarget(){}

	virtual void swap()=0;
	virtual void reset()=0;
	virtual bool activate()=0;
	virtual bool deactivate()=0;

	virtual IDirect3DDevice9 *getDirect3DDevice9() const=0;
	virtual IDirect3D9 *getDirect3D9() const=0;
};

}
}

#endif