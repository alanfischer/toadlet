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
#include <toadlet/egg/MathConversion.h>
#include <toadlet/peeper/CapabilitySet.h>
#include <toadlet/tadpole/handler/MMSHHandler.h>

using namespace toadlet::egg;
using namespace toadlet::egg::io;
using namespace toadlet::peeper;
using namespace toadlet::tadpole::mesh;

namespace toadlet{
namespace tadpole{
namespace handler{

namespace Math{
	using namespace toadlet::egg::mathfixed::Math;
}

MMSHHandler::MMSHHandler(BufferManager *bufferManager,Engine *engine){
	mBufferManager=bufferManager;
	mEngine=engine;
}

// TODO: Calculate a boundingRadius for the Mesh
Resource::ptr MMSHHandler::load(InputStream::ptr inputStream,const ResourceHandlerData *handlerData){
	DataInputStream::ptr in(new DataInputStream(inputStream));

	char signature[4];
	in->read(signature,4);
	if(signature[0]!='T' || signature[1]!='M' || signature[2]!='M' || signature[3]!='H'){
		Error::unknown(Categories::TOADLET_TADPOLE,
			"Not of TMMSH format");
		return NULL;
	}

	uint8 version=in->readUInt8();
	if(version!=3){
		Error::unknown(Categories::TOADLET_TADPOLE,
			"Not TMMH version 3");
		return NULL;
	}

	Mesh::ptr mesh(new Mesh());

	int i,j;

	int8 i8x=0,i8y=0,i8z=0,i8w=0;
	int16 i16x=0,i16y=0,i16z=0;

	while(in->available()>0){
		uint8 block=in->readUInt8();
		if(block==MESH_BLOCK){
			mesh->worldScale=MathConversion::fixedToScalar(in->readBigInt32());

			uint16 numVertexes=in->readBigUInt16();
			uint16 vertexType=in->readBigUInt16();
			int formatBit=mEngine->getIdealVertexFormatBit();

			VertexFormat::ptr vertexFormat(new VertexFormat());
			int positionElement=0;
			if((vertexType&VT_POSITION)>0){
				vertexFormat->addVertexElement(VertexElement(VertexElement::Type_POSITION,formatBit|VertexElement::Format_BIT_COUNT_3));
			}
			int normalElement=positionElement;
			if((vertexType&VT_NORMAL)>0){
				normalElement++;
				vertexFormat->addVertexElement(VertexElement(VertexElement::Type_NORMAL,formatBit|VertexElement::Format_BIT_COUNT_3));
			}
			int colorElement=normalElement;
			if((vertexType&VT_COLOR)>0){
				colorElement++;
				vertexFormat->addVertexElement(VertexElement(VertexElement::Type_COLOR_DIFFUSE,VertexElement::Format_COLOR_RGBA));
			}
			int texCoordElement=colorElement;
			if((vertexType&VT_TEXCOORD1)>0){
				texCoordElement++;
				vertexFormat->addVertexElement(VertexElement(VertexElement::Type_TEX_COORD,formatBit|VertexElement::Format_BIT_COUNT_2));
			}

			if((vertexType&VT_BONE)>0){
				mesh->vertexBoneAssignments.resize(numVertexes);
			}

			// HACK: Due to a bug in reading back vertexes from a hardware buffer in OGLES, we only load the static VertexBuffer of a Mesh if its not animated.
			VertexBuffer::ptr vertexBuffer;
			if((vertexType&VT_BONE)>0){
				vertexBuffer=mBufferManager->createVertexBuffer(Buffer::UsageFlags_STATIC,Buffer::AccessType_READ_WRITE,vertexFormat,numVertexes);
			}
			else{
				vertexBuffer=mBufferManager->createVertexBuffer(Buffer::UsageFlags_STATIC,Buffer::AccessType_WRITE_ONLY,vertexFormat,numVertexes);
			}

			VertexBufferAccessor vba;
			vba.lock(vertexBuffer,Buffer::AccessType_WRITE_ONLY);

			if((vertexType&VT_POSITION)>0){
				uint8 bytes=in->readUInt8();

				mathfixed::fixed bias[3];
				bias[0]=in->readBigInt32();
				bias[1]=in->readBigInt32();
				bias[2]=in->readBigInt32();

				mathfixed::fixed scale=in->readBigInt32();

				if(bytes==1){
					for(i=0;i<numVertexes;++i){
						i8x=in->readInt8();i8y=in->readInt8();i8z=in->readInt8();
						vba.set3(i,positionElement,
							MathConversion::fixedToScalar(Math::mul(Math::intToFixed(i8x),scale)+bias[0]),
							MathConversion::fixedToScalar(Math::mul(Math::intToFixed(i8y),scale)+bias[1]),
							MathConversion::fixedToScalar(Math::mul(Math::intToFixed(i8z),scale)+bias[2]));
					}
				}
				else if(bytes==2){
					for(i=0;i<numVertexes;++i){
						i16x=in->readBigInt16();i16y=in->readBigInt16();i16z=in->readBigInt16();
						vba.set3(i,positionElement,
							MathConversion::fixedToScalar(Math::mul(Math::intToFixed(i16x),scale)+bias[0]),
							MathConversion::fixedToScalar(Math::mul(Math::intToFixed(i16y),scale)+bias[1]),
							MathConversion::fixedToScalar(Math::mul(Math::intToFixed(i16z),scale)+bias[2]));
					}
				}
				else{
					delete mesh;
					Error::unknown(Categories::TOADLET_TADPOLE,
						"Invalid byte count");
					return NULL;
				}
			}

			if((vertexType&VT_NORMAL)>0){
				Vector3 normal;

				for(i=0;i<numVertexes;i++){
					i8x=in->readInt8();i8y=in->readInt8();i8z=in->readInt8();
					normal.set(
						MathConversion::fixedToScalar(((int)i8x)<<9),
						MathConversion::fixedToScalar(((int)i8y)<<9),
						MathConversion::fixedToScalar(((int)i8z)<<9));

					tadpole::Math::normalizeCarefully(normal,0);

					vba.set3(i,normalElement,normal);
				}
			}

			if((vertexType&VT_COLOR)>0){
				for(i=0;i<numVertexes;i++){
					uint8 r=in->readUInt8();
					uint8 g=in->readUInt8();
					uint8 b=in->readUInt8();

					vba.setABGR(i,colorElement,0xFF000000 | (((int)b)<<16) | (((int)g)<<8) | (((int)r)<<0));
				}
			}

			if((vertexType&VT_BONE)>0){
				uint8 ui8;
				for(i=0;i<numVertexes;i++){
					ui8=in->readUInt8();
					mesh->vertexBoneAssignments[i].add(Mesh::VertexBoneAssignment(ui8,Math::ONE));
				}
			}

			if((vertexType&VT_TEXCOORD1)>0){
				uint8 bytes=in->readUInt8();

				mathfixed::fixed bias[2];
				bias[0]=in->readBigInt32();
				bias[1]=in->readBigInt32();

				mathfixed::fixed scale=in->readBigInt32();

				if(bytes==1){
					for(i=0;i<numVertexes;++i){
						i8x=in->readInt8();i8y=in->readInt8();
						vba.set2(i,texCoordElement,
							MathConversion::fixedToScalar(Math::mul(Math::intToFixed(i8x),scale)+bias[0]),
							MathConversion::fixedToScalar(Math::mul(Math::intToFixed(i8y),scale)+bias[1]));
					}
				}
				else if(bytes==2){
					for(i=0;i<numVertexes;++i){
						i16x=in->readBigInt16();i16y=in->readBigInt16();
						vba.set2(i,texCoordElement,
							MathConversion::fixedToScalar(Math::mul(Math::intToFixed(i16x),scale)+bias[0]),
							MathConversion::fixedToScalar(Math::mul(Math::intToFixed(i16y),scale)+bias[1]));
					}
				}
				else{
					delete mesh;
					Error::unknown(Categories::TOADLET_TADPOLE,
						"Invalid byte count");
					return NULL;
				}
			}

			vba.unlock();
			
			mesh->staticVertexData=VertexData::ptr(new VertexData(vertexBuffer));

			uint8 numSubMeshes=in->readUInt8();
			mesh->subMeshes.resize(numSubMeshes);

			for(i=0;i<numSubMeshes;++i){
				Mesh::SubMesh::ptr sub(new Mesh::SubMesh());
				mesh->subMeshes[i]=sub;

				// Material
				{
					Material::ptr material(new Material());
					sub->material=material;

					material->setSaveLocally(true);

					uint8 faceCulling=in->readUInt8();
					if(faceCulling>Renderer::FaceCulling_BACK){
						faceCulling=Renderer::FaceCulling_BACK;
					}
					material->setFaceCulling((Renderer::FaceCulling)faceCulling);

					uint8 hasTexture=in->readUInt8();
					if(hasTexture>0){
						material->setTextureStage(0,TextureStage::ptr(new TextureStage()));
					}

					uint8 lighting=in->readUInt8();
					material->setLighting(lighting>0);
					if(lighting>0){
						LightEffect lightEffect;
						uint32 color=0;

						// MMSH stored color in ARGB, now an outdated format
						color=in->readBigUInt32();
						lightEffect.ambient=Color::rgba(((color&0xFF000000)>>24)|((color&0x00FFFFFF)<<8));
						color=in->readBigUInt32();
						lightEffect.diffuse=Color::rgba(((color&0xFF000000)>>24)|((color&0x00FFFFFF)<<8));
						color=in->readBigUInt32();
						lightEffect.specular=Color::rgba(((color&0xFF000000)>>24)|((color&0x00FFFFFF)<<8));
						lightEffect.shininess=MathConversion::fixedToScalar(in->readBigInt32());
						color=in->readBigUInt32();
						lightEffect.emissive=Color::rgba(((color&0xFF000000)>>24)|((color&0x00FFFFFF)<<8));

						material->setLightEffect(lightEffect);
					}
				}

				uint16 numIndexes=in->readBigUInt16();
				uint16 *indexes=new uint16[numIndexes];

				for(j=0;j<numIndexes;++j){
					indexes[j]=in->readBigUInt16();
				}
				
				uint16 numStrips=in->readBigUInt16();

				uint16 *stripLengths=new uint16[numStrips];

				int newNumIndexes=0;
				for(j=0;j<numStrips;++j){
					stripLengths[j]=in->readBigUInt16();
					newNumIndexes+=(stripLengths[j]-2)*3;
				}

				IndexBuffer::ptr indexBuffer=mBufferManager->createIndexBuffer(Buffer::UsageFlags_STATIC,Buffer::AccessType_WRITE_ONLY,IndexBuffer::IndexFormat_UINT_16,newNumIndexes);
				{
					uint16 *indexData=(uint16*)indexBuffer->lock(Buffer::AccessType_WRITE_ONLY);
					int ipo=0;
					int ipn=0;
					for(j=0;j<numStrips;++j){
						int k;
						for(k=0;k<stripLengths[j]-2;++k){
							if(k%2==0){
								indexData[ipn++]=indexes[ipo+k+0];
								indexData[ipn++]=indexes[ipo+k+1];
								indexData[ipn++]=indexes[ipo+k+2];
							}
							else{
								indexData[ipn++]=indexes[ipo+k+2];
								indexData[ipn++]=indexes[ipo+k+1];
								indexData[ipn++]=indexes[ipo+k+0];
							}
						}
						ipo+=stripLengths[j];
					}
					indexBuffer->unlock();
				}
				sub->indexData=IndexData::ptr(new IndexData(IndexData::Primitive_TRIS,indexBuffer,0,newNumIndexes));

				delete[] stripLengths;
				delete[] indexes;
			}
		}
		else if(block==SKELETON_BLOCK){
			Skeleton::ptr skeleton(new Skeleton());

			int numBones=in->readUInt8();
			skeleton->bones.resize(numBones);

			uint8 translateBytes=in->readUInt8();

			mathfixed::fixed translateBias[3];
			translateBias[0]=in->readBigInt32();
			translateBias[1]=in->readBigInt32();
			translateBias[2]=in->readBigInt32();

			mathfixed::fixed translateScale=in->readBigInt32();

			for(i=0;i<numBones;++i){
				Skeleton::Bone::ptr bone(new Skeleton::Bone());
				skeleton->bones[i]=bone;
				bone->index=i;

				if(translateBytes==1){
					i8x=in->readInt8();i8y=in->readInt8();i8z=in->readInt8();
					bone->translate.set(
						MathConversion::fixedToScalar(Math::mul(Math::intToFixed(i8x),translateScale)+translateBias[0]),
						MathConversion::fixedToScalar(Math::mul(Math::intToFixed(i8y),translateScale)+translateBias[1]),
						MathConversion::fixedToScalar(Math::mul(Math::intToFixed(i8z),translateScale)+translateBias[2]));
				}
				else{
					i16x=in->readBigInt16();i16y=in->readBigInt16();i16z=in->readBigInt16();
					bone->translate.set(
						MathConversion::fixedToScalar(Math::mul(Math::intToFixed(i16x),translateScale)+translateBias[0]),
						MathConversion::fixedToScalar(Math::mul(Math::intToFixed(i16y),translateScale)+translateBias[1]),
						MathConversion::fixedToScalar(Math::mul(Math::intToFixed(i16z),translateScale)+translateBias[2]));
				}

				i8x=in->readInt8();i8y=in->readInt8();i8z=in->readInt8();i8w=in->readInt8();
				bone->rotate.set(
					MathConversion::fixedToScalar(((int)i8x)<<9),
					MathConversion::fixedToScalar(((int)i8y)<<9),
					MathConversion::fixedToScalar(((int)i8z)<<9),
					MathConversion::fixedToScalar(((int)i8w)<<9));
				tadpole::Math::normalizeCarefully(bone->rotate,0);

				if(translateBytes==1){
					i8x=in->readInt8();i8y=in->readInt8();i8z=in->readInt8();
					bone->worldToBoneTranslate.set(
						MathConversion::fixedToScalar(Math::mul(Math::intToFixed(i8x),translateScale)+translateBias[0]),
						MathConversion::fixedToScalar(Math::mul(Math::intToFixed(i8y),translateScale)+translateBias[1]),
						MathConversion::fixedToScalar(Math::mul(Math::intToFixed(i8z),translateScale)+translateBias[2]));
				}
				else{
					i16x=in->readBigInt16();i16y=in->readBigInt16();i16z=in->readBigInt16();
					bone->worldToBoneTranslate.set(
						MathConversion::fixedToScalar(Math::mul(Math::intToFixed(i16x),translateScale)+translateBias[0]),
						MathConversion::fixedToScalar(Math::mul(Math::intToFixed(i16y),translateScale)+translateBias[1]),
						MathConversion::fixedToScalar(Math::mul(Math::intToFixed(i16z),translateScale)+translateBias[2]));
				}

				i8x=in->readInt8();i8y=in->readInt8();i8z=in->readInt8();i8w=in->readInt8();
				bone->worldToBoneRotate.set(
					MathConversion::fixedToScalar(((int)i8x)<<9),
					MathConversion::fixedToScalar(((int)i8y)<<9),
					MathConversion::fixedToScalar(((int)i8z)<<9),
					MathConversion::fixedToScalar(((int)i8w)<<9));
				tadpole::Math::normalizeCarefully(bone->worldToBoneRotate,0);

				bone->parentIndex=in->readInt8();
			}

			skeleton->compile();

			mesh->skeleton=skeleton;
		}
		else if(block==ANIMATION_BLOCK){
			Sequence::ptr sequence(new Sequence());

			scalar length=MathConversion::fixedToScalar(in->readBigInt32());

			uint8 translateBytes=in->readUInt8();

			mathfixed::fixed translateBias[3];
			translateBias[0]=in->readBigInt32();
			translateBias[1]=in->readBigInt32();
			translateBias[2]=in->readBigInt32();

			mathfixed::fixed translateScale=in->readBigInt32();

			uint8 numTracks=in->readUInt8();

			sequence->tracks.resize(mesh->skeleton->bones.size());

			for(i=0;i<numTracks;++i){
				Track::ptr track(new Track());

				track->length=length;

				int bone=in->readUInt8();

				sequence->tracks[bone]=track;

				uint16 numKeyFrames=in->readBigUInt16();
				track->keyFrames.resize(numKeyFrames);

				for(j=0;j<numKeyFrames;++j){
					KeyFrame &keyFrame=track->keyFrames[j];

					keyFrame.time=MathConversion::fixedToScalar(in->readBigInt32());

					if(translateBytes==1){
						i8x=in->readInt8();i8y=in->readInt8();i8z=in->readInt8();
						keyFrame.translate.set(
							MathConversion::fixedToScalar(Math::mul(Math::intToFixed(i8x),translateScale)+translateBias[0]),
							MathConversion::fixedToScalar(Math::mul(Math::intToFixed(i8y),translateScale)+translateBias[1]),
							MathConversion::fixedToScalar(Math::mul(Math::intToFixed(i8z),translateScale)+translateBias[2]));
					}
					else{
						i16x=in->readBigInt16();i16y=in->readBigInt16();i16z=in->readBigInt16();
						keyFrame.translate.set(
							MathConversion::fixedToScalar(Math::mul(Math::intToFixed(i16x),translateScale)+translateBias[0]),
							MathConversion::fixedToScalar(Math::mul(Math::intToFixed(i16y),translateScale)+translateBias[1]),
							MathConversion::fixedToScalar(Math::mul(Math::intToFixed(i16z),translateScale)+translateBias[2]));
					}

					i8x=in->readInt8();i8y=in->readInt8();i8z=in->readInt8();i8w=in->readInt8();
					keyFrame.rotate.set(
						MathConversion::fixedToScalar(((int)i8x)<<9),
						MathConversion::fixedToScalar(((int)i8y)<<9),
						MathConversion::fixedToScalar(((int)i8z)<<9),
						MathConversion::fixedToScalar(((int)i8w)<<9));
					tadpole::Math::normalizeCarefully(keyFrame.rotate,0);
				}
			}

			sequence->compile();

			mesh->skeleton->sequences.add(sequence);
		}
		else{
			Error::unknown(Categories::TOADLET_TADPOLE,
				"Invalid block type in mesh file");
			delete mesh;
			return NULL;
		}
	}

	mesh->compile();

	return mesh;
}

}
}
}


