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
#include <toadlet/tadpole/handler/XANMHandler.h>
#include <toadlet/tadpole/handler/XMLMeshUtilities.h>

using namespace toadlet::egg;
using namespace toadlet::egg::io;
using namespace toadlet::tadpole::mesh;

namespace toadlet{
namespace tadpole{
namespace handler{

XANMHandler::XANMHandler(){
}

Resource::ptr XANMHandler::load(Stream::ptr stream,const ResourceHandlerData *handlerData){
	Sequence::ptr sequence=NULL;

	// TODO: Replace the following when mxml implements custom load/save callbacks
	char buffer[1025];
	int amount=0;
	String string;
	while((amount=stream->read((byte*)buffer,1024))>0){
		buffer[amount]=0;
		string+=buffer;
	}

	mxml_node_t *root=mxmlLoadString(NULL,string,MXML_OPAQUE_CALLBACK);

	if(strcmp(mxmlGetElementName(root),"XANM")==0){
		int version=0;
		const char *versionProp=mxmlElementGetAttr(root,"Version");
		if(versionProp!=NULL){
			version=XMLMeshUtilities::parseInt(versionProp);
		}

		if(version==1){
			sequence=loadSequenceVersion1(root);
		}
		else if(version>=2){
			sequence=loadSequenceVersion2Up(root,version);
		}
		else{
			mxmlRelease(root);
			Error::unknown(Categories::TOADLET_TADPOLE,
				"XANMHandler: Invalid version");
			return NULL;
		}
	}

	mxmlRelease(root);

	return sequence;
}

bool XANMHandler::save(Sequence::ptr sequence,Stream::ptr stream){
	mxml_node_t *root=mxmlNewElement(MXML_NO_PARENT,"XANM");

	int version=XMLMeshUtilities::version;
	mxmlElementSetAttr(root,"Version",XMLMeshUtilities::makeInt(version));

	if(version==1){
		saveSequenceVersion1(root,sequence);
	}
	else if(version>=2){
		saveSequenceVersion2Up(root,sequence,version);
	}
	else{
		mxmlRelease(root);
		Error::unknown(Categories::TOADLET_TADPOLE,
			"XANMHandler: Invalid version");
		return false;
	}

	char *string=mxmlSaveAllocString(root,XMLMeshUtilities::mxmlSaveCallback);
	stream->write((byte*)string,strlen(string));
	free(string);

	mxmlRelease(root);

	return true;
}

Sequence::ptr XANMHandler::loadSequenceVersion1(mxml_node_t *root){
	Sequence::ptr sequence=NULL;

	mxml_node_t *block=root->child;
	while((block=block->next)!=NULL){
		if(strcmp(mxmlGetElementName(block),"AnimationData")==0){
			sequence=XMLMeshUtilities::loadSequence(block,1);
		}
	}

	return sequence;
}

Sequence::ptr XANMHandler::loadSequenceVersion2Up(mxml_node_t *root,int version){
	Sequence::ptr sequence=NULL;

	mxml_node_t *block=root->child;
	while((block=block->next)!=NULL){
		if(strcmp(mxmlGetElementName(block),"Sequence")==0){
			sequence=XMLMeshUtilities::loadSequence(block,version);
		}
	}

	return sequence;
}

bool XANMHandler::saveSequenceVersion1(mxml_node_t *root,Sequence::ptr sequence){
	if(sequence!=NULL){
		mxml_node_t *node=XMLMeshUtilities::saveSequence(sequence,1);
		mxmlSetElement(node,"AnimationData");
		mxmlAddChild(root,node);
	}

	return true;
}

bool XANMHandler::saveSequenceVersion2Up(mxml_node_t *root,Sequence::ptr sequence,int version){
	if(sequence!=NULL){
		mxml_node_t *node=XMLMeshUtilities::saveSequence(sequence,version);
		mxmlSetElement(node,"Sequence");
		mxmlAddChild(root,node);
	}

	return true;
}

}
}
}
