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

#ifndef TOADLET_TADPOLE_XMLMESHUTILITIES_H
#define TOADLET_TADPOLE_XMLMESHUTILITIES_H

#include <toadlet/tadpole/BufferManager.h>
#include <toadlet/tadpole/MaterialManager.h>
#include <toadlet/tadpole/TextureManager.h>
#include <toadlet/tadpole/Mesh.h>
#include "toadlet_mxml.h"

namespace toadlet{
namespace tadpole{

class TOADLET_API XMLMeshUtilities{
public:
	XMLMeshUtilities();

	static const int version=3;

	static Material::ptr loadMaterial(mxml_node_t *node,int version,Engine *engine);
	static mxml_node_t *saveMaterial(Material::ptr material,int version);

	static Mesh::ptr loadMesh(mxml_node_t *node,int version,Engine *engine);
	static mxml_node_t *saveMesh(Mesh::ptr mesh,int version);

	static Skeleton::ptr loadSkeleton(mxml_node_t *node,int version);
	static mxml_node_t *saveSkeleton(Skeleton::ptr skeleton,int version);

	static Sequence::ptr loadSequence(mxml_node_t *node,int version,Engine *engine);
	static mxml_node_t *saveSequence(Sequence::ptr sequence,int version);

	class MaterialRequest:public Object,public ResourceRequest{
	public:
		TOADLET_IOBJECT(MaterialRequest);

		MaterialRequest(Engine *engine,Mesh::ptr mesh,ResourceRequest *request):mEngine(engine),mMesh(mesh),mRequest(request),mIndex(0){}

		void request();

		void resourceReady(Resource *resource);
		void resourceException(const Exception &ex);
		void resourceProgress(float progress){}

	protected:
		Engine *mEngine;
		MaterialManager *mMaterialManager;
		Mesh::ptr mMesh;
		ResourceRequest::ptr mRequest;
		int mIndex;
	};
};

}
}

#endif
