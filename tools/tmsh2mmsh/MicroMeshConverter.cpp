#include "MicroMeshConverter.h"
#include <toadlet/egg/Logger.h>
#include <toadlet/egg/Error.h>
#include <toadlet/egg/Extents.h>
#include <toadlet/egg/io/DataStream.h>
#include <toadlet/peeper/VertexBufferAccessor.h>
#include <toadlet/peeper/IndexBufferAccessor.h>
#include <toadlet/tadpole/Material.h>
#include "../shared/VertexCompression.h"

using namespace toadlet;
using namespace toadlet::egg;
using namespace toadlet::egg::io;
using namespace toadlet::egg::math;
using namespace toadlet::egg::math::Math;
using namespace toadlet::peeper;
using namespace toadlet::tadpole;
using namespace toadlet::tadpole::mesh;

const static float MAXIMUM_ACCEPTABLE_SCALE=100.0f;
const static float MINIMUM_ACCEPTABLE_SCALE=0.01f;

MicroMeshConverter::MicroMeshConverter(){
	mTC=actcNew();
	actcParami(mTC,ACTC_OUT_MIN_FAN_VERTS,INT_MAX);
	actcParami(mTC,ACTC_OUT_HONOR_WINDING,ACTC_TRUE);
}

MicroMeshConverter::~MicroMeshConverter(){
	actcDelete(mTC);
}

bool MicroMeshConverter::convertMesh(Mesh *mesh,Stream *outStream,float meshScale,int forceBytes,bool invertYTexCoord){
	int i,j,k;

	if(meshScale==0){
		Logger::alert("Using a scale of 0!");
	}

	DataStream::ptr out(new DataStream(outStream));

	// Header
	out->write("TMMH",4);

	// Version 1 - Initial version
	// Version 2 - Added skeleton information
	// Version 3 - Fixed rotations
	out->writeUInt8(3);

	float scaleModifier=1.0f;
	if(meshScale<MINIMUM_ACCEPTABLE_SCALE){
		scaleModifier=meshScale/MINIMUM_ACCEPTABLE_SCALE;
		meshScale=MINIMUM_ACCEPTABLE_SCALE;
		Logger::alert("Scale below minimum acceptable scale, prescaling mesh");
	}
	else if(meshScale>MAXIMUM_ACCEPTABLE_SCALE){
		scaleModifier=meshScale/MAXIMUM_ACCEPTABLE_SCALE;
		meshScale=MAXIMUM_ACCEPTABLE_SCALE;
		Logger::alert("Scale above maximum acceptable scale, prescaling mesh");
	}

	// Mesh Block
	out->writeUInt8((char)MESH_BLOCK);
	{
		VertexData::ptr vertexData=mesh->staticVertexData;
		VertexBuffer::ptr vertexBuffer=vertexData->getVertexBuffer(0);
		VertexFormat::ptr vertexFormat=vertexBuffer->getVertexFormat();
		int positionIndex=vertexFormat->getVertexElementIndexOfType(VertexElement::Type_POSITION);
		int positionOffset=positionIndex>=0?vertexFormat->getVertexElement(positionIndex).offset:0;
		int normalIndex=vertexFormat->getVertexElementIndexOfType(VertexElement::Type_NORMAL);
		int colorIndex=vertexFormat->getVertexElementIndexOfType(VertexElement::Type_COLOR_DIFFUSE);
		int texCoordIndex=vertexFormat->getVertexElementIndexOfType(VertexElement::Type_TEX_COORD);
		int texCoordOffset=texCoordIndex>=0?vertexFormat->getVertexElement(texCoordIndex).offset:0;
		VertexBufferAccessor vba(vertexBuffer);

		if(scaleModifier!=1.0f){
			for(i=0;i<vba.getSize();++i){
				Vector3 position;
				vba.get3(i,positionIndex,position);
				position*=scaleModifier;
				vba.set3(i,positionIndex,position);
			}
		}

		out->writeBigInt32(mathfixed::Math::floatToFixed(meshScale));

		out->writeBigUInt16(vba.getSize());

		short outType=0;
		if(positionIndex>=0){
			outType|=VT_POSITION;
		}
		if(normalIndex>=0){
			outType|=VT_NORMAL;
		}
		if(colorIndex>=0){
			outType|=VT_COLOR;
		}
		if(texCoordIndex>=0){
			outType|=VT_TEXCOORD1;
		}
		if(mesh->vertexBoneAssignments.size()>0){
			outType|=VT_BONE;
		}

		out->writeBigUInt16(outType);

		if(outType&VT_POSITION){
			int bytes=forceBytes;
			if(bytes==0){
				bytes=VertexCompression::getSuggestedArrayBytes(3,vba.getSize(),
					(float*)(vba.getData()+positionOffset),vertexFormat->getVertexSize());
				Logger::alert(String("Position bytes:") + bytes);
			}

			if(bytes==1){
				out->writeUInt8(1); // bytes

				Vector3 bias,scale;

				VertexCompression::calculateArrayBiasAndScale(3,vba.getSize(),
					(float*)(vba.getData()+positionOffset),vertexFormat->getVertexSize(),
					(float*)&bias,(float*)&scale,127);

				out->writeBigInt32(mathfixed::Math::floatToFixed(bias.x));
				out->writeBigInt32(mathfixed::Math::floatToFixed(bias.y));
				out->writeBigInt32(mathfixed::Math::floatToFixed(bias.z));

				float scaleScalar=maxVal(maxVal(scale[0],scale[1]),scale[2]);
				out->writeBigInt32(mathfixed::Math::floatToFixed(scaleScalar));

				for(i=0;i<vba.getSize();++i){
					Vector3 position;
					vba.get3(i,positionIndex,position);

					for(j=0;j<3;++j){
						int8 value=((position[j]-bias[j])/scaleScalar);
						out->writeInt8(value);
					}
				}
			}
			else{ // Otherwise we just use shorts
				out->writeUInt8(2); // shorts

				Vector3 bias,scale;

				VertexCompression::calculateArrayBiasAndScale(3,vba.getSize(),
					(float*)(vba.getData()+positionOffset),vertexFormat->getVertexSize(),
					(float*)&bias,(float*)&scale,32767);

				out->writeBigInt32(mathfixed::Math::floatToFixed(bias.x));
				out->writeBigInt32(mathfixed::Math::floatToFixed(bias.y));
				out->writeBigInt32(mathfixed::Math::floatToFixed(bias.z));

				float scaleScalar=maxVal(maxVal(scale[0],scale[1]),scale[2]);
				out->writeBigInt32(mathfixed::Math::floatToFixed(scaleScalar));

				for(i=0;i<vba.getSize();++i){
					Vector3 position;
					vba.get3(i,positionIndex,position);

					for(j=0;j<3;++j){
						int16 value=(position[j]-bias[j])/scaleScalar;
						out->writeBigInt16(value);
					}
				}
			}
		}

		if(outType&VT_NORMAL){
			for(i=0;i<vba.getSize();++i){
				Vector3 normal;
				vba.get3(i,normalIndex,normal);
				char x=normal.x*127;
				char y=normal.y*127;
				char z=normal.z*127;
				out->writeInt8(x);
				out->writeInt8(y);
				out->writeInt8(z);
			}
		}

		if(outType&VT_COLOR){
			for(i=0;i<vba.getSize();++i){
				Color color;
				color.setRGBA(vba.getRGBA(i,colorIndex));
				unsigned char r=color.r*255;
				unsigned char g=color.g*255;
				unsigned char b=color.b*255;
				out->writeUInt8(r);
				out->writeUInt8(g);
				out->writeUInt8(b);
			}
		}

		if(outType&VT_BONE){
			for(i=0;i<vba.getSize();++i){
				const Mesh::VertexBoneAssignmentList &vbaList=mesh->vertexBoneAssignments[i];
				int bone=0;
				if(vbaList.size()>0){
					int best=0;
					for(j=1;j<vbaList.size();++j){
						if(vbaList[j].weight>vbaList[best].weight){
							best=j;
						}
					}
					bone=vbaList[best].bone;
				}

				if(bone>Extents::MAX_UINT8){
					Error::unknown("Max of 255 bones");
				}
				out->writeUInt8(bone);
			}
		}

		if(outType&VT_TEXCOORD1){
			int bytes=forceBytes;
			if(bytes==0){
				bytes=VertexCompression::getSuggestedArrayBytes(2,vba.getSize(),
					(float*)(vba.getData()+texCoordOffset),vertexFormat->getVertexSize());
				Logger::alert(String("TexCoord bytes:") + bytes);
			}

			if(invertYTexCoord){
				for(i=0;i<vba.getSize();++i){
					Vector2 texCoord;
					vba.get2(i,texCoordIndex,texCoord);
					texCoord.y=1.0f-texCoord.y;
					vba.set2(i,texCoordIndex,texCoord);
				}
			}

			if(bytes==1){
				out->writeUInt8(1); // bytes

				Vector3 bias,scale;

				VertexCompression::calculateArrayBiasAndScale(2,vba.getSize(),
					(float*)(vba.getData()+texCoordOffset),vertexFormat->getVertexSize(),
					(float*)&bias,(float*)&scale,127);

				out->writeBigInt32(mathfixed::Math::floatToFixed(bias.x));
				out->writeBigInt32(mathfixed::Math::floatToFixed(bias.y));

				float scaleScalar=maxVal(scale[0],scale[1]);
				out->writeBigInt32(mathfixed::Math::floatToFixed(scaleScalar));

				for(i=0;i<vba.getSize();++i){
					Vector2 texCoord;
					vba.get2(i,texCoordIndex,texCoord);

					out->writeInt8((texCoord.x-bias[0])/scaleScalar);
					out->writeInt8((texCoord.y-bias[1])/scaleScalar);
				}
			}
			else{ // Otherwise we just use shorts
				out->writeUInt8(2); // shorts

				Vector3 bias,scale;

				VertexCompression::calculateArrayBiasAndScale(2,vba.getSize(),
					(float*)(vba.getData()+texCoordOffset),vertexFormat->getVertexSize(),
					(float*)&bias,(float*)&scale,32767);

				out->writeBigInt32(mathfixed::Math::floatToFixed(bias.x));
				out->writeBigInt32(mathfixed::Math::floatToFixed(bias.y));

				float scaleScalar=maxVal(scale[0],scale[1]);
				out->writeBigInt32(mathfixed::Math::floatToFixed(scaleScalar));

				for(i=0;i<vba.getSize();++i){
					Vector2 texCoord;
					vba.get2(i,texCoordIndex,texCoord);

					out->writeBigInt16((texCoord.x-bias[0])/scaleScalar);
					out->writeBigInt16((texCoord.y-bias[1])/scaleScalar);
				}
			}
		}

		out->writeUInt8(mesh->subMeshes.size());

		for(i=0;i<mesh->subMeshes.size();++i){
			int j,k;
			const Mesh::SubMesh::ptr &sub=mesh->subMeshes[i];

			Material::ptr material=sub->material;
			if(material!=NULL){
				out->writeUInt8(material->getFaceCulling());
				out->writeUInt8(material->getNumTextureStages()>0);
				out->writeUInt8(material->getLighting());
				if(material->getLighting()){
					const LightEffect &le=material->getLightEffect();
					out->writeBigUInt32(le.ambient.getABGR());
					out->writeBigUInt32(le.diffuse.getABGR());
					out->writeBigUInt32(le.specular.getABGR());
					out->writeBigInt32(mathfixed::Math::floatToFixed(le.shininess));
					out->writeBigUInt32(le.emissive.getABGR());
				}
			}
			else{
				out->writeUInt8(0);
				out->writeUInt8(0); // Has a texture
				out->writeUInt8(0);
			}

			actcMakeEmpty(mTC);

			actcBeginInput(mTC);
			const IndexBuffer::ptr &indexBuffer=sub->indexData->getIndexBuffer();
			TOADLET_ASSERT(indexBuffer->getIndexFormat()==IndexBuffer::IndexFormat_UINT_16);
			IndexBufferAccessor iba(indexBuffer);
			for(j=0;j<iba.getSize()/3;++j){
				actcAddTriangle(mTC,iba.get(j*3+0),iba.get(j*3+1),iba.get(j*3+2));
			}
			actcEndInput(mTC);

			int prim=0;
			unsigned int v1=0,v2=0,v3=0;
			Collection<Collection<int> > strips;
			int numIndexes=0;

			actcBeginOutput(mTC);
			while((prim=actcStartNextPrim(mTC,&v1,&v2))!=ACTC_DATABASE_EMPTY){
				strips.add(Collection<int>());
				Collection<int> &strip=strips[strips.size()-1];

				strip.add(v1);
				strip.add(v2);
				numIndexes+=2;

				while(actcGetNextVert(mTC,&v3)!=ACTC_PRIM_COMPLETE){
					strip.add(v3);
					numIndexes++;
				}
			}
			actcEndOutput(mTC);

			out->writeBigUInt16(numIndexes);

			for(j=0;j<strips.size();++j){
				Collection<int> &strip=strips[j];
				for(k=0;k<strip.size();++k){
					out->writeBigUInt16(strip[k]);
				}
			}

			Logger::alert(String("Number of strips:") + strips.size());

			out->writeBigUInt16(strips.size());

			String string("Strip lengths:");
			for(j=0;j<strips.size();++j){
				string=string + strips[j].size() + ",";

				out->writeBigUInt16(strips[j].size());
			}
			Logger::alert(string);
		}
	}
	if(mesh->skeleton!=NULL){
		Skeleton::ptr skeleton=mesh->skeleton;

		out->writeUInt8((char)SKELETON_BLOCK);

		if(skeleton->bones.size()>Extents::MAX_UINT8){
			Error::unknown("Max of 255 bones");
		}

		out->writeUInt8(skeleton->bones.size());

		Collection<Vector3> translates;
		Collection<Quaternion> rotates;

		for(i=0;i<skeleton->bones.size();++i){
			translates.add(skeleton->bones[i]->translate * scaleModifier);
			translates.add(skeleton->bones[i]->worldToBoneTranslate * scaleModifier);
			rotates.add(skeleton->bones[i]->rotate);
			rotates.add(skeleton->bones[i]->worldToBoneRotate);
		}

		// Translates
		Vector3 translateBias,translateScale;

		int translateBytes=forceBytes;
		if(translateBytes==0){
			translateBytes=VertexCompression::getSuggestedArrayBytes(3,translates.size(),(float*)&translates[0],sizeof(Vector3));
			Logger::alert(String("Skeleton Translate bytes:") + translateBytes);
		}

		if(translateBytes==1){
			VertexCompression::calculateArrayBiasAndScale(3,translates.size(),
				(float*)&translates[0],sizeof(Vector3),
				(float*)&translateBias,(float*)&translateScale,127);
		}
		else{
			translateBytes=2;
			VertexCompression::calculateArrayBiasAndScale(3,translates.size(),
				(float*)&translates[0],sizeof(Vector3),
				(float*)&translateBias,(float*)&translateScale,32767);
		}

		out->writeUInt8(translateBytes);

		out->writeBigInt32(mathfixed::Math::floatToFixed(translateBias.x));
		out->writeBigInt32(mathfixed::Math::floatToFixed(translateBias.y));
		out->writeBigInt32(mathfixed::Math::floatToFixed(translateBias.z));

		float translateScaleScalar=maxVal(maxVal(translateScale.x,translateScale.y),translateScale.z);

		out->writeBigInt32(mathfixed::Math::floatToFixed(translateScaleScalar));

		for(i=0;i<skeleton->bones.size();++i){
			Skeleton::Bone::ptr b=skeleton->bones[i];

			if(translateBytes==1){
				out->writeInt8((b->translate.x * scaleModifier - translateBias.x)/translateScaleScalar);
				out->writeInt8((b->translate.y * scaleModifier - translateBias.y)/translateScaleScalar);
				out->writeInt8((b->translate.z * scaleModifier - translateBias.z)/translateScaleScalar);
			}
			else{
				out->writeBigInt16((b->translate.x * scaleModifier - translateBias.x)/translateScaleScalar);
				out->writeBigInt16((b->translate.y * scaleModifier - translateBias.y)/translateScaleScalar);
				out->writeBigInt16((b->translate.z * scaleModifier - translateBias.z)/translateScaleScalar);
			}

			out->writeInt8(b->rotate.x*127);
			out->writeInt8(b->rotate.y*127);
			out->writeInt8(b->rotate.z*127);
			out->writeInt8(b->rotate.w*127);

			if(translateBytes==1){
				out->writeInt8((b->worldToBoneTranslate.x * scaleModifier - translateBias.x)/translateScaleScalar);
				out->writeInt8((b->worldToBoneTranslate.y * scaleModifier - translateBias.y)/translateScaleScalar);
				out->writeInt8((b->worldToBoneTranslate.z * scaleModifier - translateBias.z)/translateScaleScalar);
			}
			else{
				out->writeBigInt16((b->worldToBoneTranslate.x * scaleModifier- translateBias.x)/translateScaleScalar);
				out->writeBigInt16((b->worldToBoneTranslate.y * scaleModifier- translateBias.y)/translateScaleScalar);
				out->writeBigInt16((b->worldToBoneTranslate.z * scaleModifier - translateBias.z)/translateScaleScalar);
			}

			out->writeInt8(b->worldToBoneRotate.x*127);
			out->writeInt8(b->worldToBoneRotate.y*127);
			out->writeInt8(b->worldToBoneRotate.z*127);
			out->writeInt8(b->worldToBoneRotate.w*127);

			out->writeUInt8(b->parentIndex);
		}

		for(i=0;i<skeleton->sequences.size();++i){
			Sequence::ptr sequence=skeleton->sequences[i];

			out->writeUInt8((char)ANIMATION_BLOCK);

			out->writeBigInt32(sequence->length);

			Collection<Vector3> translates;
			Collection<Quaternion> rotates;

			for(j=0;j<sequence->tracks.size();++j){
				Track::ptr &track=sequence->tracks[j];

				for(k=0;k<track->keyFrames.size();++k){
					KeyFrame &keyFrame=track->keyFrames[k];

					translates.add(keyFrame.translate * scaleModifier);
					rotates.add(keyFrame.rotate);
				}
			}

			// Translates
			Vector3 translateBias,translateScale;

			int translateBytes=forceBytes;
			if(translateBytes==0){
				translateBytes=VertexCompression::getSuggestedArrayBytes(3,translates.size(),(float*)&translates[0],sizeof(Vector3));
				Logger::alert(String("Animation Translate bytes:") + translateBytes);
			}

			if(translateBytes==1){
				VertexCompression::calculateArrayBiasAndScale(3,translates.size(),
					(float*)&translates[0],sizeof(Vector3),
					(float*)&translateBias,(float*)&translateScale,127);
			}
			else{
				translateBytes=2;
				VertexCompression::calculateArrayBiasAndScale(3,translates.size(),
					(float*)&translates[0],sizeof(Vector3),
					(float*)&translateBias,(float*)&translateScale,32767);
			}

			out->writeUInt8(translateBytes);

			out->writeBigInt32(mathfixed::Math::floatToFixed(translateBias.x));
			out->writeBigInt32(mathfixed::Math::floatToFixed(translateBias.y));
			out->writeBigInt32(mathfixed::Math::floatToFixed(translateBias.z));

			float translateScaleScalar=maxVal(maxVal(translateScale.x,translateScale.y),translateScale.z);

			out->writeBigInt32(mathfixed::Math::floatToFixed(translateScaleScalar));

			out->writeUInt8(sequence->tracks.size());

			for(j=0;j<sequence->tracks.size();++j){
				Track::ptr track=sequence->tracks[j];

				out->writeUInt8(track->index);

				if(track->keyFrames.size()>Extents::MAX_UINT16){
					Error::unknown("Max of 65535 keyFrames");
				}

				out->writeBigUInt16(track->keyFrames.size());

				for(k=0;k<track->keyFrames.size();++k){
					const KeyFrame &keyFrame=track->keyFrames[k];

					out->writeBigInt32(mathfixed::Math::floatToFixed(keyFrame.time));

					if(translateBytes==1){
						out->writeInt8((keyFrame.translate.x * scaleModifier - translateBias.x)/translateScaleScalar);
						out->writeInt8((keyFrame.translate.y * scaleModifier - translateBias.y)/translateScaleScalar);
						out->writeInt8((keyFrame.translate.z * scaleModifier - translateBias.z)/translateScaleScalar);
					}
					else{
						out->writeBigInt16((keyFrame.translate.x * scaleModifier - translateBias.x)/translateScaleScalar);
						out->writeBigInt16((keyFrame.translate.y * scaleModifier - translateBias.y)/translateScaleScalar);
						out->writeBigInt16((keyFrame.translate.z * scaleModifier - translateBias.z)/translateScaleScalar);
					}

					out->writeInt8(keyFrame.rotate.x*127);
					out->writeInt8(keyFrame.rotate.y*127);
					out->writeInt8(keyFrame.rotate.z*127);
					out->writeInt8(keyFrame.rotate.w*127);
				}
			}
		}
	}

	if(Error::getInstance()->getError()!=Error::Type_NONE){
		return false;
	}
	else{
		return true;
	}
}
