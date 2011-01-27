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

#ifndef TOADLET_TADPOLE_HANDLER_XANMHANDLER_H
#define TOADLET_TADPOLE_HANDLER_XANMHANDLER_H

#include <toadlet/tadpole/ResourceHandler.h>
#include <toadlet/tadpole/TransformSequence.h>

typedef struct mxml_node_s mxml_node_t;

namespace toadlet{
namespace tadpole{
namespace handler{

class TOADLET_API XANMHandler:public ResourceHandler{
public:
	TOADLET_SHARED_POINTERS(XANMHandler);

	XANMHandler();

	egg::Resource::ptr load(egg::io::Stream::ptr stream,const ResourceHandlerData *handlerData);
	bool save(TransformSequence::ptr resource,egg::io::Stream::ptr stream);

protected:
	TransformSequence::ptr loadSequenceVersion1(mxml_node_t *root);
	TransformSequence::ptr loadSequenceVersion2Up(mxml_node_t *root,int version);
	bool saveSequenceVersion1(mxml_node_t *root,TransformSequence::ptr sequence);
	bool saveSequenceVersion2Up(mxml_node_t *root,TransformSequence::ptr sequence,int version);
};

}
}
}

#endif
