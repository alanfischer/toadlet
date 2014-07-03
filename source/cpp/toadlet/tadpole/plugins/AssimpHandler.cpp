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

#include "AssimpHandler.h"
#include <assimp/Importer.hpp>
#include <assimp/Exporter.hpp>

namespace toadlet{
namespace tadpole{

AssimpHandler::AssimpHandler(Engine *engine):
	mEngine(engine)
{}

Node::ptr AssimpHandler::loadScene(Stream *stream,const String &format){
	int length=stream->length();
	tbyte *data=new tbyte[length];
	stream->read(data,length);

	Assimp::Importer *importer=new Assimp::Importer();
	
	const aiScene *scene=importer->ReadFileFromMemory(data,length,0,format);
	
	Node::ptr node=loadScene(scene);
	
	delete importer;
	
	delete[] data;
	
	return node;
}

Node::ptr AssimpHandler::loadScene(const aiScene *iscene,const aiNode *inode){
	Node::ptr node;
	if(inode==NULL){
		node=loadScene(iscene,iscene->mRootNode);
	}
	else{
		node=new Node();
		int i;
		for(i=0;i<inode->mNumChildren;++i){
			node->attach(loadScene(iscene,inode->mChildren[i]));
		}
	}
	return node;
}

bool AssimpHandler::saveScene(Stream *stream,Node *node,const String &format){
	const aiScene *scene=saveScene(node);

	Assimp::Exporter *exporter=new Assimp::Exporter();
	
	const aiExportDataBlob *blob=exporter->ExportToBlob(scene,format);

	while(blob!=NULL){
		stream->write((tbyte*)blob->data,blob->size);
		blob=blob->next;
	}
	
	delete exporter;
	
	delete scene;

	return true;
}

const aiScene *AssimpHandler::saveScene(Node *node){
	return new aiScene();
}

}
}
