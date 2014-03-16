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

#include <toadlet/egg/MathFormatting.h>
#include <toadlet/egg/io/DataStream.h>
#include "XMATStreamer.h"
#include "XMLMeshUtilities.h"

using namespace toadlet::egg::MathFormatting;

namespace toadlet{
namespace tadpole{

XMATStreamer::XMATStreamer(Engine *engine){
	mEngine=engine;
}

Resource::ptr XMATStreamer::load(Stream::ptr stream,ResourceData *data){
	Material::ptr material=NULL;

	DataStream::ptr dataStream=new DataStream(stream);
	String string=dataStream->readAllString();
	mxml_node_t *root=mxmlLoadString(NULL,string,MXML_OPAQUE_CALLBACK);

	if(strcmp(mxmlGetElementName(root),"XMAT")==0){
		int version=0;
		const char *versionProp=mxmlElementGetAttr(root,"Version");
		if(versionProp!=NULL){
			version=parseInt(versionProp);
		}

		if(version==3){
			material=loadMaterial(root,version);
		}
		else{
			mxmlRelease(root);
			Error::unknown(Categories::TOADLET_TADPOLE,
				"XMATStreamer: Invalid version");
			return NULL;
		}
	}

	mxmlRelease(root);

	return material;
}

bool XMATStreamer::save(Stream::ptr stream,Resource::ptr resource,ResourceData *data){
	Material::ptr material=shared_static_cast<Material>(resource);
	if(material==NULL){
		return false;
	}

	mxml_node_t *root=mxmlNewElement(MXML_NO_PARENT,"XMAT");

	int version=XMLMeshUtilities::version;
	mxmlElementSetAttr(root,"Version",formatInt(version));

	if(version==3){
		saveMaterial(root,material,version);
	}
	else{
		mxmlRelease(root);
		Error::unknown(Categories::TOADLET_TADPOLE,
			"XMATStreamer: Invalid version");
		return false;
	}

	char *string=mxmlSaveAllocString(root,mxmlSaveCallback);
	stream->write((tbyte*)string,strlen(string));
	free(string);

	mxmlRelease(root);

	return true;
}

Material::ptr XMATStreamer::loadMaterial(mxml_node_t *root,int version){
	Material::ptr material=NULL;

	mxml_node_t *block=root->child;
	while((block=block->next)!=NULL){
		if(strcmp(mxmlGetElementName(block),"Material")==0){
			material=XMLMeshUtilities::loadMaterial(block,version,mEngine->getMaterialManager());
		}
	}

	return material;
}

bool XMATStreamer::saveMaterial(mxml_node_t *root,Material::ptr material,int version){
	if(material!=NULL){
		mxml_node_t *node=XMLMeshUtilities::saveMaterial(material,version);
		mxmlSetElement(node,"Material");
		mxmlAddChild(root,node);
	}

	return true;
}

}
}
