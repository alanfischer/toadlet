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

#ifndef TOADLET_PEEPER_D3D10SHADER_H
#define TOADLET_PEEPER_D3D10SHADER_H

#include "D3D10Includes.h"
#include <toadlet/egg/BaseResource.h>
#include <toadlet/peeper/Shader.h>

namespace toadlet{
namespace peeper{

class D3D10RenderDevice;
class D3D10VertexFormat;

class TOADLET_API D3D10Shader:protected egg::BaseResource,public Shader{
	TOADLET_BASERESOURCE_PASSTHROUGH(Shader);
public:
	D3D10Shader(D3D10RenderDevice *renderDevice);

	virtual ~D3D10Shader();

	Shader *getRootShader(){return this;}

	bool create(ShaderType shaderType,const egg::String &profile,const egg::String &code);
	void destroy();

	ShaderType getShaderType() const{return mShaderType;}
	const egg::String &getProfile() const{return mProfile;}

	int findConstant(const egg::String &name) const{return -1;}

	ID3D10InputLayout *findInputLayout(D3D10VertexFormat *vertexFormat);

protected:
	bool createContext();
	bool destroyContext();

	D3D10RenderDevice *mDevice;
	ID3D10Device *mD3DDevice;

	ShaderType mShaderType;
	egg::String mProfile;
	egg::String mCode;

	ID3D10DeviceChild *mShader;
	ID3D10Blob *mBytecode,*mLog;
	
	egg::Collection<ID3D10InputLayout*> mLayouts;

	friend class D3D10RenderDevice;
};

}
}

#endif
