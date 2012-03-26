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
#include "XANMStreamer.h"
#include "XMLMeshUtilities.h"

namespace toadlet{
namespace tadpole{

XANMStreamer::XANMStreamer(){
}

Resource::ptr XANMStreamer::load(Stream::ptr stream,ResourceData *data,ProgressListener *listener){
	TransformSequence::ptr sequence=NULL;

	/// @todo: Replace the following when mxml implements custom load/save callbacks
	char buffer[1025];
	int amount=0;
	String string;
	while((amount=stream->read((tbyte*)buffer,1024))>0){
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
				"XANMStreamer: Invalid version");
			return NULL;
		}
	}

	mxmlRelease(root);

	return sequence;
}

bool XANMStreamer::save(Stream::ptr stream,Resource::ptr resource,ResourceData *data,ProgressListener *listener){
	TransformSequence::ptr sequence=shared_static_cast<TransformSequence>(resource);
	if(sequence==NULL){
		return false;
	}

	mxml_node_t *root=mxmlNewElement(MXML_NO_PARENT,"XANM");

	int version=XMLMeshUtilities::version;
	mxmlElementSetAttr(root,"Version",XMLMeshUtilities::makeInt(version));

	if(version==1){
		saveSequenceVersion1(root,sequence,listener);
	}
	else if(version>=2){
		saveSequenceVersion2Up(root,sequence,version,listener);
	}
	else{
		mxmlRelease(root);
		Error::unknown(Categories::TOADLET_TADPOLE,
			"XANMStreamer: Invalid version");
		return false;
	}

	char *string=mxmlSaveAllocString(root,XMLMeshUtilities::mxmlSaveCallback);
	stream->write((tbyte*)string,strlen(string));
	free(string);

	mxmlRelease(root);

	return true;
}

TransformSequence::ptr XANMStreamer::loadSequenceVersion1(mxml_node_t *root){
	TransformSequence::ptr sequence=NULL;

	mxml_node_t *block=root->child;
	while((block=block->next)!=NULL){
		if(strcmp(mxmlGetElementName(block),"AnimationData")==0){
			sequence=XMLMeshUtilities::loadSequence(block,1);
		}
	}

	return sequence;
}

TransformSequence::ptr XANMStreamer::loadSequenceVersion2Up(mxml_node_t *root,int version){
	TransformSequence::ptr sequence=NULL;

	mxml_node_t *block=root->child;
	while((block=block->next)!=NULL){
		if(strcmp(mxmlGetElementName(block),"Sequence")==0){
			sequence=XMLMeshUtilities::loadSequence(block,version);
		}
	}

	return sequence;
}

bool XANMStreamer::saveSequenceVersion1(mxml_node_t *root,TransformSequence::ptr sequence,ProgressListener *listener){
	if(sequence!=NULL){
		mxml_node_t *node=XMLMeshUtilities::saveSequence(sequence,1,listener);
		mxmlSetElement(node,"AnimationData");
		mxmlAddChild(root,node);
	}

	return true;
}

bool XANMStreamer::saveSequenceVersion2Up(mxml_node_t *root,TransformSequence::ptr sequence,int version,ProgressListener *listener){
	if(sequence!=NULL){
		mxml_node_t *node=XMLMeshUtilities::saveSequence(sequence,version,listener);
		mxmlSetElement(node,"Sequence");
		mxmlAddChild(root,node);
	}

	return true;
}

}
}
