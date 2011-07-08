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

#ifndef TOADLET_TADPOLE_MATERIAL_RENDERVARIABLESET_H
#define TOADLET_TADPOLE_MATERIAL_RENDERVARIABLESET_H

#include <toadlet/peeper/VariableBuffer.h>
#include <toadlet/peeper/Shader.h>
#include <toadlet/tadpole/material/RenderVariable.h>

namespace toadlet{
namespace tadpole{
namespace material{

class TOADLET_API RenderVariableSet{
public:
	TOADLET_SHARED_POINTERS(RenderVariableSet);

	RenderVariableSet();
	virtual ~RenderVariableSet();
	
	void destroy();

	bool addBuffer(Shader::ShaderType shaderType,int index,VariableBuffer::ptr buffer);
	void removeBuffer(VariableBuffer::ptr buffer);
	inline int getNumBuffers() const{return mBuffers.size();}
	inline VariableBuffer::ptr getBuffer(int i){return mBuffers[i].buffer;}
	inline int getBufferScope(int i){return mBuffers[i].scope;}
	inline Shader::ShaderType getBufferShaderType(int i){return mBuffers[i].shaderType;}
	inline int getBufferIndex(int i){return mBuffers[i].index;}
	
	bool addVariable(const String &name,RenderVariable::ptr variable,int scope);
	void removeVariable(RenderVariable::ptr variable);

	void update(int scope,SceneParameters *parameters);

protected:
	class VariableInfo{
	public:
		String name;
		int location;
		int scope;
		RenderVariable::ptr variable;
	};

	class BufferInfo{
	public:
		VariableBuffer::ptr buffer;
		int scope;
		Shader::ShaderType shaderType;
		int index;
		Collection<VariableInfo> variables;
	};

	Collection<BufferInfo> mBuffers;
};

}
}
}

#endif
