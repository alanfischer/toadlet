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

#ifndef TOADLET_TADPOLE_STUDIO_STUDIOHANDLER_H
#define TOADLET_TADPOLE_STUDIO_STUDIOHANDLER_H

#include <toadlet/tadpole/Engine.h>
#include <toadlet/tadpole/ResourceStreamer.h>
#include <toadlet/tadpole/studio/StudioModel.h>

namespace toadlet{
namespace tadpole{
namespace studio{

class TOADLET_API StudioHandler:public ResourceStreamer{
public:
	TOADLET_SHARED_POINTERS(StudioHandler);

	StudioHandler(Engine *engine);
	virtual ~StudioHandler();

	virtual Resource::ptr load(Stream::ptr stream,ResourceData *data,ProgressListener *listener);

protected:
	void buildBuffers(StudioModel *model);
	void buildTextures(StudioModel *model);
	void buildMaterials(StudioModel *model);
	Texture::ptr createTexture(studiotexture *ptexture,tbyte *data,tbyte *pal);

	Engine *mEngine;
};

}
}
}

#endif
