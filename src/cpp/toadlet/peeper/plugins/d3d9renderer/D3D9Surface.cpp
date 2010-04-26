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

#include "D3D9Surface.h"

namespace toadlet{
namespace peeper{

D3D9Surface::D3D9Surface(IDirect3DSurface9 *surface):
	mSurface(NULL)
{
	mSurface=surface;

	D3DSURFACE_DESC desc;
	mSurface->GetDesc(&desc);
	mWidth=desc.Width;
	mHeight=desc.Height;
}

D3D9Surface::~D3D9Surface(){
	destroy();
}

bool D3D9Surface::destroy(){
	if(mSurface!=NULL){
		mSurface->Release();
		mSurface=NULL;
	}

	return true;
}

uint8 *D3D9Surface::lock(int lockAccess){
	D3DLOCKED_RECT d3drect={0};

	RECT rect={0};
	rect.top=0;
	rect.left=0;
	rect.bottom=mHeight;
	rect.right=mWidth;

	DWORD d3dflags=0;
	if(lockAccess==Access_BIT_READ){
		d3dflags|=D3DLOCK_READONLY;
	}

	HRESULT result=mSurface->LockRect(&d3drect,&rect,d3dflags);
	TOADLET_CHECK_D3D9ERROR(result,"D3D9Surface: LockRect");

	return (uint8*)d3drect.pBits;
}

bool D3D9Surface::unlock(){
	HRESULT result=mSurface->UnlockRect();
	TOADLET_CHECK_D3D9ERROR(result,"D3D9Surface: UnlockRect");
	return SUCCEEDED(result);
}

}
}
