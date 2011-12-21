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

#include <toadlet/tadpole/handler/XMLMeshUtilities.h>
#include <toadlet/tadpole/material/Material.h>
#include <toadlet/peeper/VertexBufferAccessor.h>
#include <toadlet/peeper/IndexBufferAccessor.h>
#include <toadlet/egg/MathConversion.h>
#include <toadlet/egg/Error.h>
#include <stdio.h>

using namespace toadlet::egg::MathConversion;

namespace toadlet{
namespace tadpole{
namespace handler{

const char LF[]={0xA,0x0};
const char CRLF[]={0xD,0xA,0x0};

const int MAX_TABS=10;
const char *TABS[MAX_TABS]={
	"",
	"\t",
	"\t\t",
	"\t\t\t",
	"\t\t\t\t",
	"\t\t\t\t\t",
	"\t\t\t\t\t\t",
	"\t\t\t\t\t\t\t",
	"\t\t\t\t\t\t\t\t",
	"\t\t\t\t\t\t\t\t\t",
};

// For version 2
const int MAX_MAP_NAMES=8;
const char *MAP_NAMES[MAX_MAP_NAMES]={
	"Diffuse",
	"Ambient",
	"Specular",
	"SelfIllumination",
	"Opacity",
	"Bump",
	"Reflection",
	"Refraction",
};

const char *XMLMeshUtilities::mxmlSaveCallback(mxml_node_t *node,int ws){
	bool oneLiner=false;

	if(node->child!=NULL && node->child==node->last_child && node->child->type!=MXML_ELEMENT){
		oneLiner=true;
	}

	if(ws==MXML_WS_BEFORE_OPEN || (oneLiner==false && ws==MXML_WS_BEFORE_CLOSE)){
		int count=0;
		while((node=node->parent)!=NULL) count++;
		if(count>MAX_TABS-1){count=MAX_TABS-1;}
		return TABS[count];
	}
	else if((oneLiner==false && ws==MXML_WS_AFTER_OPEN) || ws==MXML_WS_AFTER_CLOSE){
		return "\n";
	}
	return NULL;
}

bool parseBool(const String &string){
	if(string.find("true")>=0 || string.find("TRUE")>=0 || string.find("True")>=0){
		return true;
	}
	else{
		return false;
	}
}

const char *makeBool(bool b){
	if(b){
		return "true";
	}
	else{
		return "false";
	}
}

int XMLMeshUtilities::parseInt(const char *string){
	int i=0;
	sscanf(string,"%d",&i);
	return i;
}

String XMLMeshUtilities::makeInt(int i){
	return String()+i;
}

scalar parseScalar(const char *string){
	float f=0;
	sscanf(string,"%f",&f);
	return floatToScalar(f);
}

String makeScalar(scalar s){
	return String()+scalarToFloat(s);
}

Vector2 parseVector2(const char *string){
	float x=0,y=0;
	sscanf(string,"%f,%f",&x,&y);
	return Vector2(floatToScalar(x),floatToScalar(y));
}

String makeVector2(const Vector2 &v){
	return String()+scalarToFloat(v.x)+','+scalarToFloat(v.y);
}

const char *makeVector2(char *buffer,const Vector2 &v){
	sprintf(buffer,"%f,%f ",scalarToFloat(v.x),scalarToFloat(v.y));
	return buffer;
}

Vector3 parseVector3(const char *string){
	float x=0,y=0,z=0;
	sscanf(string,"%f,%f,%f",&x,&y,&z);
	return Vector3(floatToScalar(x),floatToScalar(y),floatToScalar(z));
}

String makeVector3(const Vector3 &v){
	return String()+scalarToFloat(v.x)+','+scalarToFloat(v.y)+','+scalarToFloat(v.z);
}

const char *makeVector3(char *buffer,const Vector3 &v){
	sprintf(buffer,"%f,%f,%f ",scalarToFloat(v.x),scalarToFloat(v.y),scalarToFloat(v.z));
	return buffer;
}

Vector4 parseVector4(const char *string){
	float x=0,y=0,z=0,w=0;
	sscanf(string,"%f,%f,%f,%f",&x,&y,&z,&w);
	return Vector4(floatToScalar(x),floatToScalar(y),floatToScalar(z),floatToScalar(w));
}

String makeVector4(const Vector4 &v){
	return String()+scalarToFloat(v.x)+','+scalarToFloat(v.y)+','+scalarToFloat(v.z)+','+scalarToFloat(v.w);
}

const char *makeVector4(char *buffer,const Vector4 &v){
	sprintf(buffer,"%f,%f,%f,%f ",scalarToFloat(v.x),scalarToFloat(v.y),scalarToFloat(v.z),scalarToFloat(v.w));
	return buffer;
}

Quaternion parseQuaternion(const char *string){
	float x=0,y=0,z=0,w=0;
	sscanf(string,"%f,%f,%f,%f",&x,&y,&z,&w);
	return Quaternion(floatToScalar(x),floatToScalar(y),floatToScalar(z),floatToScalar(w));
}

String makeQuaternion(const Quaternion &q){
	return String()+scalarToFloat(q.x)+','+scalarToFloat(q.y)+','+scalarToFloat(q.z)+','+scalarToFloat(q.w);
}

String parseString(const char *string){
	return String(string).trimLeft().trimRight();
}

Mesh::VertexBoneAssignmentList parseBoneAssignment(const String &string){
	int i=0;
	int c=0;
	Mesh::VertexBoneAssignment vba;
	Mesh::VertexBoneAssignmentList vbaList;
	scalar total=0;

	while(true){
		int comma=string.find(',',i+1);
		if(comma==String::npos){
			comma=string.length();
		}

		if(c%2==0){
			vba.bone=XMLMeshUtilities::parseInt(string.substr(i,comma-i));
		}
		else{
			vba.weight=parseScalar(string.substr(i,comma-i));
			total+=vba.weight;
			vbaList.add(vba);
		}

		i=comma+1;
		c++;
		if(comma>=string.length()){
			break;
		}
	}

	if(total>0){
		for(i=0;i<vbaList.size();++i){
			vbaList[i].weight=Math::div(vbaList[i].weight,total);
		}
	}

	return vbaList;
}

const char *makeBoneAssignment(char *buffer,const Mesh::VertexBoneAssignmentList &vbaList){
	int offset=0;

	int i;
	for(i=0;i<vbaList.size();++i){
		offset+=sprintf(buffer+offset,"%d,%f",vbaList[i].bone,scalarToFloat(vbaList[i].weight));
		if(i!=vbaList.size()-1){
			strcat(buffer,",");
			offset++;
		}
		else{
			strcat(buffer," ");
			offset++;
		}
	}

	return buffer;
}

/// @todo: Support loading all of the material, instead of starting with a DiffuseMaterial
Material::ptr XMLMeshUtilities::loadMaterial(mxml_node_t *node,int version,MaterialManager *materialManager,TextureManager *textureManager){
	const char *prop=mxmlElementGetAttr(node,"Name");

	return materialManager->findMaterial(prop);
}

mxml_node_t *XMLMeshUtilities::saveMaterial(Material::ptr material,int version,ProgressListener *listener){
	if(material==NULL){
		return NULL;
	}

	mxml_node_t *materialNode=mxmlNewElement(MXML_NO_PARENT,"Material");

	mxmlElementSetAttr(materialNode,"Name",material->getName());

	return materialNode;
}

Mesh::ptr XMLMeshUtilities::loadMesh(mxml_node_t *node,int version,BufferManager *bufferManager,MaterialManager *materialManager,TextureManager *textureManager){
	Mesh::ptr mesh(new Mesh());
	const char *prop=NULL;

	mxml_node_t *transformNode=mxmlFindChild(node,"Transform");
	if(transformNode!=NULL){
		Transform transform;
		mxml_node_t *translateNode=mxmlFindChild(transformNode,"Translate");
		if(translateNode!=NULL){
			transform.setTranslate(parseVector3(mxmlGetOpaque(translateNode->child)));
		}

		mxml_node_t *rotateNode=mxmlFindChild(transformNode,"Rotate");
		if(rotateNode!=NULL){
			transform.setRotate(parseQuaternion(mxmlGetOpaque(rotateNode->child)));
		}

		mxml_node_t *scaleNode=mxmlFindChild(transformNode,"Scale");
		if(scaleNode!=NULL){
			transform.setScale(parseVector3(mxmlGetOpaque(scaleNode->child)));
		}

		mesh->setTransform(transform);
	}

	mxml_node_t *vertexNode=mxmlFindChild(node,"Vertexes");
	{
		int count=0;
		VertexFormat::ptr vertexFormat;
		if(bufferManager!=NULL){
			vertexFormat=bufferManager->createVertexFormat();
		}
		else{
			vertexFormat=VertexFormat::ptr(new BackableVertexFormat());
		}
		Collection<Mesh::VertexBoneAssignmentList> vbas;

		prop=mxmlElementGetAttr(vertexNode,"Count");
		if(prop!=NULL){
			count=parseInt(prop);
		}

		prop=mxmlElementGetAttr(vertexNode,"Type");
		if(prop!=NULL){
			String s=prop;
			String t;

			int i=0;
			while(true){
				int i2=s.find(',',i);
				if(i2==String::npos){
					t=s.substr(i,s.length()-i);
				}
				else{
					t=s.substr(i,i2-i);
				}

				if(t=="Position"){
					vertexFormat->addElement(VertexFormat::Semantic_POSITION,0,VertexFormat::Format_TYPE_FLOAT_32|VertexFormat::Format_COUNT_3);
				}
				else if(t=="Normal"){
					vertexFormat->addElement(VertexFormat::Semantic_NORMAL,0,VertexFormat::Format_TYPE_FLOAT_32|VertexFormat::Format_COUNT_3);
				}
				else if(t=="TexCoord"){
					vertexFormat->addElement(VertexFormat::Semantic_TEXCOORD,0,VertexFormat::Format_TYPE_FLOAT_32|VertexFormat::Format_COUNT_2);
				}
				else if(t=="Color"){
					vertexFormat->addElement(VertexFormat::Semantic_COLOR,0,VertexFormat::Format_TYPE_COLOR_RGBA);
				}
				else if(t=="Bone"){
					vbas.resize(count);
				}
				else{
					Error::unknown(Categories::TOADLET_TADPOLE,
						"Invalid vertex element type:"+t);
				}

				if(i2==String::npos){
					break;
				}
				else{
					i=i2+1;
				}
			}
		}

		VertexBuffer::ptr vertexBuffer;
		if(bufferManager!=NULL){
			if(vbas.size()>0){
				vertexBuffer=bufferManager->createVertexBuffer(Buffer::Usage_BIT_STAGING,Buffer::Access_READ_WRITE,vertexFormat,count);
			}
			else{
				vertexBuffer=bufferManager->createVertexBuffer(Buffer::Usage_BIT_STATIC,Buffer::Access_BIT_WRITE,vertexFormat,count);
			}
		}
		else{
			vertexBuffer=VertexBuffer::ptr(new BackableBuffer());
			vertexBuffer->create(Buffer::Usage_BIT_STATIC,Buffer::Access_BIT_WRITE,vertexFormat,count);
		}

		int pi=vertexFormat->findElement(VertexFormat::Semantic_POSITION);
		int ni=vertexFormat->findElement(VertexFormat::Semantic_NORMAL);
		int ti=vertexFormat->findElement(VertexFormat::Semantic_TEXCOORD);
		int ci=vertexFormat->findElement(VertexFormat::Semantic_COLOR);

		AABox bound;

		VertexBufferAccessor vba;
		vba.lock(vertexBuffer,Buffer::Access_BIT_WRITE);

		const char *cdata=mxmlGetOpaque(vertexNode->child);
		if(cdata!=NULL){
			String data=cdata;

			int l=0;
			int i=0;
			while(true){
				int lf=data.find(LF,i+1);
				int crlf=data.find(CRLF,i+1);
				int end=data.length();
				int start=end+1;
				if(crlf!=String::npos){
					end=crlf;
					start=crlf+2;
				}
				else if(lf!=String::npos){
					end=lf;
					start=lf+1;
				}

				String line=data.substr(i,end-i);
				line=line.trimLeft().trimRight();

				// Skip parsing empty lines
				if(line.length()>0){
					int c=0;
					int j=0;
					while(true){
						if(line.length()==0){
							break;
						}

						int space=line.find(' ',j+1);
						if(space==String::npos){
							space=line.length();
						}

						String element=line.substr(j,space);
						int semantic=-1;
						if(c<vertexFormat->getNumElements()){
							semantic=vertexFormat->getSemantic(c);
						}
						if(semantic==VertexFormat::Semantic_POSITION){
							Vector3 position=parseVector3(element);
							bound.merge(position);
							vba.set3(l,pi,position);
						}
						else if(semantic==VertexFormat::Semantic_NORMAL){
							vba.set3(l,ni,parseVector3(element));
						}
						else if(semantic==VertexFormat::Semantic_TEXCOORD){
							vba.set2(l,ti,parseVector2(element));
						}
						else if(semantic==VertexFormat::Semantic_COLOR){
 							vba.setRGBA(l,ci,parseVector4(element).getRGBA());
						}
						else if(vbas.size()>0){
							vbas[l]=parseBoneAssignment(element);
						}

						j=space;
						c++;
						if(space>=line.length()){
							break;
						}
					}
					l++;
				}

				i=start;
				if(i>=data.length()){
					break;
				}
			}
		}

		vba.unlock();

		mesh->setVertexBoneAssignments(vbas);

		mesh->setBound(bound);
		mesh->setStaticVertexData(VertexData::ptr(new VertexData(vertexBuffer)));
	}

	mxml_node_t *subMeshNode=node->child;
	while((subMeshNode=subMeshNode->next)!=NULL){
		if(version<=2){
			if(strcmp(mxmlGetElementName(subMeshNode),"SubMeshData")!=0){
				continue;
			}
		}
		else{
			if(strcmp(mxmlGetElementName(subMeshNode),"SubMesh")!=0){
				continue;
			}
		}

		Mesh::SubMesh::ptr subMesh=Mesh::SubMesh::ptr(new Mesh::SubMesh());

		prop=mxmlElementGetAttr(subMeshNode,"Name");
		if(prop!=NULL){
			subMesh->name=prop;
		}

		mxml_node_t *indexesNode=mxmlFindChild(subMeshNode,"Indexes");
		{
			int count=0;

			prop=mxmlElementGetAttr(indexesNode,"Count");
			if(prop!=NULL){
				count=parseInt(prop);
			}

			IndexBuffer::ptr indexBuffer;
			if(bufferManager!=NULL){
				indexBuffer=bufferManager->createIndexBuffer(Buffer::Usage_BIT_STATIC,Buffer::Access_BIT_WRITE,IndexBuffer::IndexFormat_UINT16,count);
			}
			else{
				indexBuffer=IndexBuffer::ptr(new BackableBuffer());
				indexBuffer->create(Buffer::Usage_BIT_STATIC,Buffer::Access_BIT_WRITE,IndexBuffer::IndexFormat_UINT16,count);
			}

			subMesh->indexData=IndexData::ptr(new IndexData(IndexData::Primitive_TRIS,indexBuffer,0,count));

			IndexBufferAccessor iba;
			iba.lock(indexBuffer,Buffer::Access_BIT_WRITE);

			const char *cdata=mxmlGetOpaque(indexesNode->child);
			if(cdata!=NULL){
				String data=cdata;
				data=data.trimLeft().trimRight();

				int i=0;
				int c=0;
				while(true){
					int space=data.find(' ',i+1);
					if(space==String::npos){
						space=data.length();
					}

					iba.set(c,parseInt(data.substr(i,space-i)));

					i=space;
					c++;
					if(space>=data.length()){
						break;
					}
				}
			}

			iba.unlock();
		}

		mxml_node_t *materialNode=subMeshNode->child;
		while((materialNode=materialNode->next)!=NULL){
			if(strcmp(mxmlGetElementName(materialNode),"Material")!=0){
				continue;
			}

			Material::ptr material;
			String materialName;

			prop=mxmlElementGetAttr(materialNode,"File");
			if(prop!=NULL){
				subMesh->materialName=prop;
				if(materialManager!=NULL){
					material=materialManager->findMaterial(subMesh->materialName);
				}
			}
			else{
				material=Material::ptr(loadMaterial(materialNode,version,materialManager,textureManager));
			}

			subMesh->material=material;
			if(subMesh->material!=NULL){
				subMesh->material->retain();
			}
		}

		mesh->addSubMesh(subMesh);
	}

	mesh->compile();

	return mesh;
}

mxml_node_t *XMLMeshUtilities::saveMesh(Mesh::ptr mesh,int version,ProgressListener *listener){
	mxml_node_t *meshNode=mxmlNewElement(MXML_NO_PARENT,"Mesh");

	mxml_node_t *transformNode=mxmlNewElement(meshNode,"Transform");
	{
		const Transform &transform=mesh->getTransform();

		mxml_node_t *translateNode=mxmlNewElement(transformNode,"Translate");
		{
			mxmlNewOpaque(translateNode,makeVector3(transform.getTranslate()));
		}

		mxml_node_t *rotateNode=mxmlNewElement(transformNode,"Rotate");
		{
			mxmlNewOpaque(rotateNode,makeQuaternion(transform.getRotate()));
		}

		mxml_node_t *scaleNode=mxmlNewElement(transformNode,"Scale");
		{
			mxmlNewOpaque(scaleNode,makeVector3(transform.getScale()));
		}
	}

	const VertexData::ptr &vertexData=mesh->getStaticVertexData();
	if(vertexData==NULL){
		return meshNode;
	}

	mxml_node_t *vertexNode=mxmlNewElement(meshNode,"Vertexes");
	{
		const VertexBuffer::ptr &vertexBuffer=vertexData->getVertexBuffer(0);

		mxmlElementSetAttr(vertexNode,"Count",makeInt(vertexBuffer->getSize()));

		VertexFormat::ptr vertexFormat=vertexBuffer->getVertexFormat();
		String type;
		int pi=vertexFormat->findElement(VertexFormat::Semantic_POSITION);
		int ni=vertexFormat->findElement(VertexFormat::Semantic_NORMAL);
		int ti=vertexFormat->findElement(VertexFormat::Semantic_TEXCOORD);
		int ci=vertexFormat->findElement(VertexFormat::Semantic_COLOR);

		const Collection<Mesh::VertexBoneAssignmentList> &vbas=mesh->getVertexBoneAssignments();

		if(pi>=0){
			type+="Position,";
		}
		if(ni>=0){
			type+="Normal,";
		}
		if(ti>=0){
			type+="TexCoord,";
		}
		if(ci>=0){
			type+="Color,";
		}
		if(vbas.size()>0){
			type+="Bone,";
		}

		// Drop trailing ','
		if(type!=(char*)NULL){
			type=type.substr(0,type.length()-1);
		}
		mxmlElementSetAttr(vertexNode,"Type",type);

		VertexBufferAccessor vba;
		vba.lock(vertexBuffer,Buffer::Access_BIT_WRITE);

		Vector3 v3;
		Vector2 v2;
		char line[1024];
		char buffer[128];
		String data;
		int i;
		for(i=0;i<vertexBuffer->getSize();++i){
			if(listener!=NULL){
				listener->progressUpdated((float)i/(float)vertexBuffer->getSize());
			}

			line[0]=0;
			buffer[0]=0;

			if(i==0) strcat(line,"\n");
			strcat(line,"\t\t\t");

			if(pi>=0){
				vba.get3(i,pi,v3); strcat(line,makeVector3(buffer,v3));
			}
			if(ni>=0){
				vba.get3(i,ni,v3); strcat(line,makeVector3(buffer,v3));
			}
			if(ti>=0){
				vba.get2(i,ti,v2); strcat(line,makeVector2(buffer,v2));
			}
			if(ci>=0){
				strcat(line,makeVector4(buffer,Vector4(vba.getRGBA(i,ci))));
			}
			if(vbas.size()>0){
				strcat(line,makeBoneAssignment(buffer,vbas[i]));
			}

			strcat(line,LF);
			if(i==vertexBuffer->getSize()-1) strcat(line,"\t\t");

			data+=line;
		}

		vba.unlock();

		mxmlNewOpaque(vertexNode,data);
	}

	int i;
	for(i=0;i<mesh->getNumSubMeshes();++i){
		Mesh::SubMesh::ptr subMesh=mesh->getSubMesh(i);

		mxml_node_t *subMeshNode=mxmlNewElement(meshNode,"SubMesh");
		{
			if(subMesh->name!=(char*)NULL){
				mxmlElementSetAttr(subMeshNode,"Name",subMesh->name);
			}

			const IndexData::ptr &indexData=subMesh->indexData;
			const IndexBuffer::ptr &indexBuffer=indexData->getIndexBuffer();

			IndexBufferAccessor iba;
			iba.lock(indexBuffer,Buffer::Access_BIT_WRITE);

			mxml_node_t *indexNode=mxmlNewElement(subMeshNode,"Indexes");
			{
				mxmlElementSetAttr(indexNode,"Count",makeInt(indexBuffer->getSize()));

				String line;
				int j;
				for(j=0;j<indexBuffer->getSize();++j){
					line+=makeInt(iba.get(j));

					if(j<indexBuffer->getSize()-1){
						line+=" ";
					}
				}

				mxmlNewOpaque(indexNode,line);
			}

			mxml_node_t *materialNode=NULL;
			if(subMesh->materialName.length()==0){
				materialNode=saveMaterial(subMesh->material,version,listener);
			}
			else{
				materialNode=mxmlNewElement(MXML_NO_PARENT,"Material");
				mxmlElementSetAttr(materialNode,"File",subMesh->materialName);
			}
			if(materialNode!=NULL){
				mxmlAddChild(subMeshNode,materialNode);
			}

			iba.unlock();
		}
	}

	return meshNode;
}

Skeleton::ptr XMLMeshUtilities::loadSkeleton(mxml_node_t *node,int version){
	Skeleton::ptr skeleton(new Skeleton());

	const char *prop=NULL;
	mxml_node_t *boneNode=node->child;
	while((boneNode=boneNode->next)!=NULL){
		if(strcmp(mxmlGetElementName(boneNode),"Bone")!=0){
			continue;
		}

		Skeleton::Bone::ptr bone(new Skeleton::Bone());

		prop=mxmlElementGetAttr(boneNode,"Index");
		if(prop!=NULL){
			bone->index=parseInt(prop);
		}

		prop=mxmlElementGetAttr(boneNode,"Parent");
		if(prop!=NULL){
			bone->parentIndex=parseInt(prop);
		}

		prop=mxmlElementGetAttr(boneNode,"Name");
		if(prop!=NULL){
			bone->name=prop;
		}

		mxml_node_t *translateNode=mxmlFindChild(boneNode,"Translate");
		if(translateNode!=NULL){
			bone->translate=parseVector3(mxmlGetOpaque(translateNode->child));
		}

		mxml_node_t *rotateNode=mxmlFindChild(boneNode,"Rotate");
		if(rotateNode!=NULL){
			bone->rotate=parseQuaternion(mxmlGetOpaque(rotateNode->child));
		}

		mxml_node_t *scaleNode=mxmlFindChild(boneNode,"Scale");
		if(scaleNode!=NULL){
			bone->scale=parseVector3(mxmlGetOpaque(scaleNode->child));
		}

		if(skeleton->bones.size()<=bone->index){
			skeleton->bones.resize(bone->index+1);
		}
		skeleton->bones[bone->index]=bone;
	}

	skeleton->compile();

	return skeleton;
}

mxml_node_t *XMLMeshUtilities::saveSkeleton(Skeleton::ptr skeleton,int version,ProgressListener *listener){
	mxml_node_t *skeletonNode=mxmlNewElement(MXML_NO_PARENT,"Skeleton");

	int i;
	for(i=0;i<skeleton->bones.size();++i){
		if(listener!=NULL){
			listener->progressUpdated((float)i/(float)skeleton->bones.size());
		}
		Skeleton::Bone::ptr bone=skeleton->bones[i];

		mxml_node_t *boneNode=mxmlNewElement(skeletonNode,"Bone");
		{
			mxmlElementSetAttr(boneNode,"Index",makeInt(bone->index));

			mxmlElementSetAttr(boneNode,"Parent",makeInt(bone->parentIndex));

			mxmlElementSetAttr(boneNode,"Name",bone->name);

			mxml_node_t *translateNode=mxmlNewElement(boneNode,"Translate");
			{
				mxmlNewOpaque(translateNode,makeVector3(bone->translate));
			}

			mxml_node_t *rotateNode=mxmlNewElement(boneNode,"Rotate");
			{
				mxmlNewOpaque(rotateNode,makeQuaternion(bone->rotate));
			}

			mxml_node_t *scaleNode=mxmlNewElement(boneNode,"Scale");
			{
				mxmlNewOpaque(scaleNode,makeVector3(bone->scale));
			}
		}
	}

	return skeletonNode;
}

TransformSequence::ptr XMLMeshUtilities::loadSequence(mxml_node_t *node,int version){
	TransformSequence::ptr sequence(new TransformSequence());

	const char *prop=NULL;
	prop=mxmlElementGetAttr(node,"Name");
	if(prop!=NULL){
		sequence->setName(prop);
	}

	scalar sequenceLength=0;
	prop=mxmlElementGetAttr(node,"Length");
	if(prop!=NULL){
		sequenceLength=parseScalar(prop);
	}

	mxml_node_t *trackNode=node->child;
	while((trackNode=trackNode->next)!=NULL){
		if(strcmp(mxmlGetElementName(trackNode),"Track")!=0){
			continue;
		}

		TransformTrack::ptr track(new TransformTrack());
		sequence->tracks.add(track);

		if(version<=2){
			prop=mxmlElementGetAttr(trackNode,"Bone");
		}
		else{
			prop=mxmlElementGetAttr(trackNode,"Index");
		}
		if(prop!=NULL){
			track->index=parseInt(prop);
		}

		track->length=sequenceLength;

		mxml_node_t *keyFrameNode=trackNode->child;
		while((keyFrameNode=keyFrameNode->next)!=NULL){
			if(strcmp(mxmlGetElementName(keyFrameNode),"KeyFrame")!=0){
				continue;
			}

			TransformKeyFrame keyFrame;
			prop=mxmlElementGetAttr(keyFrameNode,"Time");
			if(prop!=NULL){
				keyFrame.time=parseScalar(prop);
			}

			mxml_node_t *translateNode=mxmlFindChild(keyFrameNode,"Translate");
			if(translateNode!=NULL){
				keyFrame.translate=parseVector3(mxmlGetOpaque(translateNode->child));
			}

			mxml_node_t *rotateNode=mxmlFindChild(keyFrameNode,"Rotate");
			if(rotateNode!=NULL){
				keyFrame.rotate=parseQuaternion(mxmlGetOpaque(rotateNode->child));
			}

			mxml_node_t *scaleNode=mxmlFindChild(keyFrameNode,"Scale");
			if(scaleNode!=NULL){
				keyFrame.scale=parseVector3(mxmlGetOpaque(scaleNode->child));
			}

			track->keyFrames.add(keyFrame);
		}
	}

	sequence->compile();

	return sequence;
}

mxml_node_t *XMLMeshUtilities::saveSequence(TransformSequence::ptr sequence,int version,ProgressListener *listener){
	mxml_node_t *sequenceNode=mxmlNewElement(MXML_NO_PARENT,"Sequence");

	if(sequence->getName()!=(char*)NULL){
		mxmlElementSetAttr(sequenceNode,"Name",sequence->getName());
	}

	mxmlElementSetAttr(sequenceNode,"Length",makeScalar(sequence->length));

	int j;
	for(j=0;j<sequence->tracks.size();++j){
		if(listener!=NULL){
			listener->progressUpdated((float)j/(float)sequence->tracks.size());
		}

		TransformTrack::ptr track=sequence->tracks[j];

		mxml_node_t *trackNode=mxmlNewElement(sequenceNode,"Track");

		mxmlElementSetAttr(trackNode,"Bone",makeInt(track->index));

		int k;
		for(k=0;k<track->keyFrames.size();++k){
			const TransformKeyFrame keyFrame=track->keyFrames[k];

			mxml_node_t *keyFrameNode=mxmlNewElement(trackNode,"KeyFrame");

			mxmlElementSetAttr(keyFrameNode,"Time",makeScalar(keyFrame.time));

			mxml_node_t *translateNode=mxmlNewElement(keyFrameNode,"Translate");
			{
				mxmlNewOpaque(translateNode,makeVector3(keyFrame.translate));
			}

			mxml_node_t *rotateNode=mxmlNewElement(keyFrameNode,"Rotate");
			{
				mxmlNewOpaque(rotateNode,makeQuaternion(keyFrame.rotate));
			}

			mxml_node_t *scaleNode=mxmlNewElement(keyFrameNode,"Scale");
			{
				mxmlNewOpaque(scaleNode,makeVector3(keyFrame.scale));
			}
		}
	}

	return sequenceNode;
}

}
}
}
