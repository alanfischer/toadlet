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
#include "XMSHStreamer.h"
#include "XMLMeshUtilities.h"

using namespace toadlet::egg::MathFormatting;

namespace toadlet{
namespace tadpole{

XMSHStreamer::XMSHStreamer(Engine *engine){
	mEngine=engine;
}

Resource::ptr XMSHStreamer::load(Stream::ptr stream,ResourceData *data,ProgressListener *listener){
	Mesh::ptr mesh;

	DataStream::ptr dataStream=new DataStream(stream);
	String string=dataStream->readAllString();
	mxml_node_t *root=mxmlLoadString(NULL,string,MXML_OPAQUE_CALLBACK);

	if(root==NULL){
		return NULL;
	}

	if(strcmp(mxmlGetElementName(root),"XMSH")==0){
		int version=0;
		const char *versionProp=mxmlElementGetAttr(root,"Version");
		if(versionProp!=NULL){
			version=parseInt(versionProp);
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
				"XMSHStreamer: Invalid version");
			return NULL;
		}
	}

	mxmlRelease(root);

	if(mesh->getSkeleton()!=NULL){
		Skeleton::ptr skeleton=mesh->getSkeleton();

		// Clean up skeleton
		int i,j;
		for(i=0;i<skeleton->getNumBones();++i){
			if(skeleton->getBone(i)->parentIndex>i) break;
		}
		if(i<skeleton->getNumBones()){
			// Reorder Bones
			Skeleton::ptr unordered=skeleton;
			Skeleton::ptr ordered=new Skeleton();
			// Pull all root bones
			for(i=0;i<unordered->getNumBones();++i){
				if(unordered->getBone(i)->parentIndex==-1){
					ordered->addBone(unordered->getBone(i));
					unordered->removeBone(i);
					i--;
				}
			}
			// Pull all bones that depend on a bone in the ordered bones
			for(j=0;j<ordered->getNumBones();++j){
				Skeleton::Bone::ptr bone=ordered->getBone(j);
				for(i=0;i<unordered->getNumBones();++i){
					if(unordered->getBone(i)->parentIndex==bone->index){
						ordered->addBone(unordered->getBone(i));
						unordered->removeBone(i);
						i--;
					}
				}
			}

			// Reassign the vertexBoneAssignments
			Collection<int> oldIndexes(ordered->getNumBones(),-1);
			for(i=0;i<ordered->getNumBones();++i){
				oldIndexes[ordered->getBone(i)->index]=i;
			}
			Collection<Mesh::VertexBoneAssignmentList> vbas=mesh->getVertexBoneAssignments();
			for(i=0;i<vbas.size();++i){
				Mesh::VertexBoneAssignmentList &vbalist=vbas[i];
				for(j=0;j<vbalist.size();++j){
					vbalist[j].bone=oldIndexes[vbalist[j].bone];
				}
			}
			mesh->setVertexBoneAssignments(vbas);

			// Reindex the ordered bones
			Collection<int> newParentIndexes(ordered->getNumBones(),-1);
			for(j=0;j<ordered->getNumBones();++j){
				int oldIndex=ordered->getBone(j)->index;
				ordered->getBone(j)->index=j;
				for(i=0;i<ordered->getNumBones();++i){
					if(ordered->getBone(i)->parentIndex==oldIndex){
						newParentIndexes[i]=j;
					}
				}
			}
			for(j=0;j<ordered->getNumBones();++j){
				ordered->getBone(j)->parentIndex=newParentIndexes[j];
			}

			ordered->compile();

			mesh->setSkeleton(ordered);
		}
	}

	mesh->compileBoneBounds();

	return mesh;
}

bool XMSHStreamer::save(Stream::ptr stream,Resource::ptr resource,ResourceData *data,ProgressListener *listener){
	Mesh::ptr mesh=shared_static_cast<Mesh>(resource);
	if(mesh==NULL){
		return false;
	}

	mxml_node_t *root=mxmlNewElement(MXML_NO_PARENT,"XMSH");

	int version=XMLMeshUtilities::version;
	mxmlElementSetAttr(root,"Version",formatInt(version));

	if(version==1){
		saveMeshVersion1(root,mesh,listener);
	}
	else if(version>=2){
		saveMeshVersion2Up(root,mesh,version,listener);
	}
	else{
		mxmlRelease(root);
		Error::unknown(Categories::TOADLET_TADPOLE,
			"XMSHStreamer: Invalid version");
		return false;
	}

	char *string=mxmlSaveAllocString(root,mxmlSaveCallback);
	stream->write((tbyte*)string,strlen(string));
	free(string);

	mxmlRelease(root);

	return true;
}

Mesh::ptr XMSHStreamer::loadMeshVersion1(mxml_node_t *root){
	Mesh::ptr mesh=NULL;

	mxml_node_t *block=root->child;
	while((block=block->next)!=NULL){
		if(strcmp(mxmlGetElementName(block),"MeshData")==0){
			mesh=XMLMeshUtilities::loadMesh(block,1,mEngine->getBufferManager(),mEngine->getMaterialManager(),mEngine->getTextureManager());
		}
		else if(strcmp(mxmlGetElementName(block),"SkeletonData")==0){
			mesh->setSkeleton(XMLMeshUtilities::loadSkeleton(block,1));
		}
		else if(strcmp(mxmlGetElementName(block),"AnimationData")==0){
			mesh->getSkeleton()->addSequence(XMLMeshUtilities::loadSequence(block,1,mEngine->getBufferManager()));
		}
	}

	return mesh;
}

Mesh::ptr XMSHStreamer::loadMeshVersion2Up(mxml_node_t *root,int version){
	Mesh::ptr mesh=NULL;

	mxml_node_t *block=root->child;
	while((block=block->next)!=NULL){
		if(strcmp(mxmlGetElementName(block),"Mesh")==0){
			mesh=XMLMeshUtilities::loadMesh(block,version,mEngine->getBufferManager(),mEngine->getMaterialManager(),mEngine->getTextureManager());
		}
		else if(strcmp(mxmlGetElementName(block),"Skeleton")==0){
			mesh->setSkeleton(XMLMeshUtilities::loadSkeleton(block,version));
		}
		else if(strcmp(mxmlGetElementName(block),"Sequence")==0){
			mesh->getSkeleton()->addSequence(XMLMeshUtilities::loadSequence(block,version,mEngine->getBufferManager()));
		}
	}

	return mesh;
}

bool XMSHStreamer::saveMeshVersion1(mxml_node_t *root,Mesh::ptr mesh,ProgressListener *listener){
	if(mesh!=NULL){
		mxml_node_t *node=XMLMeshUtilities::saveMesh(mesh,1,listener);
		mxmlSetElement(node,"MeshData");
		mxmlAddChild(root,node);
	}
	if(mesh!=NULL && mesh->getSkeleton()!=NULL){
		Skeleton::ptr skeleton=mesh->getSkeleton();
		mxml_node_t *node=XMLMeshUtilities::saveSkeleton(skeleton,1,listener);
		mxmlSetElement(node,"SkeletonData");
		mxmlAddChild(root,node);

		int i;
		for(i=0;i<skeleton->getNumSequences();++i){
			Sequence::ptr sequence=skeleton->getSequence(i);
			mxml_node_t *node=XMLMeshUtilities::saveSequence(sequence,1,listener);
			mxmlSetElement(node,"AnimationData");
			mxmlAddChild(root,node);
		}
	}

	return true;
}

bool XMSHStreamer::saveMeshVersion2Up(mxml_node_t *root,Mesh::ptr mesh,int version,ProgressListener *listener){
	if(mesh!=NULL){
		mxml_node_t *node=XMLMeshUtilities::saveMesh(mesh,version,listener);
		mxmlSetElement(node,"Mesh");
		mxmlAddChild(root,node);
	}
	if(mesh!=NULL && mesh->getSkeleton()!=NULL){
		Skeleton::ptr skeleton=mesh->getSkeleton();
		mxml_node_t *node=XMLMeshUtilities::saveSkeleton(skeleton,version,listener);
		mxmlSetElement(node,"Skeleton");
		mxmlAddChild(root,node);

		int i;
		for(i=0;i<skeleton->getNumSequences();++i){
			Sequence::ptr sequence=skeleton->getSequence(i);
			mxml_node_t *node=XMLMeshUtilities::saveSequence(sequence,version,listener);
			mxmlSetElement(node,"Sequence");
			mxmlAddChild(root,node);
		}
	}

	return true;
}

}
}
