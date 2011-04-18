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
#include <toadlet/tadpole/handler/XMSHHandler.h>
#include <toadlet/tadpole/handler/XMLMeshUtilities.h>

using namespace toadlet::egg;
using namespace toadlet::egg::io;

namespace toadlet{
namespace tadpole{
namespace handler{

XMSHHandler::XMSHHandler(Engine *engine){
	mEngine=engine;
}

Resource::ptr XMSHHandler::load(Stream::ptr stream,const ResourceHandlerData *handlerData){
	Mesh::ptr mesh;

	/// @todo: Replace the following when mxml implements custom load/save callbacks
	char buffer[1025];
	int amount=0;
	String string;
	while((amount=stream->read((tbyte*)buffer,1024))>0){
		buffer[amount]=0;
		string+=buffer;
	}

	mxml_node_t *root=mxmlLoadString(NULL,string,MXML_OPAQUE_CALLBACK);

	if(root==NULL){
		return NULL;
	}

	if(strcmp(mxmlGetElementName(root),"XMSH")==0){
		int version=0;
		const char *versionProp=mxmlElementGetAttr(root,"Version");
		if(versionProp!=NULL){
			version=XMLMeshUtilities::parseInt(versionProp);
		}

		if(version==1){
			mesh=loadMeshVersion1(root);
		}
		else if(version>=2){
			mesh=loadMeshVersion2Up(root,version);
		}
		else{
			mxmlRelease(root);
			Error::unknown(Categories::TOADLET_TADPOLE,
				"XMSHHandler: Invalid version");
			return NULL;
		}
	}

	mxmlRelease(root);

	if(mesh->skeleton!=NULL){
		// Clean up skeleton
		int i,j;
		for(i=0;i<mesh->skeleton->bones.size();++i){
			if(mesh->skeleton->bones[i]->parentIndex>i) break;
		}
		if(i<mesh->skeleton->bones.size()){
			// Reorder Bones
			Skeleton::ptr unordered=mesh->skeleton;
			Skeleton::ptr ordered(new Skeleton());
			// Pull all root bones
			for(i=0;i<unordered->bones.size();++i){
				if(unordered->bones[i]->parentIndex==-1){
					ordered->bones.add(unordered->bones[i]);
					unordered->bones.removeAt(i);
					i--;
				}
			}
			// Pull all bones that depend on a bone in the ordered bones
			for(j=0;j<ordered->bones.size();++j){
				Skeleton::Bone::ptr bone=ordered->bones[j];
				for(i=0;i<unordered->bones.size();++i){
					if(unordered->bones[i]->parentIndex==bone->index){
						ordered->bones.add(unordered->bones[i]);
						unordered->bones.removeAt(i);
						i--;
					}
				}
			}

			// Reassign the vertexBoneAssignments
			Collection<int> oldIndexes(ordered->bones.size(),-1);
			for(i=0;i<ordered->bones.size();++i){
				oldIndexes[ordered->bones[i]->index]=i;
			}
			for(i=0;i<mesh->vertexBoneAssignments.size();++i){
				Mesh::VertexBoneAssignmentList &vbalist=mesh->vertexBoneAssignments[i];
				for(j=0;j<vbalist.size();++j){
					vbalist[j].bone=oldIndexes[vbalist[j].bone];
				}
			}

			// Reindex the ordered bones
			Collection<int> newParentIndexes(ordered->bones.size(),-1);
			for(j=0;j<ordered->bones.size();++j){
				int oldIndex=ordered->bones[j]->index;
				ordered->bones[j]->index=j;
				for(i=0;i<ordered->bones.size();++i){
					if(ordered->bones[i]->parentIndex==oldIndex){
						newParentIndexes[i]=j;
					}
				}
			}
			for(j=0;j<ordered->bones.size();++j){
				ordered->bones[j]->parentIndex=newParentIndexes[j];
			}

			ordered->compile();

			mesh->skeleton=ordered;
		}
	}

	return mesh;
}

bool XMSHHandler::save(Mesh::ptr mesh,Stream::ptr stream,ProgressListener *listener){
	mxml_node_t *root=mxmlNewElement(MXML_NO_PARENT,"XMSH");

	int version=XMLMeshUtilities::version;
	mxmlElementSetAttr(root,"Version",XMLMeshUtilities::makeInt(version));

	if(version==1){
		saveMeshVersion1(root,mesh,listener);
	}
	else if(version>=2){
		saveMeshVersion2Up(root,mesh,version,listener);
	}
	else{
		mxmlRelease(root);
		Error::unknown(Categories::TOADLET_TADPOLE,
			"XMSHHandler: Invalid version");
		return false;
	}

	char *string=mxmlSaveAllocString(root,XMLMeshUtilities::mxmlSaveCallback);
	stream->write((tbyte*)string,strlen(string));
	free(string);

	mxmlRelease(root);

	return true;
}

Mesh::ptr XMSHHandler::loadMeshVersion1(mxml_node_t *root){
	Mesh::ptr mesh=NULL;

	mxml_node_t *block=root->child;
	while((block=block->next)!=NULL){
		if(strcmp(mxmlGetElementName(block),"MeshData")==0){
			mesh=XMLMeshUtilities::loadMesh(block,1,mEngine!=NULL?mEngine->getBufferManager():NULL,mEngine!=NULL?mEngine->getMaterialManager():NULL,mEngine!=NULL?mEngine->getTextureManager():NULL);
		}
		else if(strcmp(mxmlGetElementName(block),"SkeletonData")==0){
			mesh->skeleton=Skeleton::ptr(XMLMeshUtilities::loadSkeleton(block,1));
		}
		else if(strcmp(mxmlGetElementName(block),"AnimationData")==0){
			mesh->skeleton->sequences.add(TransformSequence::ptr(XMLMeshUtilities::loadSequence(block,1)));
		}
	}

	return mesh;
}

Mesh::ptr XMSHHandler::loadMeshVersion2Up(mxml_node_t *root,int version){
	Mesh::ptr mesh=NULL;

	mxml_node_t *block=root->child;
	while((block=block->next)!=NULL){
		if(strcmp(mxmlGetElementName(block),"Mesh")==0){
			mesh=XMLMeshUtilities::loadMesh(block,version,mEngine!=NULL?mEngine->getBufferManager():NULL,mEngine!=NULL?mEngine->getMaterialManager():NULL,mEngine!=NULL?mEngine->getTextureManager():NULL);
		}
		else if(strcmp(mxmlGetElementName(block),"Skeleton")==0){
			mesh->skeleton=Skeleton::ptr(XMLMeshUtilities::loadSkeleton(block,version));
		}
		else if(strcmp(mxmlGetElementName(block),"Sequence")==0){
			mesh->skeleton->sequences.add(TransformSequence::ptr(XMLMeshUtilities::loadSequence(block,version)));
		}
	}

	return mesh;
}

bool XMSHHandler::saveMeshVersion1(mxml_node_t *root,Mesh::ptr mesh,ProgressListener *listener){
	if(mesh!=NULL){
		mxml_node_t *node=XMLMeshUtilities::saveMesh(mesh,1,listener);
		mxmlSetElement(node,"MeshData");
		mxmlAddChild(root,node);
	}
	if(mesh!=NULL && mesh->skeleton!=NULL){
		mxml_node_t *node=XMLMeshUtilities::saveSkeleton(mesh->skeleton,1,listener);
		mxmlSetElement(node,"SkeletonData");
		mxmlAddChild(root,node);

		int i;
		for(i=0;i<mesh->skeleton->sequences.size();++i){
			TransformSequence::ptr sequence=mesh->skeleton->sequences[i];
			mxml_node_t *node=XMLMeshUtilities::saveSequence(sequence,1,listener);
			mxmlSetElement(node,"AnimationData");
			mxmlAddChild(root,node);
		}
	}

	return true;
}

bool XMSHHandler::saveMeshVersion2Up(mxml_node_t *root,Mesh::ptr mesh,int version,ProgressListener *listener){
	if(mesh!=NULL){
		mxml_node_t *node=XMLMeshUtilities::saveMesh(mesh,version,listener);
		mxmlSetElement(node,"Mesh");
		mxmlAddChild(root,node);
	}
	if(mesh!=NULL && mesh->skeleton!=NULL){
		mxml_node_t *node=XMLMeshUtilities::saveSkeleton(mesh->skeleton,version,listener);
		mxmlSetElement(node,"Skeleton");
		mxmlAddChild(root,node);

		int i;
		for(i=0;i<mesh->skeleton->sequences.size();++i){
			TransformSequence::ptr sequence=mesh->skeleton->sequences[i];
			mxml_node_t *node=XMLMeshUtilities::saveSequence(sequence,version,listener);
			mxmlSetElement(node,"Sequence");
			mxmlAddChild(root,node);
		}
	}

	return true;
}

}
}
}
