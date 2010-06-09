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

MMSHHandler::MMSHHandler(Engine *engine){
	mBufferManager=engine->getBufferManager();
	mMaterialManager=engine->getMaterialManager();
	mEngine=engine;
}

/// @todo: Calculate a boundingRadius for the Mesh
Resource::ptr MMSHHandler::load(Stream::ptr stream,const ResourceHandlerData *handlerData){
	DataStream::ptr dataStream(new DataStream(stream));

	byte signature[4];
	dataStream->read(signature,4);
	if(signature[0]!='T' || signature[1]!='M' || signature[2]!='M' || signature[3]!='H'){
		Error::unknown(Categories::TOADLET_TADPOLE,
			"Not of TMMSH format");
		return NULL;
	}

	uint8 version=dataStream->readUInt8();
	if(version!=3){
		Error::unknown(Categories::TOADLET_TADPOLE,
			"Not TMMH version 3");
		return NULL;
	}

	Mesh::ptr mesh(new Mesh());

	int i,j;

	int8 i8x=0,i8y=0,i8z=0,i8w=0;
	int16 i16x=0,i16y=0,i16z=0;

	while(dataStream->position()<dataStream->length()){
		uint8 block=dataStream->readUInt8();
		if(block==MESH_BLOCK){
			mesh->worldScale=MathConversion::fixedToScalar(dataStream->readBigInt32());

			uint16 numVertexes=dataStream->readBigUInt16();
			uint16 vertexType=dataStream->readBigUInt16();
			int formatBit=mEngine->getIdealVertexFormatBit();

			VertexFormat::ptr vertexFormat=mBufferManager->createVertexFormat();
			int positionElement=0;
			if((vertexType&VT_POSITION)>0){
				vertexFormat->addElement(VertexFormat::Semantic_POSITION,0,formatBit|VertexFormat::Format_BIT_COUNT_3);
			}
			int normalElement=positionElement;
			if((vertexType&VT_NORMAL)>0){
				normalElement++;
				vertexFormat->addElement(VertexFormat::Semantic_NORMAL,0,formatBit|VertexFormat::Format_BIT_COUNT_3);
			}
			int colorElement=normalElement;
			if((vertexType&VT_COLOR)>0){
				colorElement++;
				vertexFormat->addElement(VertexFormat::Semantic_COLOR,0,VertexFormat::Format_COLOR_RGBA);
			}
			int texCoordElement=colorElement;
			if((vertexType&VT_TEXCOORD1)>0){
				texCoordElement++;
				vertexFormat->addElement(VertexFormat::Semantic_TEX_COORD,0,formatBit|VertexFormat::Format_BIT_COUNT_2);
			}

			if((vertexType&VT_BONE)>0){
				mesh->vertexBoneAssignments.resize(numVertexes);
			}

			// HACK: Due to a bug in reading back vertexes from a hardware buffer in OGLES, we only load the static VertexBuffer of a Mesh if its not animated.
			// UPDATE: This actually isnt a bug I believe, but instead the fact that OGLES doesnt use map, and instead bufferSubData, which obviously cant get the previous data
			VertexBuffer::ptr vertexBuffer=mBufferManager->createVertexBuffer(Buffer::Usage_BIT_STATIC,Buffer::Access_BIT_WRITE,vertexFormat,numVertexes);

			VertexBufferAccessor vba;
			vba.lock(vertexBuffer,Buffer::Access_BIT_WRITE);

			if((vertexType&VT_POSITION)>0){
				uint8 bytes=dataStream->readUInt8();

				mathfixed::fixed bias[3];
				bias[0]=dataStream->readBigInt32();
				bias[1]=dataStream->readBigInt32();
				bias[2]=dataStream->readBigInt32();

				mathfixed::fixed scale=dataStream->readBigInt32();

				if(bytes==1){
					for(i=0;i<numVertexes;++i){
						i8x=dataStream->readInt8();i8y=dataStream->readInt8();i8z=dataStream->readInt8();
						vba.set3(i,positionElement,
							MathConversion::fixedToScalar(Math::mul(Math::intToFixed(i8x),scale)+bias[0]),
							MathConversion::fixedToScalar(Math::mul(Math::intToFixed(i8y),scale)+bias[1]),
							MathConversion::fixedToScalar(Math::mul(Math::intToFixed(i8z),scale)+bias[2]));
					}
				}
				else if(bytes==2){
					for(i=0;i<numVertexes;++i){
						i16x=dataStream->readBigInt16();i16y=dataStream->readBigInt16();i16z=dataStream->readBigInt16();
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
					i8x=dataStream->readInt8();i8y=dataStream->readInt8();i8z=dataStream->readInt8();
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
					uint8 r=dataStream->readUInt8();
					uint8 g=dataStream->readUInt8();
					uint8 b=dataStream->readUInt8();

					vba.setABGR(i,colorElement,0xFF000000 | (((int)b)<<16) | (((int)g)<<8) | (((int)r)<<0));
				}
			}

			if((vertexType&VT_BONE)>0){
				uint8 ui8;
				for(i=0;i<numVertexes;i++){
					ui8=dataStream->readUInt8();
					mesh->vertexBoneAssignments[i].add(Mesh::VertexBoneAssignment(ui8,Math::ONE));
				}
			}

			if((vertexType&VT_TEXCOORD1)>0){
				uint8 bytes=dataStream->readUInt8();

				mathfixed::fixed bias[2];
				bias[0]=dataStream->readBigInt32();
				bias[1]=dataStream->readBigInt32();

				mathfixed::fixed scale=dataStream->readBigInt32();

				if(bytes==1){
					for(i=0;i<numVertexes;++i){
						i8x=dataStream->readInt8();i8y=dataStream->readInt8();
						vba.set2(i,texCoordElement,
							MathConversion::fixedToScalar(Math::mul(Math::intToFixed(i8x),scale)+bias[0]),
							MathConversion::fixedToScalar(Math::mul(Math::intToFixed(i8y),scale)+bias[1]));
					}
				}
				else if(bytes==2){
					for(i=0;i<numVertexes;++i){
						i16x=dataStream->readBigInt16();i16y=dataStream->readBigInt16();
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

			uint8 numSubMeshes=dataStream->readUInt8();
			mesh->subMeshes.resize(numSubMeshes);

			for(i=0;i<numSubMeshes;++i){
				Mesh::SubMesh::ptr sub(new Mesh::SubMesh());
				mesh->subMeshes[i]=sub;

				// Material
				{
					Material::ptr material=mMaterialManager->createMaterial();
					material->retain();
					sub->material=material;

					material->setSaveLocally(true);

					uint8 faceCulling=dataStream->readUInt8();
					if(faceCulling>Renderer::FaceCulling_BACK){
						faceCulling=Renderer::FaceCulling_BACK;
					}
					material->setFaceCulling((Renderer::FaceCulling)faceCulling);

					uint8 hasTexture=dataStream->readUInt8();
					if(hasTexture>0){
						material->setTextureStage(0,TextureStage::ptr(new TextureStage()));
					}

					uint8 lighting=dataStream->readUInt8();
					material->setLighting(lighting>0);
					if(lighting>0){
						LightEffect lightEffect;
						uint32 color=0;

						// MMSH stored color in ARGB, now an outdated format
						color=dataStream->readBigUInt32();
						lightEffect.ambient=Color::rgba(((color&0xFF000000)>>24)|((color&0x00FFFFFF)<<8));
						color=dataStream->readBigUInt32();
						lightEffect.diffuse=Color::rgba(((color&0xFF000000)>>24)|((color&0x00FFFFFF)<<8));
						color=dataStream->readBigUInt32();
						lightEffect.specular=Color::rgba(((color&0xFF000000)>>24)|((color&0x00FFFFFF)<<8));
						lightEffect.shininess=MathConversion::fixedToScalar(dataStream->readBigInt32());
						color=dataStream->readBigUInt32();
						lightEffect.emissive=Color::rgba(((color&0xFF000000)>>24)|((color&0x00FFFFFF)<<8));

						material->setLightEffect(lightEffect);
					}
				}

				uint16 numIndexes=dataStream->readBigUInt16();
				uint16 *indexes=new uint16[numIndexes];

				for(j=0;j<numIndexes;++j){
					indexes[j]=dataStream->readBigUInt16();
				}
				
				uint16 numStrips=dataStream->readBigUInt16();

				uint16 *stripLengths=new uint16[numStrips];

				int newNumIndexes=0;
				for(j=0;j<numStrips;++j){
					stripLengths[j]=dataStream->readBigUInt16();
					newNumIndexes+=(stripLengths[j]-2)*3;
				}

				IndexBuffer::ptr indexBuffer=mBufferManager->createIndexBuffer(Buffer::Usage_BIT_STATIC,Buffer::Access_BIT_WRITE,IndexBuffer::IndexFormat_UINT_16,newNumIndexes);
				{
					uint16 *indexData=(uint16*)indexBuffer->lock(Buffer::Access_BIT_WRITE);
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

			int numBones=dataStream->readUInt8();
			skeleton->bones.resize(numBones);

			uint8 translateBytes=dataStream->readUInt8();

			mathfixed::fixed translateBias[3];
			translateBias[0]=dataStream->readBigInt32();
			translateBias[1]=dataStream->readBigInt32();
			translateBias[2]=dataStream->readBigInt32();

			mathfixed::fixed translateScale=dataStream->readBigInt32();

			for(i=0;i<numBones;++i){
				Skeleton::Bone::ptr bone(new Skeleton::Bone());
				skeleton->bones[i]=bone;
				bone->index=i;

				if(translateBytes==1){
					i8x=dataStream->readInt8();i8y=dataStream->readInt8();i8z=dataStream->readInt8();
					bone->translate.set(
						MathConversion::fixedToScalar(Math::mul(Math::intToFixed(i8x),translateScale)+translateBias[0]),
						MathConversion::fixedToScalar(Math::mul(Math::intToFixed(i8y),translateScale)+translateBias[1]),
						MathConversion::fixedToScalar(Math::mul(Math::intToFixed(i8z),translateScale)+translateBias[2]));
				}
				else{
					i16x=dataStream->readBigInt16();i16y=dataStream->readBigInt16();i16z=dataStream->readBigInt16();
					bone->translate.set(
						MathConversion::fixedToScalar(Math::mul(Math::intToFixed(i16x),translateScale)+translateBias[0]),
						MathConversion::fixedToScalar(Math::mul(Math::intToFixed(i16y),translateScale)+translateBias[1]),
						MathConversion::fixedToScalar(Math::mul(Math::intToFixed(i16z),translateScale)+translateBias[2]));
				}

				i8x=dataStream->readInt8();i8y=dataStream->readInt8();i8z=dataStream->readInt8();i8w=dataStream->readInt8();
				bone->rotate.set(
					MathConversion::fixedToScalar(((int)i8x)<<9),
					MathConversion::fixedToScalar(((int)i8y)<<9),
					MathConversion::fixedToScalar(((int)i8z)<<9),
					MathConversion::fixedToScalar(((int)i8w)<<9));
				tadpole::Math::normalizeCarefully(bone->rotate,0);

				if(translateBytes==1){
					i8x=dataStream->readInt8();i8y=dataStream->readInt8();i8z=dataStream->readInt8();
					bone->worldToBoneTranslate.set(
						MathConversion::fixedToScalar(Math::mul(Math::intToFixed(i8x),translateScale)+translateBias[0]),
						MathConversion::fixedToScalar(Math::mul(Math::intToFixed(i8y),translateScale)+translateBias[1]),
						MathConversion::fixedToScalar(Math::mul(Math::intToFixed(i8z),translateScale)+translateBias[2]));
				}
				else{
					i16x=dataStream->readBigInt16();i16y=dataStream->readBigInt16();i16z=dataStream->readBigInt16();
					bone->worldToBoneTranslate.set(
						MathConversion::fixedToScalar(Math::mul(Math::intToFixed(i16x),translateScale)+translateBias[0]),
						MathConversion::fixedToScalar(Math::mul(Math::intToFixed(i16y),translateScale)+translateBias[1]),
						MathConversion::fixedToScalar(Math::mul(Math::intToFixed(i16z),translateScale)+translateBias[2]));
				}

				i8x=dataStream->readInt8();i8y=dataStream->readInt8();i8z=dataStream->readInt8();i8w=dataStream->readInt8();
				bone->worldToBoneRotate.set(
					MathConversion::fixedToScalar(((int)i8x)<<9),
					MathConversion::fixedToScalar(((int)i8y)<<9),
					MathConversion::fixedToScalar(((int)i8z)<<9),
					MathConversion::fixedToScalar(((int)i8w)<<9));
				tadpole::Math::normalizeCarefully(bone->worldToBoneRotate,0);

				bone->parentIndex=dataStream->readInt8();
			}

			skeleton->compile();

			mesh->skeleton=skeleton;
		}
		else if(block==ANIMATION_BLOCK){
			Sequence::ptr sequence(new Sequence());

			scalar length=MathConversion::fixedToScalar(dataStream->readBigInt32());

			uint8 translateBytes=dataStream->readUInt8();

			mathfixed::fixed translateBias[3];
			translateBias[0]=dataStream->readBigInt32();
			translateBias[1]=dataStream->readBigInt32();
			translateBias[2]=dataStream->readBigInt32();

			mathfixed::fixed translateScale=dataStream->readBigInt32();

			uint8 numTracks=dataStream->readUInt8();

			sequence->tracks.resize(mesh->skeleton->bones.size());

			for(i=0;i<numTracks;++i){
				Track::ptr track(new Track());

				track->length=length;

				track->index=dataStream->readUInt8();

				sequence->tracks[i]=track;

				uint16 numKeyFrames=dataStream->readBigUInt16();
				track->keyFrames.resize(numKeyFrames);

				for(j=0;j<numKeyFrames;++j){
					KeyFrame &keyFrame=track->keyFrames[j];

					keyFrame.time=MathConversion::fixedToScalar(dataStream->readBigInt32());

					if(translateBytes==1){
						i8x=dataStream->readInt8();i8y=dataStream->readInt8();i8z=dataStream->readInt8();
						keyFrame.translate.set(
							MathConversion::fixedToScalar(Math::mul(Math::intToFixed(i8x),translateScale)+translateBias[0]),
							MathConversion::fixedToScalar(Math::mul(Math::intToFixed(i8y),translateScale)+translateBias[1]),
							MathConversion::fixedToScalar(Math::mul(Math::intToFixed(i8z),translateScale)+translateBias[2]));
					}
					else{
						i16x=dataStream->readBigInt16();i16y=dataStream->readBigInt16();i16z=dataStream->readBigInt16();
						keyFrame.translate.set(
							MathConversion::fixedToScalar(Math::mul(Math::intToFixed(i16x),translateScale)+translateBias[0]),
							MathConversion::fixedToScalar(Math::mul(Math::intToFixed(i16y),translateScale)+translateBias[1]),
							MathConversion::fixedToScalar(Math::mul(Math::intToFixed(i16z),translateScale)+translateBias[2]));
					}

					i8x=dataStream->readInt8();i8y=dataStream->readInt8();i8z=dataStream->readInt8();i8w=dataStream->readInt8();
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


