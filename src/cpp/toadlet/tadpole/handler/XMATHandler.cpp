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

#include <toadlet/egg/Error.h>
#include <toadlet/egg/Logger.h>
#include <toadlet/tadpole/handler/XMATHandler.h>
#include <toadlet/tadpole/handler/XMLMeshUtilities.h>

using namespace toadlet::egg;
using namespace toadlet::egg::io;

namespace toadlet{
namespace tadpole{
namespace handler{

XMATHandler::XMATHandler(Engine *engine){
	mEngine=engine;
}

Resource::ptr XMATHandler::load(Stream::ptr stream,const ResourceHandlerData *handlerData){
	Material::ptr material=NULL;

	/// @todo: Replace the following when mxml implements custom load/save callbacks
	char buffer[1025];
	int amount=0;
	String string;
	while((amount=stream->read((tbyte*)buffer,1024))>0){
		buffer[amount]=0;
		string+=buffer;
	}

	mxml_node_t *root=mxmlLoadString(NULL,string,MXML_OPAQUE_CALLBACK);

	if(strcmp(mxmlGetElementName(root),"XMAT")==0){
		int version=0;
		const char *versionProp=mxmlElementGetAttr(root,"Version");
		if(versionProp!=NULL){
			version=XMLMeshUtilities::parseInt(versionProp);
		}

		if(version==3){
			material=loadMaterial(root,version);
		}
		else{
			mxmlRelease(root);
			Error::unknown(Categories::TOADLET_TADPOLE,
				"XMATHandler: Invalid version");
			return NULL;
		}
	}

	mxmlRelease(root);

	return material;
}

bool XMATHandler::save(Material::ptr material,Stream::ptr stream,ProgressListener *listener){
	mxml_node_t *root=mxmlNewElement(MXML_NO_PARENT,"XMAT");

	int version=XMLMeshUtilities::version;
	mxmlElementSetAttr(root,"Version",XMLMeshUtilities::makeInt(version));

	if(version==3){
		saveMaterial(root,material,version,listener);
	}
	else{
		mxmlRelease(root);
		Error::unknown(Categories::TOADLET_TADPOLE,
			"XMATHandler: Invalid version");
		return false;
	}

	char *string=mxmlSaveAllocString(root,XMLMeshUtilities::mxmlSaveCallback);
	stream->write((tbyte*)string,strlen(string));
	free(string);

	mxmlRelease(root);

	return true;
}

Material::ptr XMATHandler::loadMaterial(mxml_node_t *root,int version){
	Material::ptr material=NULL;

	mxml_node_t *block=root->child;
	while((block=block->next)!=NULL){
		if(strcmp(mxmlGetElementName(block),"Material")==0){
			material=XMLMeshUtilities::loadMaterial(block,version,mEngine!=NULL?mEngine->getMaterialManager():NULL,mEngine!=NULL?mEngine->getTextureManager():NULL);
		}
	}

	return material;
}

bool XMATHandler::saveMaterial(mxml_node_t *root,Material::ptr material,int version,ProgressListener *listener){
	if(material!=NULL){
		mxml_node_t *node=XMLMeshUtilities::saveMaterial(material,version,listener);
		mxmlSetElement(node,"Material");
		mxmlAddChild(root,node);
	}

	return true;
}

}
}
}
