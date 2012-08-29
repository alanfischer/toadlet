#include "SMDConverter.h"

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

	Triangle(String m,const Vertex &v1,const Vertex &v2,const Vertex &v3){
		material=m;
		verts[0]=v1;
		verts[1]=v2;
		verts[2]=v3;
	}

	String material;
	Vertex verts[3];
};

SMDConverter::SMDConverter():
	mEngine(NULL),
	mPositionEpsilon(Math::fromMilli(10)),
	mNormalEpsilon(Math::fromMilli(10)),
	mTexCoordEpsilon(Math::fromMilli(10)),
	mFPS(30),
	mRemoveSkeleton(false),
	mInvertFaces(false),
	mScale(Math::ONE)
{
}

void SMDConverter::load(Engine *engine,Stream *in,const String &fileName){
	mEngine=engine;

	String resourceName;
	int ext=fileName.rfind('.');
	if(ext>=0){
		resourceName=fileName.substr(0,ext);
	}
	else{
		resourceName=fileName;
	}

	int block=Block_NONE;
	int version=0;
	bool reference=false;
	int time=0;
	String material;
	int vertindex=0;
	Vertex triverts[3];
	Collection<Triangle> triangles;
	Skeleton::ptr skeleton=new Skeleton();

	if(mMesh==NULL){
		mMesh=new Mesh();
		mMesh->setName(resourceName);

		mSkeleton=skeleton;
		mMesh->setSkeleton(skeleton);

		reference=true;
	}

	String s;
	while(in->position()<in->length()){
		s=readLine(in);
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
					mSequence=new Sequence();
					mSequence->setName(resourceName);
					mSkeleton->addSequence(mSequence);

					for(int i=0;i<mSkeleton->getNumBones();++i){
						mSequence->addTrack(new Track(engine->getVertexFormats().POSITION_ROTATE_SCALE));
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
					for(i=0;i<mSkeleton->getNumBones();++i){
						if(mSkeleton->getBone(i)==NULL){
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

				Skeleton::Bone::ptr bone(new Skeleton::Bone());
				bone->index=id;
				bone->parentIndex=pid;
				bone->name=name;
				skeleton->addBone(bone);
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

				if(reference){
					Skeleton::Bone::ptr bone=skeleton->getBone(id);
					bone->translate.set(MathConversion::floatToScalar(px),MathConversion::floatToScalar(py),MathConversion::floatToScalar(pz));
					Math::mul(bone->translate,mScale);
					setQuaternionFromXYZ(bone->rotate,MathConversion::floatToScalar(rx),MathConversion::floatToScalar(ry),MathConversion::floatToScalar(rz));
				}
				else{
					Skeleton::Bone::ptr bone=mSkeleton->getBone(skeleton->getBone(id)->name);
					if(bone!=NULL){
						Track::ptr track=mSequence->getTrack(bone->index);

						VertexBufferAccessor &vba=track->getAccessor();
						Vector3 translate;
						Quaternion rotate;
						int index=track->addKeyFrame(Math::fromInt(time)/mFPS);
						translate.set(MathConversion::floatToScalar(px),MathConversion::floatToScalar(py),MathConversion::floatToScalar(pz));
						Math::mul(translate,mScale);
						setQuaternionFromXYZ(rotate,MathConversion::floatToScalar(rx),MathConversion::floatToScalar(ry),MathConversion::floatToScalar(rz));
						vba.set3(index,0,translate);
						vba.set4(index,1,rotate);
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
				v.position.set(MathConversion::floatToScalar(px),MathConversion::floatToScalar(py),MathConversion::floatToScalar(pz));
				Math::mul(v.position,mScale);
				v.normal.set(MathConversion::floatToScalar(nx),MathConversion::floatToScalar(ny),MathConversion::floatToScalar(nz));
				v.texCoord.set(MathConversion::floatToScalar(tu),MathConversion::floatToScalar(tv));
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
			SkeletonComponent::ptr skeleton=new SkeletonComponent(mEngine,mSkeleton);
			skeleton->updateBones();
			for(i=0;i<skeleton->getNumBones();++i){
				Vector3 wtbTranslation(skeleton->getBone(i)->worldTranslate);
				Math::neg(wtbTranslation);

				Quaternion wtbRotation(skeleton->getBone(i)->worldRotate);
				Math::conjugate(wtbRotation);
				Math::mul(wtbTranslation,wtbRotation);

				mSkeleton->getBone(i)->worldToBoneTranslate.set(wtbTranslation);
				mSkeleton->getBone(i)->worldToBoneRotate.set(wtbRotation);
			}
		}
		else{
			mSkeleton=NULL;
			mMesh->setSkeleton(NULL);
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

		VertexFormat::ptr vertexFormat=mEngine->getBufferManager()->createVertexFormat();
		vertexFormat->addElement(VertexFormat::Semantic_POSITION,0,VertexFormat::Format_TYPE_FLOAT_32|VertexFormat::Format_COUNT_3);
		vertexFormat->addElement(VertexFormat::Semantic_NORMAL,0,VertexFormat::Format_TYPE_FLOAT_32|VertexFormat::Format_COUNT_3);
		vertexFormat->addElement(VertexFormat::Semantic_TEXCOORD,0,VertexFormat::Format_TYPE_FLOAT_32|VertexFormat::Format_COUNT_2);

		VertexBuffer::ptr vertexBuffer=mEngine->getBufferManager()->createVertexBuffer(Buffer::Usage_BIT_STATIC,Buffer::Access_BIT_WRITE,vertexFormat,verts.size());
		{
			VertexBufferAccessor vba(vertexBuffer);
			for(i=0;i<verts.size();++i){
				Vertex &v=verts[i];
				vba.set3(i,0,v.position);
				vba.set3(i,1,v.normal);
				vba.set2(i,2,v.texCoord);
			}
		}
		mMesh->setStaticVertexData(VertexData::ptr(new VertexData(vertexBuffer)));

		if(mSkeleton!=NULL){
			Collection<Mesh::VertexBoneAssignmentList> vbas(verts.size());
			for(i=0;i<verts.size();++i){
				Mesh::VertexBoneAssignmentList list;
				list.add(Mesh::VertexBoneAssignment(verts[i].bone,Math::ONE));
				vbas.setAt(i,list);
			}
			mMesh->setVertexBoneAssignments(vbas);
		}

		for(j=0;j<materialIndexLists.size();++j){
			const String &materialName=materialIndexLists.at(j).first;
			const Collection<int> &indexList=materialIndexLists.at(j).second;

			IndexBuffer::ptr indexBuffer=mEngine->getBufferManager()->createIndexBuffer(Buffer::Usage_BIT_STATIC,Buffer::Access_BIT_WRITE,IndexBuffer::IndexFormat_UINT16,indexList.size());
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
			mMesh->addSubMesh(subMesh);

			subMesh->indexData=IndexData::ptr(new IndexData(IndexData::Primitive_TRIS,indexBuffer));
			subMesh->materialName=materialName;
		}
	}
}

String SMDConverter::readLine(Stream *stream){
	String string;
	char c=0;
	while(stream->length()>stream->position()){
		stream->read((tbyte*)&c,1);
		if(c==10 || c==13){
			if(string.length()>0){
				break;
			}
		}
		else{
			string=string+c;
		}
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