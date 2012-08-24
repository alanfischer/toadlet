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
#include <mxml.h>

#define mxmlGetElementName(x) ((x->type==MXML_ELEMENT)?x->value.element.name:"")
#define mxmlFindChild(x,s) mxmlFindElement(x->child,x,s,NULL,NULL,MXML_NO_DESCEND)
#define mxmlGetOpaque(x) ((x!=NULL)?x->value.opaque:NULL)
#define mxmlAddChild(x,c) mxmlAdd(x,MXML_ADD_AFTER,MXML_ADD_TO_PARENT,c)

namespace toadlet{
namespace tadpole{

class TOADLET_API XMLMeshUtilities{
public:
	XMLMeshUtilities();

	static const int version=3;

	static const char *mxmlSaveCallback(mxml_node_t *node,int ws);

	static Material::ptr loadMaterial(mxml_node_t *node,int version,MaterialManager *materialManager,TextureManager *textureManager);
	static mxml_node_t *saveMaterial(Material::ptr material,int version,ProgressListener *listener);

	static Mesh::ptr loadMesh(mxml_node_t *node,int version,BufferManager *bufferManager,MaterialManager *materialManager,TextureManager *textureManager);
	static mxml_node_t *saveMesh(Mesh::ptr mesh,int version,ProgressListener *listener);

	static Skeleton::ptr loadSkeleton(mxml_node_t *node,int version);
	static mxml_node_t *saveSkeleton(Skeleton::ptr skeleton,int version,ProgressListener *listener);

	static Sequence::ptr loadSequence(mxml_node_t *node,int version,BufferManager *bufferManager);
	static mxml_node_t *saveSequence(Sequence::ptr sequence,int version,ProgressListener *listener);
};

}
}

#endif
