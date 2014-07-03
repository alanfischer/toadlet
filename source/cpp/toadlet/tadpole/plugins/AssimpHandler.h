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

#ifndef TOADLET_TADPOLE_ASSIMPHANDLER_H
#define TOADLET_TADPOLE_ASSIMPHANDLER_H

#include <toadlet/egg/io/Stream.h>
#include <toadlet/tadpole/Node.h>
#include <assimp/scene.h>

namespace toadlet{
namespace tadpole{

class TOADLET_API AssimpHandler:public Object{
public:
	TOADLET_OBJECT(AssimpHandler);

	AssimpHandler(Engine *engine);
	
	Node::ptr loadScene(Stream *stream,const String &format);
	bool saveScene(Stream *stream,Node *node,const String &format);

protected:
	Node::ptr loadScene(const aiScene *scene,const aiNode *node);
	const aiScene *saveScene(Node *node);

	Engine *mEngine;
};

}
}

#endif
