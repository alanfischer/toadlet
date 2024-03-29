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

#ifndef TOADLET_PEEPER_D3D10RENDERTARGET_H
#define TOADLET_PEEPER_D3D10RENDERTARGET_H

#include "D3D10Includes.h"
#include <toadlet/egg/Collection.h>
#include <toadlet/peeper/RenderTarget.h>

namespace toadlet{
namespace peeper{

class TOADLET_API D3D10RenderTarget:public RenderTarget{
public:
	TOADLET_INTERFACE(D3D10RenderTarget);

	D3D10RenderTarget();
	virtual ~D3D10RenderTarget(){}

	virtual bool create();
	virtual void destroy();

	virtual void clear(int clearFlags,const Vector4 &clearColor);
	virtual void swap(){}
	virtual void reset(){}
	virtual bool activate();
	virtual bool deactivate();

	inline ID3D10Device *getD3D10Device() const{return mD3DDevice;}

protected:
	ID3D10Device *mD3DDevice;
	Collection<ID3D10RenderTargetView*> mRenderTargetViews;
	ID3D10DepthStencilView *mDepthStencilView;
};

}
}

#endif