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

#include <toadlet/peeper/Program.h>
#include <toadlet/egg/Logger.h>

namespace toadlet{
namespace peeper{

Program::Program(){
	mProgramPeer=NULL;
	mOwnsProgramPeer=false;
}

Program::Program(Renderer *renderer){
	mProgramPeer=renderer->createProgramPeer(this);
	mOwnsProgramPeer=true;
}

Program::~Program(){
	internal_setProgramPeer(NULL,false);
}

void Program::addShader(Shader::ptr shader){
	mShaders.add(shader);
}

const Shader::ptr &Program::getShader(int i) const{
	return mShaders[i];
}

int Program::getNumShaders() const{
	return mShaders.size();
}

void Program::internal_setProgramPeer(ProgramPeer *programPeer,bool own){
	if(mOwnsProgramPeer && mProgramPeer!=NULL){
		delete mProgramPeer;
	}
	mProgramPeer=programPeer;
	mOwnsProgramPeer=own;
}

}
}
