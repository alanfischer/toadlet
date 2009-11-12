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
#include <toadlet/tadpole/Material.h>
#include <toadlet/peeper/VertexBufferAccessor.h>
#include <toadlet/peeper/IndexBufferAccessor.h>
#include <toadlet/egg/MathConversion.h>
#include <toadlet/egg/Error.h>
#include <stdio.h>

#if defined(TOADLET_PLATFORM_WIN32) && defined(TOADLET_LIBMXML_NAME)
	#pragma comment(lib,TOADLET_LIBMXML_NAME)
#endif

using namespace toadlet::egg;
using namespace toadlet::egg::MathConversion;
using namespace toadlet::peeper;
using namespace toadlet::tadpole::mesh;

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

Quaternion parseQuaternion(const char *string){
	float x=0,y=0,z=0,w=0;
	sscanf(string,"%f,%f,%f,%f",&x,&y,&z,&w);
	return Quaternion(floatToScalar(x),floatToScalar(y),floatToScalar(z),floatToScalar(w));
}

String makeQuaternion(const Quaternion &q){
	return String()+scalarToFloat(q.x)+','+scalarToFloat(q.y)+','+scalarToFloat(q.z)+','+scalarToFloat(q.w);
}

Color parseColor(const char *string){
	float r=0,g=0,b=0,a=0;
	sscanf(string,"%f,%f,%f,%f",&r,&g,&b,&a);
	return Color(floatToScalar(r),floatToScalar(g),floatToScalar(b),floatToScalar(a));
}

String makeColor(const Color &c){
	return String()+scalarToFloat(c.r)+','+scalarToFloat(c.g)+','+scalarToFloat(c.b)+','+scalarToFloat(c.a);
}

const char *makeColor(char *buffer,const Color &c){
	sprintf(buffer,"%f,%f,%f,%f ",scalarToFloat(c.r),scalarToFloat(c.g),scalarToFloat(c.b),scalarToFloat(c.a));
	return buffer;
}

Mesh::VertexBoneAssignmentList parseBoneAssignment(const String &string){
	int i=0;
	int c=0;
	Mesh::VertexBoneAssignment vba;
	Mesh::VertexBoneAssignmentList vbaList;
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
			vbaList.add(vba);
		}

		i=comma+1;
		c++;
		if(comma>=string.length()){
			break;
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

const char *MAP_NAMES[]={
	"Diffuse",
	"Ambient",
	"Specular",
	"SelfIllumination",
	"Opacity",
	"Bump",
	"Reflection",
	"Refraction",
};

int MAX_MAP=8;

Material::ptr XMLMeshUtilities::loadMaterial(mxml_node_t *node,int version,ResourceManager *textureManager){
	Material::ptr material(new Material());

	const char *prop=mxmlElementGetAttr(node,"Name");
	if(prop!=NULL){
		material->setName(prop);
	}

	mxml_node_t *lightEffectNode=mxmlFindChild(node,"LightEffect");
	if(lightEffectNode!=NULL){
		LightEffect le;

		mxml_node_t *ambientNode=mxmlFindChild(lightEffectNode,"Ambient");
		if(ambientNode!=NULL){
			const char *data=mxmlGetOpaque(ambientNode->child);
			if(data!=NULL){
				le.ambient=parseColor(data);
			}
		}

		mxml_node_t *diffuseNode=mxmlFindChild(lightEffectNode,"Diffuse");
		if(diffuseNode!=NULL){
			const char *data=mxmlGetOpaque(diffuseNode->child);
			if(data!=NULL){
				le.diffuse=parseColor(data);
			}
		}

		mxml_node_t *specularNode=mxmlFindChild(lightEffectNode,"Specular");
		if(specularNode!=NULL){
			const char *data=mxmlGetOpaque(specularNode->child);
			if(data!=NULL){
				le.specular=parseColor(data);
			}
		}

		mxml_node_t *shininessNode=mxmlFindChild(lightEffectNode,"Shininess");
		if(shininessNode!=NULL){
			const char *data=mxmlGetOpaque(shininessNode->child);
			if(data!=NULL){
				le.shininess=parseScalar(data);
			}
		}

		mxml_node_t *emissiveNode=mxmlFindChild(lightEffectNode,"Emissive");
		if(emissiveNode!=NULL){
			const char *data=mxmlGetOpaque(emissiveNode->child);
			if(data!=NULL){
				le.emissive=parseColor(data);
			}
		}

		mxml_node_t *trackColorNode=mxmlFindChild(lightEffectNode,"TrackColor");
		if(trackColorNode!=NULL){
			const char *data=mxmlGetOpaque(trackColorNode->child);
			if(data!=NULL){
				le.trackColor=parseInt(data)>0;
			}
		}

		material->setLightEffect(le);
	}

	mxml_node_t *lightingNode=NULL;
	if(version<=2){
		lightingNode=mxmlFindChild(node,"LightingEnabled");
	}
	else{
		lightingNode=mxmlFindChild(node,"Lighting");
	}
	if(lightingNode!=NULL){
		const char *data=mxmlGetOpaque(lightingNode->child);
		if(data!=NULL){
			bool lighting=parseBool(data);
			material->setLighting(lighting);
		}
	}

	mxml_node_t *faceCullingNode=mxmlFindChild(node,"FaceCulling");
	if(faceCullingNode!=NULL){
		const char *data=mxmlGetOpaque(faceCullingNode->child);
		if(data!=NULL){
			if(version<=2){
				if(strcmp(data,"Back")==0){
					material->setFaceCulling(Renderer::FaceCulling_BACK);
				}
				else if(strcmp(data,"Front")==0){
					material->setFaceCulling(Renderer::FaceCulling_FRONT);
				}
				else if(strcmp(data,"None")==0){
					material->setFaceCulling(Renderer::FaceCulling_NONE);
				}
			}
			else{
				if(strcmp(data,"back")==0){
					material->setFaceCulling(Renderer::FaceCulling_BACK);
				}
				else if(strcmp(data,"front")==0){
					material->setFaceCulling(Renderer::FaceCulling_FRONT);
				}
				else if(strcmp(data,"none")==0){
					material->setFaceCulling(Renderer::FaceCulling_NONE);
				}
			}
		}
	}

	if(version<=2){
		mxml_node_t *mapNode=node->child;
		while((mapNode=mapNode->next)!=NULL){
			if(strcmp(mxmlGetElementName(mapNode),"Map")!=0){
				continue;
			}

			int type=MAX_MAP;
			String file;
			float amount;

			const char *prop=mxmlElementGetAttr(mapNode,"Type");
			if(prop!=NULL){
				int i;
				for(i=0;i<MAX_MAP;++i){
					if(strcmp(prop,MAP_NAMES[i])==0){
						type=i;
						break;
					}
				}
			}

			if(type!=MAX_MAP){
				mxml_node_t *fileNode=mxmlFindChild(mapNode,"File");
				if(fileNode!=NULL){
					const char *data=mxmlGetOpaque(fileNode->child);
					if(data!=NULL){
						file=data;
						file=file.trimLeft().trimRight();
					}
				}

				mxml_node_t *amountNode=mxmlFindChild(mapNode,"Amount");
				if(amountNode!=NULL){
					const char *data=mxmlGetOpaque(amountNode->child);
					if(data!=NULL){
						amount=parseScalar(data);
					}
				}

				if(textureManager!=NULL){
					textureManager->cleanFilename(file);
					material->setTextureStage(0,TextureStage::ptr(new TextureStage(textureManager->findTexture(file))));
				}
			}
		}
	}
	else{
		mxml_node_t *textureStageNode=node->child;
		// TODO: Support more than 1 texture stage
		while((textureStageNode=textureStageNode->next)!=NULL){
			if(strcmp(mxmlGetElementName(textureStageNode),"TextureStage")!=0){
				continue;
			}

			TextureStage::ptr textureStage(new TextureStage());

			mxml_node_t *textureNode=mxmlFindChild(textureStageNode,"Texture");
			if(textureNode!=NULL){
				prop=mxmlElementGetAttr(textureNode,"File");
				if(prop!=NULL){
					Texture::ptr texture;
					String textureName=prop;
					if(textureManager!=NULL){
						textureManager->cleanFilename(textureName);
						texture=textureManager->findTexture(textureName);
						textureStage->setTexture(texture);
					}
					if(texture!=NULL){
						textureStage->setTextureName(texture->getName());
					}
					else{
						textureStage->setTextureName(textureName);
					}
				}
			}
		}
	}

	return material;
}

mxml_node_t *XMLMeshUtilities::saveMaterial(Material::ptr material,int version){
	mxml_node_t *materialNode=mxmlNewElement(MXML_NO_PARENT,"Material");

	mxmlElementSetAttr(materialNode,"Name",material->getName());

	if(material->getSaveLocally()){
		mxml_node_t *lightEffectNode=mxmlNewElement(materialNode,"LightEffect");
		{
			const LightEffect &lightEffect=material->getLightEffect();

			mxml_node_t *ambientNode=mxmlNewElement(lightEffectNode,"Ambient");
			{
				mxmlNewOpaque(ambientNode,makeColor(lightEffect.ambient));
			}

			mxml_node_t *diffuseNode=mxmlNewElement(lightEffectNode,"Diffuse");
			{
				mxmlNewOpaque(diffuseNode,makeColor(lightEffect.diffuse));
			}

			mxml_node_t *specularNode=mxmlNewElement(lightEffectNode,"Specular");
			{
				mxmlNewOpaque(specularNode,makeColor(lightEffect.specular));
			}

			mxml_node_t *shininessNode=mxmlNewElement(lightEffectNode,"Shininess");
			{
				mxmlNewOpaque(shininessNode,makeScalar(lightEffect.shininess));
			}

			mxml_node_t *emissiveNode=mxmlNewElement(lightEffectNode,"Emissive");
			{
				mxmlNewOpaque(emissiveNode,makeColor(lightEffect.emissive));
			}

			mxml_node_t *trackColorNode=mxmlNewElement(lightEffectNode,"TrackColor");
			{
				mxmlNewOpaque(trackColorNode,makeInt(lightEffect.trackColor));
			}
		}

		mxml_node_t *lightingNode=NULL;
		if(version<=2){
			lightingNode=mxmlNewElement(materialNode,"LightingEnabled");
		}
		else{
			lightingNode=mxmlNewElement(materialNode,"Lighting");
		}
		{
			mxmlNewOpaque(lightingNode,makeBool(material->getLighting()));
		}

		mxml_node_t *faceCullingNode=mxmlNewElement(materialNode,"FaceCulling");
		{
			if(version<=2){
				if(material->getFaceCulling()==Renderer::FaceCulling_BACK){
					mxmlNewOpaque(faceCullingNode,"Back");
				}
				else if(material->getFaceCulling()==Renderer::FaceCulling_FRONT){
					mxmlNewOpaque(faceCullingNode,"Front");
				}
				else if(material->getFaceCulling()==Renderer::FaceCulling_NONE){
					mxmlNewOpaque(faceCullingNode,"None");
				}
			}
			else{
				if(material->getFaceCulling()==Renderer::FaceCulling_BACK){
					mxmlNewOpaque(faceCullingNode,"back");
				}
				else if(material->getFaceCulling()==Renderer::FaceCulling_FRONT){
					mxmlNewOpaque(faceCullingNode,"front");
				}
				else if(material->getFaceCulling()==Renderer::FaceCulling_NONE){
					mxmlNewOpaque(faceCullingNode,"none");
				}
			}
		}

		int i;
		for(i=0;i<material->getNumTextureStages();++i){
			if(version<=2){
				String name=material->getTextureStage(i)->getTextureName();
				float amount=Math::ONE;

				if(name!=(char*)NULL){
					mxml_node_t *mapNode=mxmlNewElement(materialNode,"Map");
					{
						mxmlElementSetAttr(mapNode,"Type",MAP_NAMES[i]);

						mxml_node_t *fileNode=mxmlNewElement(mapNode,"File");
						{
							mxmlNewOpaque(fileNode,name);
						}

						mxml_node_t *amountNode=mxmlNewElement(mapNode,"Amount");
						{
							mxmlNewOpaque(amountNode,makeScalar(amount));
						}
					}
				}
			}
			else{
				mxml_node_t *textureStageNode=mxmlNewElement(materialNode,"TextureStage");
				{
					TextureStage *textureStage=material->getTextureStage(i);

					mxml_node_t *textureNode=mxmlNewElement(textureStageNode,"Texture");
					{
						Texture *texture=textureStage->getTexture();

						String textureName;
						if(texture!=NULL){
							textureName=texture->getName();
						}
						else{
							textureName=textureStage->getTextureName();
						}

						mxmlElementSetAttr(textureNode,"File",textureName);
					}
				}
			}
		}
	}

	return materialNode;
}

Mesh::ptr XMLMeshUtilities::loadMesh(mxml_node_t *node,int version,BufferManager *bufferManager,ResourceManager *materialManager,TextureManager *textureManager){
	Mesh::ptr mesh(new Mesh());
	const char *prop=NULL;

	mxml_node_t *vertexNode=mxmlFindChild(node,"Vertexes");
	{
		int count=0;
		VertexFormat::ptr vertexFormat(new VertexFormat());

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
					vertexFormat->addVertexElement(VertexElement(VertexElement::Type_POSITION,VertexElement::Format(VertexElement::Format_BIT_FLOAT_32|VertexElement::Format_BIT_COUNT_3)));
				}
				else if(t=="Normal"){
					vertexFormat->addVertexElement(VertexElement(VertexElement::Type_NORMAL,VertexElement::Format(VertexElement::Format_BIT_FLOAT_32|VertexElement::Format_BIT_COUNT_3)));
				}
				else if(t=="TexCoord"){
					vertexFormat->addVertexElement(VertexElement(VertexElement::Type_TEX_COORD,VertexElement::Format(VertexElement::Format_BIT_FLOAT_32|VertexElement::Format_BIT_COUNT_2)));
				}
				else if(t=="Color"){
					vertexFormat->addVertexElement(VertexElement(VertexElement::Type_COLOR_DIFFUSE,VertexElement::Format_COLOR_RGBA));
				}
				else if(t=="Bone"){
					mesh->vertexBoneAssignments.resize(count);
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

		// HACK: Due to a bug in reading back vertexes from a hardware buffer in OGLES, we only load the static VertexBuffer of a Mesh if its not animated.
		VertexBuffer::ptr vertexBuffer;
		if(mesh->vertexBoneAssignments.size()>0){
			vertexBuffer=bufferManager->createVertexBuffer(Buffer::UsageFlags_STATIC,Buffer::AccessType_READ_WRITE,vertexFormat,count);
		}
		else{
			vertexBuffer=bufferManager->createVertexBuffer(Buffer::UsageFlags_STATIC,Buffer::AccessType_WRITE_ONLY,vertexFormat,count);
		}

		int pi=vertexFormat->getVertexElementIndexOfType(VertexElement::Type_POSITION);
		int ni=vertexFormat->getVertexElementIndexOfType(VertexElement::Type_NORMAL);
		int ti=vertexFormat->getVertexElementIndexOfType(VertexElement::Type_TEX_COORD);
		int ci=vertexFormat->getVertexElementIndexOfType(VertexElement::Type_COLOR_DIFFUSE);

		VertexBufferAccessor vba;
		vba.lock(vertexBuffer,Buffer::AccessType_WRITE_ONLY);

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
						if(vertexFormat->getVertexElement(c).type==VertexElement::Type_POSITION){
							Vector3 position=parseVector3(element);
							scalar length=Math::length(position);;
							if(mesh->boundingRadius<length){
								mesh->boundingRadius=length;
							}
							vba.set3(l,pi,position);
						}
						else if(vertexFormat->getVertexElement(c).type==VertexElement::Type_NORMAL){
							vba.set3(l,ni,parseVector3(element));
						}
						else if(vertexFormat->getVertexElement(c).type==VertexElement::Type_TEX_COORD){
							vba.set2(l,ti,parseVector2(element));
						}
						else if(vertexFormat->getVertexElement(c).type==VertexElement::Type_COLOR_DIFFUSE){
							vba.setRGBA(l,ci,parseColor(element).getRGBA());
						}
						else if(mesh->vertexBoneAssignments.size()>0){
							mesh->vertexBoneAssignments[l]=parseBoneAssignment(element);
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

		mesh->staticVertexData=VertexData::ptr(new VertexData(vertexBuffer));
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

			IndexBuffer::ptr indexBuffer=bufferManager->createIndexBuffer(Buffer::UsageFlags_STATIC,Buffer::AccessType_WRITE_ONLY,IndexBuffer::IndexFormat_UINT_16,count);
			subMesh->indexData=IndexData::ptr(new IndexData(IndexData::Primitive_TRIS,indexBuffer,0,count));

			IndexBufferAccessor iba;
			iba.lock(indexBuffer,Buffer::AccessType_WRITE_ONLY);

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
				materialName=prop;
				if(materialManager!=NULL){
					material=materialManager->findMaterial(materialName);
				}
			}
			else{
				material=Material::ptr(loadMaterial(materialNode,version,textureManager));
				material->setSaveLocally(true);
			}

			subMesh->material=material;
			if(material!=NULL){
				subMesh->materialName=material->getName();
			}
			else{
				subMesh->materialName=materialName;
			}
		}

		mesh->subMeshes.add(subMesh);
	}

	mesh->compile();

	return mesh;
}

mxml_node_t *XMLMeshUtilities::saveMesh(Mesh::ptr mesh,int version){
	mxml_node_t *meshNode=NULL;
	if(version<=2){
		meshNode=mxmlNewElement(MXML_NO_PARENT,"MeshData");
	}
	else{
		meshNode=mxmlNewElement(MXML_NO_PARENT,"Mesh");
	}

	const VertexData::ptr &vertexData=mesh->staticVertexData;
	if(vertexData==NULL){
		return meshNode;
	}

	mxml_node_t *vertexNode=mxmlNewElement(meshNode,"Vertexes");
	{
		const VertexBuffer::ptr &vertexBuffer=vertexData->getVertexBuffer(0);

		mxmlElementSetAttr(vertexNode,"Count",makeInt(vertexBuffer->getSize()));

		VertexFormat::ptr vertexFormat=vertexBuffer->getVertexFormat();
		String type;

		if(vertexFormat->hasVertexElementOfType(VertexElement::Type_POSITION)){
			type+="Position,";
		}
		if(vertexFormat->hasVertexElementOfType(VertexElement::Type_NORMAL)){
			type+="Normal,";
		}
		if(vertexFormat->hasVertexElementOfType(VertexElement::Type_TEX_COORD)){
			type+="TexCoord,";
		}
		if(vertexFormat->hasVertexElementOfType(VertexElement::Type_COLOR_DIFFUSE)){
			type+="Color,";
		}
		if(mesh->vertexBoneAssignments.size()>0){
			type+="Bone,";
		}

		// Drop trailing ','
		if(type!=(char*)NULL){
			type=type.substr(0,type.length()-1);
		}
		mxmlElementSetAttr(vertexNode,"Type",type);

		int pi=vertexFormat->getVertexElementIndexOfType(VertexElement::Type_POSITION);
		int ni=vertexFormat->getVertexElementIndexOfType(VertexElement::Type_NORMAL);
		int ti=vertexFormat->getVertexElementIndexOfType(VertexElement::Type_TEX_COORD);
		int ci=vertexFormat->getVertexElementIndexOfType(VertexElement::Type_COLOR_DIFFUSE);

		VertexBufferAccessor vba;
		vba.lock(vertexBuffer,Buffer::AccessType_WRITE_ONLY);

		Vector3 v3;
		Vector2 v2;
		char line[1024];
		char buffer[128];
		String data;
		int i;
		for(i=0;i<vertexBuffer->getSize();++i){
			line[0]=0;
			buffer[0]=0;

			if(i==0) strcat(line,"\n");
			strcat(line,"\t\t\t");

			if(vertexFormat->hasVertexElementOfType(VertexElement::Type_POSITION)){
				vba.get3(i,pi,v3); strcat(line,makeVector3(buffer,v3));
			}
			if(vertexFormat->hasVertexElementOfType(VertexElement::Type_NORMAL)){
				vba.get3(i,ni,v3); strcat(line,makeVector3(buffer,v3));
			}
			if(vertexFormat->hasVertexElementOfType(VertexElement::Type_TEX_COORD)){
				vba.get2(i,ti,v2); strcat(line,makeVector2(buffer,v2));
			}
			if(vertexFormat->hasVertexElementOfType(VertexElement::Type_COLOR_DIFFUSE)){
				strcat(line,makeColor(buffer,Color::rgba(vba.getRGBA(i,ci))));
			}
			if(mesh->vertexBoneAssignments.size()>0){
				strcat(line,makeBoneAssignment(buffer,mesh->vertexBoneAssignments[i]));
			}

			strcat(line,LF);
			if(i==vertexBuffer->getSize()-1) strcat(line,"\t\t");

			data+=line;
		}

		vba.unlock();

		mxmlNewOpaque(vertexNode,data);
	}

	int i;
	for(i=0;i<mesh->subMeshes.size();++i){
		Mesh::SubMesh::ptr subMesh=mesh->subMeshes[i];

		mxml_node_t *subMeshNode=NULL;
		if(version<=2){
			subMeshNode=mxmlNewElement(meshNode,"SubMeshData");
		}
		else{
			subMeshNode=mxmlNewElement(meshNode,"SubMesh");
		}
		{
			if(subMesh->name!=(char*)NULL){
				mxmlElementSetAttr(subMeshNode,"Name",subMesh->name);
			}

			const IndexData::ptr &indexData=subMesh->indexData;
			const IndexBuffer::ptr &indexBuffer=indexData->getIndexBuffer();

			IndexBufferAccessor iba;
			iba.lock(indexBuffer,Buffer::AccessType_WRITE_ONLY);

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

			Material::ptr material=subMesh->material;
			if(material!=NULL && material->getSaveLocally()){
				mxmlAddChild(subMeshNode,saveMaterial(material,version));
			}
			else{
				String materialName;
				if(material!=NULL){
					materialName=material->getName();
				}
				else{
					materialName=subMesh->materialName;
				}

				mxml_node_t *materialNode=mxmlNewElement(subMeshNode,"Material");
				mxmlElementSetAttr(materialNode,"File",materialName);
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

		mxml_node_t *worldToBoneTranslateNode=mxmlFindChild(boneNode,"WorldToBoneTranslate");
		if(worldToBoneTranslateNode!=NULL){
			bone->worldToBoneTranslate=parseVector3(mxmlGetOpaque(worldToBoneTranslateNode->child));
		}

		mxml_node_t *worldToBoneRotateNode=mxmlFindChild(boneNode,"WorldToBoneRotate");
		if(worldToBoneRotateNode!=NULL){
			bone->worldToBoneRotate=parseQuaternion(mxmlGetOpaque(worldToBoneRotateNode->child));
		}

		skeleton->bones.add(bone);
	}

	skeleton->compile();

	return skeleton;
}

mxml_node_t *XMLMeshUtilities::saveSkeleton(Skeleton::ptr skeleton,int version){
	mxml_node_t *skeletonNode=NULL;
	if(version<=2){
		skeletonNode=mxmlNewElement(MXML_NO_PARENT,"SkeletonData");
	}
	else{
		skeletonNode=mxmlNewElement(MXML_NO_PARENT,"Skeleton");
	}

	int i;
	for(i=0;i<skeleton->bones.size();++i){
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

			mxml_node_t *worldToBoneTranslateNode=mxmlNewElement(boneNode,"WorldToBoneTranslate");
			{
				mxmlNewOpaque(worldToBoneTranslateNode,makeVector3(bone->worldToBoneTranslate));
			}

			mxml_node_t *worldToBoneRotateNode=mxmlNewElement(boneNode,"WorldToBoneRotate");
			{
				mxmlNewOpaque(worldToBoneRotateNode,makeQuaternion(bone->worldToBoneRotate));
			}
		}
	}

	return skeletonNode;
}

Sequence::ptr XMLMeshUtilities::loadSequence(mxml_node_t *node,int version){
	Sequence::ptr sequence(new Sequence());

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

		Track::ptr track(new Track());
		sequence->tracks.add(track);

		prop=mxmlElementGetAttr(trackNode,"Index");
		if(prop!=NULL){
			track->index=parseInt(prop);
		}

		track->length=sequenceLength;

		mxml_node_t *keyFrameNode=trackNode->child;
		while((keyFrameNode=keyFrameNode->next)!=NULL){
			if(strcmp(mxmlGetElementName(keyFrameNode),"KeyFrame")!=0){
				continue;
			}

			KeyFrame keyFrame;
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

mxml_node_t *XMLMeshUtilities::saveSequence(Sequence::ptr sequence,int version){
	mxml_node_t *sequenceNode=NULL;
	if(version<=2){
		sequenceNode=mxmlNewElement(MXML_NO_PARENT,"AnimationData");
	}
	else{
		sequenceNode=mxmlNewElement(MXML_NO_PARENT,"Sequence");
	}

	if(sequence->getName()!=(char*)NULL){
		mxmlElementSetAttr(sequenceNode,"Name",sequence->getName());
	}

	mxmlElementSetAttr(sequenceNode,"Length",makeScalar(sequence->length));

	int j;
	for(j=0;j<sequence->tracks.size();++j){
		Track::ptr track=sequence->tracks[j];

		mxml_node_t *trackNode=mxmlNewElement(sequenceNode,"Track");

		mxmlElementSetAttr(trackNode,"Index",makeInt(track->index));

		int k;
		for(k=0;k<track->keyFrames.size();++k){
			const KeyFrame keyFrame=track->keyFrames[k];

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
