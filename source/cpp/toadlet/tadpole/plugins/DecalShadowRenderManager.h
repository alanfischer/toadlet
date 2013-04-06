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

#ifndef TOADLET_TADPOLE_DECALSHADOWRENDERMANAGER_H
#define TOADLET_TADPOLE_DECALSHADOWRENDERMANAGER_H

#include "SimpleRenderManager.h"

namespace toadlet{
namespace tadpole{

class TOADLET_API DecalShadowRenderManager:public SimpleRenderManager{
public:
	TOADLET_OBJECT(DecalShadowRenderManager);

	DecalShadowRenderManager(Scene *scene);
	virtual ~DecalShadowRenderManager();

	void setShadowScope(int scope){mShadowScope=scope;}
	int getShadowScope() const{return mShadowScope;}

	void setTraceScope(int scope){mTraceScope=scope;}
	int getTraceScope() const{return mTraceScope;}

	void setTraceDistance(scalar distance){mTraceDistance=distance;}
	scalar getTraceDistance() const{return mTraceDistance;}

	void setOffset(scalar offset){mOffset=offset;}
	scalar getOffset() const{return mOffset;}

	void interRenderRenderables(RenderableSet *set,RenderDevice *device,Camera *camera,bool useMaterials);

	static tbyte *createPoint(TextureFormat *format);

protected:
	int mShadowScope;
	int mTraceScope;
	scalar mTraceDistance;
	scalar mOffset;
	Material::ptr mMaterial;
	VertexData::ptr mVertexData;
	IndexData::ptr mIndexData;
};

}
}

#endif
