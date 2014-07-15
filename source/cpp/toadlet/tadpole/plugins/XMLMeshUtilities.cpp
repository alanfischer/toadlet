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

#include "XMLMeshUtilities.h"
#include <toadlet/tadpole/material/Material.h>
#include <toadlet/tadpole/Engine.h>
#include <toadlet/peeper/VertexBufferAccessor.h>
#include <toadlet/peeper/IndexBufferAccessor.h>
#include <toadlet/egg/MathConversion.h>
#include <toadlet/egg/MathFormatting.h>
#include <stdio.h>

using namespace toadlet::egg::MathConversion;
using namespace toadlet::egg::MathFormatting;

namespace toadlet{
namespace tadpole{

const char LF[]={0xA,0x0};
const char CRLF[]={0xD,0xA,0x0};

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
			vba.bone=parseInt(string.substr(i,comma-i));
		}
		else{
			parseScalar(vba.weight,string.substr(i,comma-i));
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

String makeBoneAssignment(const Mesh::VertexBoneAssignmentList &vbaList){
	char buffer[128];

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

	return String(buffer);
}

Material::ptr XMLMeshUtilities::loadMaterial(mxml_node_t *materialNode,int version,Engine *engine){
	const char *prop=mxmlElementGetAttr(materialNode,"Name");

	Material::ptr material=new Material(engine->getMaterialManager());

	if(prop!=NULL){
		material->setName(prop);
	}

	RenderState::ptr renderState=engine->getMaterialManager()->createRenderState();

	if(renderState!=NULL){
		MaterialState materialState;
		renderState->getMaterialState(materialState);
		{
			mxml_node_t *lightNode=mxmlFindChild(materialNode,"Light");
			if(lightNode!=NULL){
				materialState.light=parseBool(mxmlGetOpaque(lightNode->child));
			}

			mxml_node_t *ambientNode=mxmlFindChild(materialNode,"Ambient");
			if(ambientNode!=NULL){
				parseVector4(materialState.ambient,mxmlGetOpaque(ambientNode->child));
			}

			mxml_node_t *diffuseNode=mxmlFindChild(materialNode,"Diffuse");
			if(diffuseNode!=NULL){
				parseVector4(materialState.diffuse,mxmlGetOpaque(diffuseNode->child));
			}

			mxml_node_t *specularNode=mxmlFindChild(materialNode,"Specular");
			if(specularNode!=NULL){
				parseVector4(materialState.specular,mxmlGetOpaque(specularNode->child));
			}

			mxml_node_t *shininessNode=mxmlFindChild(materialNode,"Shininess");
			if(shininessNode!=NULL){
				parseScalar(materialState.shininess,mxmlGetOpaque(shininessNode->child));
			}

			mxml_node_t *emissiveNode=mxmlFindChild(materialNode,"Emissive");
			if(emissiveNode!=NULL){
				parseVector4(materialState.emissive,mxmlGetOpaque(emissiveNode->child));
			}

			mxml_node_t *shadeNode=mxmlFindChild(materialNode,"Shade");
			if(shadeNode!=NULL){
				String shade=String(mxmlGetOpaque(shadeNode->child)).toLower();
				if(shade=="flag"){
					materialState.shade=MaterialState::ShadeType_FLAT;
				}
				else if(shade=="phong"){
					materialState.shade=MaterialState::ShadeType_PHONG;
				}
				else{
					materialState.shade=MaterialState::ShadeType_GOURAUD;
				}
			}

			if(ambientNode!=NULL || diffuseNode!=NULL || specularNode!=NULL || emissiveNode!=NULL){
				renderState->setMaterialState(materialState);
			}
		}

		RasterizerState rasterizerState;
		renderState->getRasterizerState(rasterizerState);
		{
			mxml_node_t *cullNode=mxmlFindChild(materialNode,"Cull");
			if(cullNode!=NULL){
				String cull=String(mxmlGetOpaque(cullNode->child)).toLower();
				if(cull=="front"){
					rasterizerState.cull=RasterizerState::CullType_FRONT;
				}
				else if(cull=="back"){
					rasterizerState.cull=RasterizerState::CullType_BACK;
				}
				else{
					rasterizerState.cull=RasterizerState::CullType_NONE;
				}
			}

			if(cullNode!=NULL){
				renderState->setRasterizerState(rasterizerState);
			}
		}
	
		BlendState blendState;
		renderState->getBlendState(blendState);
		{
			mxml_node_t *blendNode=mxmlFindChild(materialNode,"Blend");
			if(blendNode!=NULL){
				String blend=String(mxmlGetOpaque(blendNode->child)).toLower();
				if(blend=="alpha"){
					blendState.set(BlendState::Combination_ALPHA);
				}
				else if(blend=="color"){
					blendState.set(BlendState::Combination_COLOR);
				}
			}

			if(blendNode!=NULL){
				renderState->setBlendState(blendState);
			}
		}
	}

	material->addPath()->addPass(renderState);

	return material;
}

mxml_node_t *XMLMeshUtilities::saveMaterial(Material::ptr material,int version){
	if(material==NULL){
		return NULL;
	}

	mxml_node_t *materialNode=mxmlNewElement(MXML_NO_PARENT,"Material");

	if(material->getName()!=(char*)NULL){
		mxmlElementSetAttr(materialNode,"Name",material->getName());
	}

	RenderState::ptr renderState=material->getRenderState();

	if(renderState!=NULL){
		MaterialState materialState;
		if(renderState->getMaterialState(materialState)){
			mxml_node_t *lightNode=mxmlNewElement(materialNode,"Light");
			{
				mxmlNewOpaque(lightNode,formatBool(materialState.light));
			}

			mxml_node_t *ambientNode=mxmlNewElement(materialNode,"Ambient");
			{
				mxmlNewOpaque(ambientNode,formatVector4(materialState.ambient));
			}

			mxml_node_t *diffuseNode=mxmlNewElement(materialNode,"Diffuse");
			{
				mxmlNewOpaque(diffuseNode,formatVector4(materialState.diffuse));
			}

			mxml_node_t *specularNode=mxmlNewElement(materialNode,"Specular");
			{
				mxmlNewOpaque(specularNode,formatVector4(materialState.specular));
			}
		
			mxml_node_t *shininessNode=mxmlNewElement(materialNode,"Shininess");
			{
				mxmlNewOpaque(shininessNode,formatScalar(materialState.shininess));
			}

			mxml_node_t *emissiveNode=mxmlNewElement(materialNode,"Emissive");
			{
				mxmlNewOpaque(emissiveNode,formatVector4(materialState.emissive));
			}
		}

		RasterizerState rasterizerState;
		if(renderState->getRasterizerState(rasterizerState)){
			mxml_node_t *cullNode=mxmlNewElement(materialNode,"Cull");
			{
				switch(rasterizerState.cull){
					case RasterizerState::CullType_FRONT:
						mxmlNewOpaque(cullNode,"Front");
					break;
					case RasterizerState::CullType_BACK:
						mxmlNewOpaque(cullNode,"Back");
					break;
					case RasterizerState::CullType_NONE:
						mxmlNewOpaque(cullNode,"None");
					break;
					default:
					break;
				}
			}
		}

		BlendState blendState;
		if(renderState->getBlendState(blendState)){
			mxml_node_t *blendNode=mxmlNewElement(materialNode,"Blend");
			{
				switch(blendState.dest){
					case BlendState::Operation_ONE_MINUS_SOURCE_ALPHA:
						mxmlNewOpaque(blendNode,"Alpha");
					break;
					case BlendState::Operation_ONE_MINUS_SOURCE_COLOR:
						mxmlNewOpaque(blendNode,"Color");
					break;
					default:
					break;
				}
			}
		}
	}

	return materialNode;
}

Mesh::ptr XMLMeshUtilities::loadMesh(mxml_node_t *node,int version,Engine *engine){
	Mesh::ptr mesh=new Mesh();
	const char *prop=NULL;

	mxml_node_t *transformNode=mxmlFindChild(node,"Transform");
	if(transformNode!=NULL){
		Transform::ptr transform=new Transform();

		mxml_node_t *translateNode=mxmlFindChild(transformNode,"Translate");
		if(translateNode!=NULL){
			Vector3 translate;
			parseVector3(translate,mxmlGetOpaque(translateNode->child));
			transform->setTranslate(translate);
		}

		mxml_node_t *rotateNode=mxmlFindChild(transformNode,"Rotate");
		if(rotateNode!=NULL){
			Quaternion rotate;
			parseQuaternion(rotate,mxmlGetOpaque(rotateNode->child));
			transform->setRotate(rotate);
		}

		mxml_node_t *scaleNode=mxmlFindChild(transformNode,"Scale");
		if(scaleNode!=NULL){
			Vector3 scale;
			parseVector3(scale,mxmlGetOpaque(scaleNode->child));
			transform->setScale(scale);
		}

		mesh->setTransform(transform);
	}

	mxml_node_t *vertexNode=mxmlFindChild(node,"Vertexes");
	{
		int count=0;
		VertexFormat::ptr vertexFormat=engine->getBufferManager()->createVertexFormat();
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
		if(vbas.size()>0){
			vertexBuffer=engine->getBufferManager()->createVertexBuffer(Buffer::Usage_BIT_STAGING,Buffer::Access_READ_WRITE,vertexFormat,count);
		}
		else{
			vertexBuffer=engine->getBufferManager()->createVertexBuffer(Buffer::Usage_BIT_STATIC,Buffer::Access_BIT_WRITE,vertexFormat,count);
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
							semantic=vertexFormat->getElementSemantic(c);
						}
						if(semantic==VertexFormat::Semantic_POSITION){
							Vector3 position;
							parseVector3(position,element);
							bound.merge(position);
							vba.set3(l,pi,position);
						}
						else if(semantic==VertexFormat::Semantic_NORMAL){
							Vector3 normal;
							parseVector3(normal,element);
							vba.set3(l,ni,normal);
						}
						else if(semantic==VertexFormat::Semantic_TEXCOORD){
							Vector2 texCoord;
							parseVector2(texCoord,element);
							vba.set2(l,ti,texCoord);
						}
						else if(semantic==VertexFormat::Semantic_COLOR){
							Vector4 color;
							parseVector4(color,element);
 							vba.setRGBA(l,ci,color.getRGBA());
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

		mesh->setBound(new Bound(bound));
		mesh->setStaticVertexData(new VertexData(vertexBuffer));
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

		Mesh::SubMesh::ptr subMesh=new Mesh::SubMesh();

		prop=mxmlElementGetAttr(subMeshNode,"Name");
		if(prop!=NULL){
			subMesh->setName(prop);
		}

		mxml_node_t *indexesNode=mxmlFindChild(subMeshNode,"Indexes");
		{
			int count=0;

			prop=mxmlElementGetAttr(indexesNode,"Count");
			if(prop!=NULL){
				count=parseInt(prop);
			}

			IndexBuffer::ptr indexBuffer=engine->getBufferManager()->createIndexBuffer(Buffer::Usage_BIT_STATIC,Buffer::Access_BIT_WRITE,IndexBuffer::IndexFormat_UINT16,count);

			subMesh->indexData=new IndexData(IndexData::Primitive_TRIS,indexBuffer,0,count);

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
				subMesh->material=new Material(engine->getMaterialManager());
				subMesh->material->setName(prop);
			}
			else{
				material=loadMaterial(materialNode,version,engine);
			}

			subMesh->material=material;
		}

		mesh->addSubMesh(subMesh);
	}

	return mesh;
}

mxml_node_t *XMLMeshUtilities::saveMesh(Mesh::ptr mesh,int version){
	mxml_node_t *meshNode=mxmlNewElement(MXML_NO_PARENT,"Mesh");

	Transform *transform=mesh->getTransform();
	if(transform!=NULL){
		mxml_node_t *transformNode=mxmlNewElement(meshNode,"Transform");
		{
			mxml_node_t *translateNode=mxmlNewElement(transformNode,"Translate");
			{
				mxmlNewOpaque(translateNode,formatVector3(transform->getTranslate()));
			}

			mxml_node_t *rotateNode=mxmlNewElement(transformNode,"Rotate");
			{
				mxmlNewOpaque(rotateNode,formatQuaternion(transform->getRotate()));
			}

			mxml_node_t *scaleNode=mxmlNewElement(transformNode,"Scale");
			{
				mxmlNewOpaque(scaleNode,formatVector3(transform->getScale()));
			}
		}
	}

	VertexData::ptr vertexData=mesh->getStaticVertexData();
	if(vertexData==NULL){
		return meshNode;
	}

	mxml_node_t *vertexNode=mxmlNewElement(meshNode,"Vertexes");
	{
		VertexBuffer::ptr vertexBuffer=vertexData->getVertexBuffer(0);

		mxmlElementSetAttr(vertexNode,"Count",formatInt(vertexBuffer->getSize()));

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
		String data;
		int i;
		for(i=0;i<vertexBuffer->getSize();++i){
			line[0]=0;

			if(i==0) strcat(line,"\n");
			strcat(line,"\t\t\t");

			if(pi>=0){
				vba.get3(i,pi,v3); strcat(line,formatVector3(v3)); strcat(line," ");
			}
			if(ni>=0){
				vba.get3(i,ni,v3); strcat(line,formatVector3(v3)); strcat(line," ");
			}
			if(ti>=0){
				vba.get2(i,ti,v2); strcat(line,formatVector2(v2)); strcat(line," ");
			}
			if(ci>=0){
				strcat(line,formatVector4(Vector4(vba.getRGBA(i,ci)))); strcat(line," ");
			}
			if(vbas.size()>0){
				strcat(line,makeBoneAssignment(vbas[i])); strcat(line," ");
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
			if(subMesh->getName()!=(char*)NULL){
				mxmlElementSetAttr(subMeshNode,"Name",subMesh->getName());
			}

			IndexData::ptr indexData=subMesh->indexData;
			IndexBuffer::ptr indexBuffer=indexData->getIndexBuffer();

			IndexBufferAccessor iba;
			iba.lock(indexBuffer,Buffer::Access_BIT_WRITE);

			mxml_node_t *indexNode=mxmlNewElement(subMeshNode,"Indexes");
			{
				mxmlElementSetAttr(indexNode,"Count",formatInt(indexBuffer->getSize()));

				String line;
				int j;
				for(j=0;j<indexBuffer->getSize();++j){
					line+=formatInt(iba.get(j));

					if(j<indexBuffer->getSize()-1){
						line+=" ";
					}
				}

				mxmlNewOpaque(indexNode,line);
			}

			mxml_node_t *materialNode=NULL;
			if(subMesh->materialName.length()==0){
				materialNode=saveMaterial(subMesh->material,version);
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
	Skeleton::ptr skeleton=new Skeleton();

	const char *prop=NULL;
	mxml_node_t *boneNode=node->child;
	while((boneNode=boneNode->next)!=NULL){
		if(strcmp(mxmlGetElementName(boneNode),"Bone")!=0){
			continue;
		}

		Skeleton::Bone::ptr bone=new Skeleton::Bone();

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
			parseVector3(bone->translate,mxmlGetOpaque(translateNode->child));
		}

		mxml_node_t *rotateNode=mxmlFindChild(boneNode,"Rotate");
		if(rotateNode!=NULL){
			parseQuaternion(bone->rotate,mxmlGetOpaque(rotateNode->child));
		}

		mxml_node_t *scaleNode=mxmlFindChild(boneNode,"Scale");
		if(scaleNode!=NULL){
			parseVector3(bone->scale,mxmlGetOpaque(scaleNode->child));
		}

		skeleton->addBone(bone);
	}

	skeleton->compile();

	return skeleton;
}

mxml_node_t *XMLMeshUtilities::saveSkeleton(Skeleton::ptr skeleton,int version){
	mxml_node_t *skeletonNode=mxmlNewElement(MXML_NO_PARENT,"Skeleton");

	int i;
	for(i=0;i<skeleton->getNumBones();++i){
		Skeleton::Bone::ptr bone=skeleton->getBone(i);

		mxml_node_t *boneNode=mxmlNewElement(skeletonNode,"Bone");
		{
			mxmlElementSetAttr(boneNode,"Index",formatInt(bone->index));

			mxmlElementSetAttr(boneNode,"Parent",formatInt(bone->parentIndex));

			if(bone->name!=(char*)NULL){
				mxmlElementSetAttr(boneNode,"Name",bone->name);
			}

			mxml_node_t *translateNode=mxmlNewElement(boneNode,"Translate");
			{
				mxmlNewOpaque(translateNode,formatVector3(bone->translate));
			}

			mxml_node_t *rotateNode=mxmlNewElement(boneNode,"Rotate");
			{
				mxmlNewOpaque(rotateNode,formatQuaternion(bone->rotate));
			}

			mxml_node_t *scaleNode=mxmlNewElement(boneNode,"Scale");
			{
				mxmlNewOpaque(scaleNode,formatVector3(bone->scale));
			}
		}
	}

	return skeletonNode;
}

Sequence::ptr XMLMeshUtilities::loadSequence(mxml_node_t *node,int version,Engine *engine){
	Sequence::ptr sequence=new Sequence();

	const char *prop=NULL;
	prop=mxmlElementGetAttr(node,"Name");
	if(prop!=NULL){
		sequence->setName(prop);
	}

	scalar sequenceLength=0;
	prop=mxmlElementGetAttr(node,"Length");
	if(prop!=NULL){
		parseScalar(sequenceLength,prop);
	}

	mxml_node_t *trackNode=node->child;
	while((trackNode=trackNode->next)!=NULL){
		if(strcmp(mxmlGetElementName(trackNode),"Track")!=0){
			continue;
		}

		VertexFormat::ptr format=engine->getBufferManager()->createVertexFormat();
		format->addElement(VertexFormat::Semantic_POSITION,0,VertexFormat::Format_TYPE_FLOAT_32|VertexFormat::Format_COUNT_3);
		format->addElement(VertexFormat::Semantic_ROTATE,0,VertexFormat::Format_TYPE_FLOAT_32|VertexFormat::Format_COUNT_4);
		format->addElement(VertexFormat::Semantic_SCALE,0,VertexFormat::Format_TYPE_FLOAT_32|VertexFormat::Format_COUNT_3);

		Track::ptr track=new Track(format);
		sequence->addTrack(track);

		if(version<=2){
			prop=mxmlElementGetAttr(trackNode,"Bone");
		}
		else{
			prop=mxmlElementGetAttr(trackNode,"Index");
		}
		if(prop!=NULL){
			track->setIndex(parseInt(prop));
		}

		track->setLength(sequenceLength);

		VertexBufferAccessor &vba=track->getAccessor();
		mxml_node_t *keyFrameNode=trackNode->child;
		while((keyFrameNode=keyFrameNode->next)!=NULL){
			if(strcmp(mxmlGetElementName(keyFrameNode),"KeyFrame")!=0){
				continue;
			}

			int index=0;
			prop=mxmlElementGetAttr(keyFrameNode,"Time");
			if(prop!=NULL){
				scalar time;
				parseScalar(time,prop);
				index=track->addKeyFrame(time);
			}

			mxml_node_t *translateNode=mxmlFindChild(keyFrameNode,"Translate");
			if(translateNode!=NULL){
				Vector3 translate;
				parseVector3(translate,mxmlGetOpaque(translateNode->child));
				vba.set3(index,0,translate);
			}

			mxml_node_t *rotateNode=mxmlFindChild(keyFrameNode,"Rotate");
			if(rotateNode!=NULL){
				Quaternion rotate;
				parseQuaternion(rotate,mxmlGetOpaque(rotateNode->child));
				vba.set4(index,1,rotate);
			}

			mxml_node_t *scaleNode=mxmlFindChild(keyFrameNode,"Scale");
			if(scaleNode!=NULL){
				Vector3 scale;
				parseVector3(scale,mxmlGetOpaque(scaleNode->child));
				vba.set3(index,2,scale);
			}
		}
	}

	sequence->compile();

	return sequence;
}

mxml_node_t *XMLMeshUtilities::saveSequence(Sequence::ptr sequence,int version){
	mxml_node_t *sequenceNode=mxmlNewElement(MXML_NO_PARENT,"Sequence");

	if(sequence->getName()!=(char*)NULL){
		mxmlElementSetAttr(sequenceNode,"Name",sequence->getName());
	}

	mxmlElementSetAttr(sequenceNode,"Length",formatScalar(sequence->getLength()));

	int j;
	for(j=0;j<sequence->getNumTracks();++j){
		Track::ptr track=sequence->getTrack(j);

		mxml_node_t *trackNode=mxmlNewElement(sequenceNode,"Track");

		mxmlElementSetAttr(trackNode,"Index",formatInt(track->getIndex()));

		VertexBufferAccessor &vba=track->getAccessor();
		int k;
		for(k=0;k<track->getNumKeyFrames();++k){
			mxml_node_t *keyFrameNode=mxmlNewElement(trackNode,"KeyFrame");

			mxmlElementSetAttr(keyFrameNode,"Time",formatScalar(track->getTime(k)));

			mxml_node_t *translateNode=mxmlNewElement(keyFrameNode,"Translate");
			{
				Vector3 translate;
				vba.get3(k,0,translate);
				mxmlNewOpaque(translateNode,formatVector3(translate));
			}

			mxml_node_t *rotateNode=mxmlNewElement(keyFrameNode,"Rotate");
			{
				Quaternion rotate;
				vba.get4(k,1,rotate);
				mxmlNewOpaque(rotateNode,formatQuaternion(rotate));
			}

			mxml_node_t *scaleNode=mxmlNewElement(keyFrameNode,"Scale");
			{
				Vector3 scale;
				vba.get3(k,2,scale);
				mxmlNewOpaque(scaleNode,formatVector3(scale));
			}
		}
	}

	return sequenceNode;
}

void XMLMeshUtilities::MaterialRequest::request(){
	if(mIndex<mMesh->getNumSubMeshes()){
		if(mMesh->getSubMesh(mIndex)->materialName.length()!=0){
			mMaterialManager->find(mMesh->getSubMesh(mIndex)->materialName,this);
		}
		else{
			resourceException(Exception());
		}
	}
	else{
		mMesh->compile();
		mRequest->resourceReady(mMesh);
	}
}

void XMLMeshUtilities::MaterialRequest::resourceReady(Resource *resource){
	Material::ptr material=(Material*)resource;
	Mesh::SubMesh *subMesh=mMesh->getSubMesh(mIndex);
	if(material->getRenderState() && subMesh->material!=NULL && subMesh->material->getRenderState()!=NULL){
		mMaterialManager->modifyRenderState(material->getRenderState(),subMesh->material->getRenderState());
	}
	subMesh->material=material;

	mIndex++;
	request();
}

void XMLMeshUtilities::MaterialRequest::resourceException(const Exception &ex){
	Mesh::SubMesh *subMesh=mMesh->getSubMesh(mIndex);
	Material::ptr material=mEngine->createDiffuseMaterial(NULL,subMesh->material->getRenderState());
	subMesh->material=material;

	mIndex++;
	request();
}


}
}
