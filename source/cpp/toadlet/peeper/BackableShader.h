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

#ifndef TOADLET_PEEPER_BACKABLESHADER_H
#define TOADLET_PEEPER_BACKABLESHADER_H

#include <toadlet/egg/BaseResource.h>
#include <toadlet/peeper/Shader.h>
#include <toadlet/peeper/RenderDevice.h>

namespace toadlet{
namespace peeper{

class TOADLET_API BackableShader:public BaseResource,public Shader{
public:
	TOADLET_RESOURCE(BackableShader,Shader);

	BackableShader();
	virtual ~BackableShader();

	virtual Shader *getRootShader(){return mBack!=NULL?mBack->getRootShader():NULL;}

	virtual bool create(ShaderType shaderType,const String &profile,const String &code);
	virtual bool create(ShaderType shaderType,const String profiles[],const String codes[],int numCodes);
	virtual void destroy();
	
	virtual ShaderType getShaderType() const{return mShaderType;}
	virtual String getProfile() const{return mBack!=NULL?mBack->getProfile():(char*)NULL;}

	virtual void setBack(Shader::ptr back,RenderDevice *renderDevice);
	virtual Shader::ptr getBack(){return mBack;}

	static bool convertCreate(Shader::ptr shader,RenderDevice *renderDevice,Shader::ShaderType shaderType,const String profiles[],const String codes[],int numCodes);

protected:
	ShaderType mShaderType;
	Collection<String> mProfiles;
	Collection<String> mCodes;
	Shader::ptr mBack;
};

}
}

#endif
