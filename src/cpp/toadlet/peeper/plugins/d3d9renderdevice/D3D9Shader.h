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

#ifndef TOADLET_PEEPER_D3D9SHADER_H
#define TOADLET_PEEPER_D3D9SHADER_H

#include "D3D9Includes.h"
#include <toadlet/egg/BaseResource.h>
#include <toadlet/peeper/Shader.h>

namespace toadlet{
namespace peeper{

class D3D9RenderDevice;

class TOADLET_API D3D9Shader:protected egg::BaseResource,public Shader{
	TOADLET_BASERESOURCE_PASSTHROUGH(Shader);
public:
	D3D9Shader(D3D9RenderDevice *renderDevice);

	virtual ~D3D9Shader();

	Shader *getRootShader(){return this;}

	bool create(ShaderType shaderType,const egg::String &profile,const egg::String &code);
	void destroy();

	ShaderType getShaderType() const{return mShaderType;}
	const egg::String &getProfile() const{return mProfile;}

	int getNumVariableBuffers(){return mVariableBufferFormats.size();}
	VariableBufferFormat::ptr getVariableBufferFormat(int i){return mVariableBufferFormats[i];}

protected:
	bool createContext();
	bool destroyContext();
	bool reflect();
	bool activate();

	D3D9RenderDevice *mDevice;
	IDirect3DDevice9 *mD3DDevice;

	ShaderType mShaderType;
	egg::String mProfile;
	egg::String mCode;

	IUnknown *mShader;
	ID3DXBuffer *mBytecode,*mLog;
	ID3DXConstantTable *mConstantTable;

	egg::Collection<VariableBufferFormat::ptr> mVariableBufferFormats;

	friend class D3D9RenderDevice;
	friend class D3D9ShaderState;
};

}
}

#endif
