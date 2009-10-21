#include "SMDConverter.h"
#include <toadlet/egg/MathConversion.h>
#include <toadlet/egg/Logger.h>
#include <toadlet/egg/Error.h>
#include <toadlet/peeper/IndexBufferAccessor.h>
#include <toadlet/peeper/VertexBufferAccessor.h>
#include <toadlet/tadpole/entity/MeshEntitySkeleton.h>
#include <stdio.h>

using namespace toadlet;
using namespace toadlet::egg;
using namespace toadlet::egg::io;
using namespace toadlet::egg::MathConversion;
using namespace toadlet::peeper;
using namespace toadlet::tadpole;
using namespace toadlet::tadpole::entity;

class Vertex{
public:
	Vertex(){}

	Vector3 position;
	Vector3 normal;
	Vector2 texCoord;
	int bone;
};

class Triangle{
public:
	Triangle(){}

	Triangle(String m,Vertex v1,Vertex v2,Vertex v3){
		material=m;
		verts[0]=v1;
		verts[1]=v2;
		verts[2]=v3;
	}

	String material;
	Vertex verts[3];
};

SMDConverter::SMDConverter():
	mPositionEpsilon(Math::fromMilli(10)),
	mNormalEpsilon(Math::fromMilli(10)),
	mTexCoordEpsilon(Math::fromMilli(10)),
	mRemoveSkeleton(false),
	mInvertFaces(false)
{}

void SMDConverter::load(toadlet::egg::io::InputStream *in){
	int block=Block_NONE;
	int version=0;
	bool reference=false;
	int time=0;
	String material;
	int vertindex=0;
	Vertex triverts[3];
	Collection<Triangle> triangles;

	if(mMesh==NULL){
		mMesh=Mesh::ptr(new Mesh());
	}

	String s;
	while((s=readLine(in)).length()>0){
		if(block==Block_NONE){
			if(s.startsWith("version")){
				sscanf(s.c_str(),"version %d",&version);
				if(version!=1){
					Error::unknown("unknown smd version");
					return;
				}
			}
			else if(s.startsWith("nodes")){
				block=Block_NODES;
			}
			else if(s.startsWith("skeleton")){
				block=Block_SKELETON;
				if(reference==false && mSkeleton!=NULL){
					mSequence=MeshSkeletonSequence::ptr(new MeshSkeletonSequence());
					mSkeleton->sequences.add(mSequence);

					for(int i=0;i<mSkeleton->bones.size();++i){
						mSequence->tracks.add(Track::ptr(new Track()));
					}
				}
			}
			else if(s.startsWith("triangles")){
				block=Block_TRIANGLES;
			}
		}
		else if(block==Block_NODES){
			if(s.equals("end")){
				// Bone sanity check
				if(reference){
					int i;
					for(i=0;i<mSkeleton->bones.size();++i){
						if(mSkeleton->bones[i]==NULL){
							Error::unknown("invalid skeleton");
							return;
						}
					}
				}

				block=Block_NONE;
			}
			else{
				int id=0;
				char name[1024]="";
				int pid=0;

				int amt=sscanf(s.c_str(),"%d \"%[^\"]\" %d",&id,name,&pid);
				if(amt<3){
					sscanf(s.c_str(),"%d %s %d",&id,name,&pid);
				}

				if(mSkeleton==NULL){
					mSkeleton=MeshSkeleton::ptr(new MeshSkeleton());
					mMesh->skeleton=mSkeleton;
					reference=true;
				}

				if(reference && mSkeleton!=NULL){
					MeshSkeleton::Bone::ptr bone(new MeshSkeleton::Bone());
					bone->index=id;
					bone->parentIndex=pid;
					bone->name=name;

					if(mSkeleton->bones.size()<=bone->index){
						mSkeleton->bones.resize(bone->index+1);
					}
					mSkeleton->bones.setAt(bone->index,bone);
				}
			}
		}
		else if(block==Block_SKELETON){
			if(s.equals("end")){
				block=Block_NONE;
			}
			else if(s.startsWith("time")){
				sscanf(s.c_str(),"time %d",&time);
			}
			else{
				int id=0;
				float px=0,py=0,pz=0;
				float rx=0,ry=0,rz=0;
				sscanf(s.c_str(),"%d %f %f %f %f %f %f",&id,&px,&py,&pz,&rx,&ry,&rz);

				if(mSkeleton!=NULL){
					if(reference){
						MeshSkeleton::Bone::ptr bone=mSkeleton->bones.at(id);
						bone->translate.set(floatToScalar(px),floatToScalar(py),floatToScalar(pz));
						setQuaternionFromXYZ(bone->rotate,floatToScalar(rx),floatToScalar(ry),floatToScalar(rz));
					}
					else{
						Track::ptr track=mSequence->tracks.at(id);

						KeyFrame keyFrame;
						keyFrame.time=Math::fromInt(time);
						keyFrame.translate.set(floatToScalar(px),floatToScalar(py),floatToScalar(pz));
						setQuaternionFromXYZ(keyFrame.rotate,floatToScalar(rx),floatToScalar(ry),floatToScalar(rz));

						track->keyFrames.add(keyFrame);
					}
				}
			}
		}
		else if(block==Block_TRIANGLES){
			if(s.equals("end")){
				block=Block_NONE;
			}
			else if(vertindex==0){
				material=s;
				vertindex=1;
			}
			else{
				int bone=0;
				float px=0,py=0,pz=0;
				float nx=0,ny=0,nz=0;
				float tu=0,tv=0;

				sscanf(s.c_str(),"%d %f %f %f %f %f %f %f %f",&bone,&px,&py,&pz,&nx,&ny,&nz,&tu,&tv);

				Vertex v;
				v.bone=bone;
				v.position.set(floatToScalar(px),floatToScalar(py),floatToScalar(pz));
				v.normal.set(floatToScalar(nx),floatToScalar(ny),floatToScalar(nz));
				v.texCoord.set(floatToScalar(tu),floatToScalar(tv));
				triverts[vertindex-1]=v;

				if(++vertindex>3){
					vertindex=0;

					triangles.add(Triangle(material,triverts[0],triverts[1],triverts[2]));
				}
			}
		}
	}

	if(reference){
		int i,j,k;

		if(mRemoveSkeleton==false && mSkeleton!=NULL){
			MeshEntitySkeleton::ptr skeleton(new MeshEntitySkeleton(NULL,mSkeleton));
			skeleton->updateBones();
			for(i=0;i<skeleton->getNumBones();++i){
				Vector3 wtbTranslation(skeleton->getBone(i)->worldTranslate);
				Math::neg(wtbTranslation);

				Quaternion wtbRotation(skeleton->getBone(i)->worldRotate);
				Math::conjugate(wtbRotation);
				Matrix3x3 wtbRotationMatrix;
				Math::setMatrix3x3FromQuaternion(wtbRotationMatrix,wtbRotation);

				Math::mul(wtbTranslation,wtbRotationMatrix);

				mSkeleton->bones[i]->worldToBoneTranslate.set(wtbTranslation);
				mSkeleton->bones[i]->worldToBoneRotate.set(wtbRotation);
			}
		}
		else{
			mSkeleton=NULL;
			mMesh->skeleton=NULL;
		}

		Collection<Vertex> verts;
		Map<String,Collection<int> > materialIndexLists;
		for(i=0;i<triangles.size();++i){
			for(j=0;j<3;++j){
				for(k=0;k<verts.size();++k){
					if(vertsEqual(verts.at(k),triangles.at(i).verts[j])){
						materialIndexLists[triangles.at(i).material].add(k);
						break;
					}
				}
				if(k==verts.size()){
					verts.add(triangles.at(i).verts[j]);
					materialIndexLists[triangles.at(i).material].add(k);
				}
			}
		}

		VertexFormat::ptr vertexFormat(new VertexFormat(3));
		vertexFormat->addVertexElement(VertexElement(VertexElement::Type_POSITION,VertexElement::Format_BIT_FLOAT_32|VertexElement::Format_BIT_COUNT_3));
		vertexFormat->addVertexElement(VertexElement(VertexElement::Type_NORMAL,VertexElement::Format_BIT_FLOAT_32|VertexElement::Format_BIT_COUNT_3));
		vertexFormat->addVertexElement(VertexElement(VertexElement::Type_TEX_COORD,VertexElement::Format_BIT_FLOAT_32|VertexElement::Format_BIT_COUNT_2));
		VertexBuffer::ptr vertexBuffer(new VertexBuffer(Buffer::UsageType_STATIC,Buffer::AccessType_WRITE_ONLY,vertexFormat,verts.size()));
		{
			VertexBufferAccessor vba(vertexBuffer);
			for(i=0;i<verts.size();++i){
				Vertex &v=verts[i];
				vba.set3(i,0,v.position);
				vba.set3(i,1,v.normal);
				vba.set2(i,2,v.texCoord);
			}
		}
		mMesh->staticVertexData=VertexData::ptr(new VertexData(vertexBuffer));

		if(mSkeleton!=NULL){
			mMesh->vertexBoneAssignments.resize(verts.size());
			for(i=0;i<verts.size();++i){
				Mesh::VertexBoneAssignmentList list;
				list.add(Mesh::VertexBoneAssignment(verts[i].bone,Math::ONE));
				mMesh->vertexBoneAssignments.setAt(i,list);
			}
		}

		for(j=0;j<materialIndexLists.size();++j){
			const String &materialName=materialIndexLists.at(j).first;
			const Collection<int> &indexList=materialIndexLists.at(j).second;

			IndexBuffer::ptr indexBuffer(new IndexBuffer(Buffer::UsageType_STATIC,Buffer::AccessType_WRITE_ONLY,IndexBuffer::IndexFormat_UINT_16,indexList.size()));
			{
				IndexBufferAccessor iba(indexBuffer);

				if(mInvertFaces){
					for(i=0;i<indexList.size();i+=3){
						iba.set(i+0,indexList[i+2]);
						iba.set(i+1,indexList[i+1]);
						iba.set(i+2,indexList[i+0]);
					}
				}
				else{
					for(i=0;i<indexList.size();i+=3){
						iba.set(i+0,indexList[i+0]);
						iba.set(i+1,indexList[i+1]);
						iba.set(i+2,indexList[i+2]);
					}
				}
			}
			Mesh::SubMesh::ptr subMesh(new Mesh::SubMesh());
			mMesh->subMeshes.add(subMesh);

			subMesh->indexData=IndexData::ptr(new IndexData(IndexData::Primitive_TRIS,indexBuffer));
			subMesh->materialName=materialName;
		}
	}
}

String SMDConverter::readLine(InputStream *in){
	String string;
	char c=0;
	while(in->available()>0){
		c=in->readByte();
		if(c==(char)10){
			break;
		}
		string=string+(char)c;
	}
	return string;
}

void SMDConverter::setQuaternionFromXYZ(Quaternion &r,scalar x,scalar y,scalar z){
	Matrix3x3 matrix;

	Matrix3x3 xmat,ymat,zmat;
	Math::setMatrix3x3FromX(xmat,x);
	Math::setMatrix3x3FromY(ymat,y);
	Math::setMatrix3x3FromZ(zmat,z);
	Math::mul(matrix,zmat,ymat);
	zmat.set(matrix);
	Math::mul(matrix,zmat,xmat);
	Math::setQuaternionFromMatrix3x3(r,matrix);
}

bool SMDConverter::vertsEqual(const Vertex &v1,const Vertex &v2){
	return	v1.bone==v2.bone &&
		Math::lengthSquared(v1.position,v2.position)<mPositionEpsilon &&
		Math::lengthSquared(v1.normal,v2.normal)<mNormalEpsilon &&
		Math::lengthSquared(v1.texCoord,v2.texCoord)<mTexCoordEpsilon;
}