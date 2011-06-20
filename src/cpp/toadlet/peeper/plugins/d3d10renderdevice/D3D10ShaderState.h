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

#ifndef TOADLET_PEEPER_D3D10SHADERSTATE_H
#define TOADLET_PEEPER_D3D10SHADERSTATE_H

#include "D3D10Includes.h"
#include <toadlet/peeper/ShaderState.h>

namespace toadlet{
namespace peeper{

class D3D10RenderDevice;
class D3D10VertexFormat;
class D3D10VertexLayout;

class TOADLET_API D3D10ShaderState:public ShaderState{
public:
	D3D10ShaderState(D3D10RenderDevice *renderDevice);
	virtual ~D3D10ShaderState();

	ShaderState *getRootShaderState(){return this;}

	void setShaderStateDestroyedListener(ShaderStateDestroyedListener *listener){mListener=listener;}

	bool create(){return true;}
	void destroy();

	void setShader(Shader::ShaderType type,Shader::ptr shader);
	Shader::ptr getShader(Shader::ShaderType type);

	bool activate();

protected:
	D3D10RenderDevice *mDevice;
	ID3D10Device *mD3DDevice;

	ShaderStateDestroyedListener *mListener;
	egg::Collection<Shader::ptr> mShaders;

	friend class D3D10RenderDevice;
};

}
}

#endif
