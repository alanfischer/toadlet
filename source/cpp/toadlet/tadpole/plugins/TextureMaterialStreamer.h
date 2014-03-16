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

#ifndef TOADLET_TADPOLE_TEXTUREMATERIALSTREAMER_H
#define TOADLET_TADPOLE_TEXTUREMATERIALSTREAMER_H

#include <toadlet/tadpole/BaseResourceStreamer.h>
#include <toadlet/tadpole/TextureManager.h>
#include <toadlet/tadpole/Engine.h>

namespace toadlet{
namespace tadpole{

class TOADLET_API TextureMaterialStreamer:public Object,public ResourceStreamer{
public:
	TOADLET_IOBJECT(TextureMaterialStreamer);

	TextureMaterialStreamer(Engine *engine){mEngine=engine;}

	bool load(Stream::ptr stream,ResourceData *data,ResourceRequest *request);
	bool save(Stream::ptr stream,Resource::ptr resource,ResourceData *data,ResourceRequest *request){return false;}

protected:
	class TextureMaterialRequest:public Object,public ResourceRequest{
	public:
		TOADLET_IOBJECT(TextureMaterialRequest);

		TextureMaterialRequest(Engine *engine,ResourceRequest *request):mEngine(engine),mRequest(request){}

		void resourceReady(Resource *resource);
		void resourceException(const Exception &ex){mRequest->resourceException(ex);}
		void resourceProgress(float progress){mRequest->resourceProgress(progress);}

	protected:
		Engine *mEngine;
		ResourceRequest::ptr mRequest;
	};

	Engine *mEngine;
};

}
}

#endif
