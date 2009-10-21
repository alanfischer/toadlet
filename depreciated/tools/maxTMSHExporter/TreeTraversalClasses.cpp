#include "TreeTraversalClasses.h"
#include "../../experimental/MAXUtilities/MAXUtilities.h"

#include "DECOMP.H"

using namespace toadlet::egg;
using namespace toadlet::egg::math;
using namespace toadlet::tadpole::skeleton;

// Simple debugging function
static void assertionFailed(bool b, char *err){
	if(!b){
		TOADLET_LOG(NULL,Logger::LEVEL_ERROR) << err;
		throw std::runtime_error(err);
	}
}


int IndexNodes::callback(INode *node){
	assertionFailed(!(node)->IsRootNode(), "Encountered a root node in IndexNodes!");

	if(mMeshExport->skipNode(node)){
		return TREE_CONTINUE;
	}

	mMeshExport->setNodeIndex(node, mNodeIndex);

	++mNodeIndex;

	mMeshExport->mNumNodes=mNodeIndex;
	
	return TREE_CONTINUE;
}


int GetSkeleton::callback(INode *node){
	assertionFailed(!(node)->IsRootNode(), "Encountered a root node in DumpNodes!");

	if(mMeshExport->skipNode(node)){
		return TREE_CONTINUE;
	}

	INode *nodeParent;
	nodeParent = node->GetParentNode();
	
	// The model's root is a child of the real "scene root"
	String nodeName(node->GetName());
	BOOL parentIsRoot = nodeParent->IsRootNode();

	// Get transformation matrices
	const Matrix3 nm = node->GetNodeTM(mDumpTime);
	const Matrix3 pm = nodeParent->GetNodeTM(mDumpTime);
	const Matrix3 nml = nm * Inverse(pm);

	// Get local affine decomposition in max
	AffineParts ap;
	decomp_affine(nml,&ap);
	Point3 maxPosition=ap.t;
	Point3 maxScale=ap.k;
	Quat maxRotation=ap.q;

	// Assign to toadlet objects
	Vector3 boneTranslation=makeVector3FromMaxPoint3(maxPosition);
	Quaternion boneQuat=makeQuaternionFromMaxQuat(maxRotation);
	Vector3 boneScale=makeVector3FromMaxPoint3(maxScale);

	// Get indices
	int nodeIndex = mMeshExport->getNodeIndex(node);
	int parentIndex = mMeshExport->getNodeIndex(nodeParent);
	if(parentIsRoot){
		parentIndex = -1;
	}

	// Add a new bone to the skeleton
	BoneData *b = new BoneData();
	b->index = nodeIndex;
	b->parent = parentIndex;
	if(parentIndex>=0){
		mMeshExport->mSkeleton->getBoneData(parentIndex)->children.push_back(nodeIndex);
	}
	b->translate = boneTranslation;
	b->rotate = boneQuat;
	b->scale = boneScale;
	b->simpleName = nodeName;

	// Get global affine decomposition in max
	decomp_affine(Inverse(nm),&ap);
	maxPosition=ap.t;
	maxRotation=ap.q;

	// Compute world to bone info
	setQuaternionFromMaxQuat(b->worldToBoneRotate,maxRotation);
	setVector3FromMaxPoint3(b->worldToBoneTranslate,maxPosition);
	mMeshExport->mSkeleton->addBoneData(b);

	return TREE_CONTINUE;
}


int GetAnimation::callback(INode *node){
	assertionFailed(!(node)->IsRootNode(), "Encountered a root node in DumpNodes!");

	if(mMeshExport->skipNode(node)){
		return TREE_CONTINUE;
	}

	INode *nodeParent;
	nodeParent = node->GetParentNode();
	
	// The model's root is a child of the real "scene root"
	String nodeName(node->GetName());
	BOOL parentIsRoot = nodeParent->IsRootNode();

	// Get transformation matrices
	const Matrix3 nm = node->GetNodeTM(mDumpTime);
	const Matrix3 pm = nodeParent->GetNodeTM(mDumpTime);
	const Matrix3 nml = nm * Inverse(pm);

	// Get local affine decomposition in max
	AffineParts ap;
	decomp_affine(nml,&ap);
	Point3 maxPosition=ap.t;
	Point3 maxScale=ap.k;
	Quat maxRotation=ap.q;

	// Assign to toadlet objects
	Vector3 boneTranslation=makeVector3FromMaxPoint3(maxPosition);
	Quaternion boneQuat=makeQuaternionFromMaxQuat(maxRotation);
	Vector3 boneScale=makeVector3FromMaxPoint3(maxScale);

	// Get indices
	int nodeIndex = mMeshExport->getNodeIndex(node);
	int parentIndex = mMeshExport->getNodeIndex(nodeParent);
	if(parentIsRoot){
		parentIndex = -1;
	}

	// Add a new animation track if needed
	if(nodeIndex >= mMeshExport->mAnimation->tracks.size()){
		AnimationData::Track *t = new AnimationData::Track(mMeshExport->mAnimation);
		t->bone = nodeIndex;
		mMeshExport->mAnimation->tracks.push_back(t);
	}

	// Add new keyframe for bone to track
	AnimationData::KeyFrame k;
	k.time = TicksToSec(mDumpTime);
	k.translate = boneTranslation;
	k.rotate = boneQuat;
	k.scale = boneScale;

	mMeshExport->mAnimation->tracks[nodeIndex]->keyFrames.push_back(k);

	return TREE_CONTINUE;
}


int GetGeometry::callback(INode *node){
	Object*	obj;
	String nodeName(node->GetName());
	
	// See if this node should be skipped
	if(mMeshExport->skipNode(node)){
		return TREE_CONTINUE;
	}

	// Get the node's index
	int nodeIndex = mMeshExport->getNodeIndex(node);

	// Update progress bar
	mMeshExport->mInterface->ProgressUpdate((int)(((float)nodeIndex/(float)(mMeshExport->mNumNodes))*100.0f));

	// Grab the nodes material, and optionally skip the null ones
	Mtl *nodeMaterial = node->GetMtl();
	if(nodeMaterial==NULL && mMeshExport->mOptions->mSkipNullMaterials){
		return TREE_CONTINUE;
	}

	// Get Node's object and convert to a triangle-mesh object, so I can access the Faces
	obj = node->EvalWorldState(mDumpTime).obj;
	TriObject *triobj;
	BOOL convertToTriObject = obj->CanConvertToType(triObjectClassID) &&
		(triobj = (TriObject*)obj->ConvertToType(mDumpTime, triObjectClassID)) != NULL;
	if(!convertToTriObject){
		return TREE_CONTINUE;
	}
	Mesh *mesh = &triobj->mesh;

	// Ensure that the vertex normals are up-to-date
	mesh->buildNormals();

	// Get transform matrix of object and one without rotation or scaling for normals
	Matrix3 objectMatrix = node->GetObjectTM(mDumpTime);
	Matrix3 objectMatrixOnlyRot = objectMatrix;
	objectMatrixOnlyRot.NoScale();
	objectMatrixOnlyRot.NoTrans();

	// See if the object is negatively scaled - affecting vertex index order
	BOOL negScale = (DotProd(CrossProd(objectMatrix.GetRow(0),objectMatrix.GetRow(1)),objectMatrix.GetRow(2))<0.0)?1:0;
	int vi0=0, vi1=1, vi2=2;
	if(negScale){
		vi0 = 2;
		vi1 = 1;
		vi2 = 0;
	}

	// Initialize physique export parameters
	mPhyContextExport = NULL;
	mPhyExport = NULL;
    mPhyMod = NULL;

	// See if the physique modifier is used with this node
	mUsePhysique = findPhysiqueModifier(node);

	// Loop through the face info
	int numFaces = mesh->getNumFaces();
	bool textured = true;
	if(mesh->tvFace == NULL){
		textured = false;
	}
	for(int f=0; f<numFaces; ++f){
		Face* face = &mesh->faces[f];
		DWORD groupFace = face->getSmGroup();

		// Get face's 3 indexes into the Mesh's vertex array(s).
		DWORD v0 = face->getVert(vi0);
		DWORD v1 = face->getVert(vi1);
		DWORD v2 = face->getVert(vi2);
		assertionFailed((int)v0 < mesh->getNumVerts(), "Bogus Vertex 0 index");
		assertionFailed((int)v1 < mesh->getNumVerts(), "Bogus Vertex 1 index");
		assertionFailed((int)v2 < mesh->getNumVerts(), "Bogus Vertex 2 index");
		
		// Get the 3 Vertex's for this face
		Point3 pt0 = mesh->getVert(v0);
		Point3 pt1 = mesh->getVert(v1);
		Point3 pt2 = mesh->getVert(v2);

		// Rotate the face vertices out of object-space, and into world-space
		Point3 vw0 = pt0 * objectMatrix;
		Point3 vw1 = pt1 * objectMatrix;
		Point3 vw2 = pt2 * objectMatrix;

		// Store these vertices in the current submesh
		mMeshExport->mVertexes.push_back(Vector3(vw0.x, vw0.y, vw0.z));
		mMeshExport->mVertexes.push_back(Vector3(vw1.x, vw1.y, vw1.z));
		mMeshExport->mVertexes.push_back(Vector3(vw2.x, vw2.y, vw2.z));

		// Get the 3 RVertex's for this face
		// Apparently using getRVertPtr instead of getRVert works around a 3DSMax bug
		RVertex *rv0 = mesh->getRVertPtr(v0);
		RVertex *rv1 = mesh->getRVertPtr(v1);
		RVertex *rv2 = mesh->getRVertPtr(v2);
		
		// Find appropriate normals for each RVertex
		// A vertex can be part of multiple faces, so the "smoothing group"
		// is used to locate the normal for this face's use of the vertex.
		if(mMeshExport->mOptions->mExportVertexNormals){
			Point3 n0;
			Point3 n1;
			Point3 n2;
			if(groupFace){
				n0 = getVertexNormal(rv0, groupFace);
				n1 = getVertexNormal(rv1, groupFace);
				n2 = getVertexNormal(rv2, groupFace);
			}
			else {
				n0 = mesh->getFaceNormal(f);
				n1 = mesh->getFaceNormal(f);
				n2 = mesh->getFaceNormal(f);
			}
			assertionFailed( Length( n0 ) <= 1.1, "bogus orig normal 0" );
			assertionFailed( Length( n1 ) <= 1.1, "bogus orig normal 1" );
			assertionFailed( Length( n2 ) <= 1.1, "bogus orig normal 2" );

			// Just rotate the normals back to world space
			Point3 nr0 = VectorTransform(objectMatrixOnlyRot, n0);
			Point3 nr1 = VectorTransform(objectMatrixOnlyRot, n1);
			Point3 nr2 = VectorTransform(objectMatrixOnlyRot, n2);

			// Store these normals in the current submesh
			mMeshExport->mNormals.push_back(Vector3(nr0.x, nr0.y, nr0.z));
			mMeshExport->mNormals.push_back(Vector3(nr1.x, nr1.y, nr1.z));
			mMeshExport->mNormals.push_back(Vector3(nr2.x, nr2.y, nr2.z));
		}

		// Get the material for this face
		Mtl *mtl = nodeMaterial;
		if(mMeshExport->mOptions->mExportMaterials && nodeMaterial){
			if(nodeMaterial->IsMultiMtl()){
				// We have a multi material, so find the currently active one
				MtlID materialId = face->getMatID();
				if(materialId >= nodeMaterial->NumSubMtls()){
					materialId = 0;
				}
				mtl = nodeMaterial->GetSubMtl(materialId);

				if(mtl->IsMultiMtl()){
					// Found a sub-material
					mtl = mtl->GetSubMtl(materialId);
				}
			}
		}

		// Store the material
		Collection<int> &ib=mMeshExport->mMtlIndexBufferMap[mtl];

		// Add indices to the index buffer
		for(int i=0; i<3; ++i){
			ib.push_back(mMeshExport->mIndexCounter);
			mMeshExport->mIndexCounter++;
		}

		// Obtain texture coordinates
		if(mMeshExport->mOptions->mExportTextureCoordinates){
			// Parametric vertices for textures
			UVVert uv0( 0, 0, 0 );
			UVVert uv1( 0, 0, 0 );
			UVVert uv2( 0, 0, 0 );

			// Get TVface's 3 indexes into the Mesh's TVertex array(s),
			if(textured){
				TVFace*	tvFace = &mesh->tvFace[f];
				DWORD tv0 = tvFace->getTVert(vi0);
				DWORD tv1 = tvFace->getTVert(vi1);
				DWORD tv2 = tvFace->getTVert(vi2);
				assertionFailed((int)tv0 < mesh->getNumTVerts(), "Bogus texture 0 index");
				assertionFailed((int)tv1 < mesh->getNumTVerts(), "Bogus texture 1 index");
				assertionFailed((int)tv2 < mesh->getNumTVerts(), "Bogus texture 2 index");

				// Get the 3 TVertex's for this TVFace
				uv0 = mesh->getTVert(tv0);
				uv1 = mesh->getTVert(tv1);
				uv2 = mesh->getTVert(tv2);
			}

			// Store these coordinates in the current submesh
			mMeshExport->mTextureVertexes.push_back(Vector2(uv0.x, uv0.y));
			mMeshExport->mTextureVertexes.push_back(Vector2(uv1.x, uv1.y));
			mMeshExport->mTextureVertexes.push_back(Vector2(uv2.x, uv2.y));
		}

		if(mMeshExport->mSkeleton!=NULL){
			// Determine owning bones for the vertices.
			int numBones0,numBones1,numBones2;
			Collection<int> bindexes;
			Collection<float> bweights;

			if(mUsePhysique){
				// The Physique add-in allows vertices to be assigned to bones arbitrarily
				int i;

				bindexes.clear();
				bweights.clear();
				numBones0=getNumberOfBonesControllingVertex(v0);
				for(i=0;i<numBones0;i++){
					bindexes.push_back(getBoneControllerOfVertex(i,v0));
					bweights.push_back(getBoneControllerWeightOfVertex(i,v0));
				}
				mMeshExport->mBoneIndexes.push_back(bindexes);
				mMeshExport->mBoneWeights.push_back(bweights);

				bindexes.clear();
				bweights.clear();
				numBones1=getNumberOfBonesControllingVertex(v1);
				for(i=0;i<numBones1;i++){
					bindexes.push_back(getBoneControllerOfVertex(i,v1));
					bweights.push_back(getBoneControllerWeightOfVertex(i,v1));
				}
				mMeshExport->mBoneIndexes.push_back(bindexes);
				mMeshExport->mBoneWeights.push_back(bweights);			

				bindexes.clear();
				bweights.clear();
				numBones2=getNumberOfBonesControllingVertex(v2);
				for(i=0;i<numBones2;i++){
					bindexes.push_back(getBoneControllerOfVertex(i,v2));
					bweights.push_back(getBoneControllerWeightOfVertex(i,v2));
				}
				mMeshExport->mBoneIndexes.push_back(bindexes);
				mMeshExport->mBoneWeights.push_back(bweights);	
			}	
			else{
				// Simple 3dsMax model: the vertices are owned by the object, and hence the node
				numBones0=1;
				bindexes.clear();
				bweights.clear();
				bindexes.push_back(nodeIndex);
				bweights.push_back(1);
				mMeshExport->mBoneIndexes.push_back(bindexes);
				mMeshExport->mBoneWeights.push_back(bweights);

				numBones1=1;
				bindexes.clear();
				bweights.clear();
				bindexes.push_back(nodeIndex);
				bweights.push_back(1);
				mMeshExport->mBoneIndexes.push_back(bindexes);
				mMeshExport->mBoneWeights.push_back(bweights);

				numBones2=1;
				bindexes.clear();
				bweights.clear();
				bindexes.push_back(nodeIndex);
				bweights.push_back(1);
				mMeshExport->mBoneIndexes.push_back(bindexes);
				mMeshExport->mBoneWeights.push_back(bweights);
			}
		}
	}
	
	// Clean up possible use of physique modifier
	if (mPhyMod && mPhyExport){
		if (mPhyContextExport){
			mPhyExport->ReleaseContextInterface(mPhyContextExport);
			mPhyContextExport = NULL;
        }
        mPhyMod->ReleaseInterface(I_PHYINTERFACE, mPhyExport);
		mPhyExport = NULL;
		mPhyMod = NULL;
	}
	
	return TREE_CONTINUE;
}


Point3 GetGeometry::getVertexNormal(RVertex *rv, DWORD groupFace){
	// Lookup the appropriate vertex normal, based on smoothing group.
	int normals = rv->rFlags & NORCT_MASK;

	assertionFailed((normals==1 && rv->ern==NULL) || (normals>1 && rv->ern!=NULL), "BOGUS RVERTEX");
	
	if(normals==1){
		return rv->rn.getNormal();
	}
	else{
		for(int i=0; i<normals; ++i){
			if(rv->ern[i].getSmGroup() & groupFace){
				break;
			}
		}

		if(i>=normals){
			i=0;
		}

		return rv->ern[i].getNormal();
	}
}


bool GetGeometry::findPhysiqueModifier(INode *node){
	Object *obj = node->GetObjectRef();
	if(!obj){
		return false;
	}

	// See if we can derive all the way down to a PhyContextExport
	if(obj->SuperClassID() == GEN_DERIVOB_CLASS_ID){
		IDerivedObject *derivedObj = static_cast<IDerivedObject*>(obj);

		for(int i=0; i<derivedObj->NumModifiers(); ++i){
			mPhyMod = derivedObj->GetModifier(i);

			if(mPhyMod->ClassID()==Class_ID(PHYSIQUE_CLASS_ID_A, PHYSIQUE_CLASS_ID_B)){
				mPhyExport = (IPhysiqueExport *)mPhyMod->GetInterface(I_PHYINTERFACE);

				if(mPhyExport){
					mPhyContextExport = (IPhyContextExport *)mPhyExport->GetContextInterface(node);

					if(mPhyContextExport){
						mPhyContextExport->ConvertToRigid(TRUE);
						mPhyContextExport->AllowBlending(TRUE);
						return true;
					}
				}
			}
		}
	}

	return false;
}


int GetGeometry::getNumberOfBonesControllingVertex(int i){
	int num=0;

	IPhyVertexExport *export=mPhyContextExport->GetVertexInterface(i);

	if(export){
		if(export->GetVertexType() == RIGID_BLENDED_TYPE){
			IPhyBlendedRigidVertex *vertex=(IPhyBlendedRigidVertex*)export;
			num=vertex->GetNumberNodes();
		}
		else{
			num=1;
		}
		mPhyContextExport->ReleaseVertexInterface(export);
	}

	return num;
}


int GetGeometry::getBoneControllerOfVertex(int boneIndex,int vertexIndex){
	int index=0;

	IPhyVertexExport *export=mPhyContextExport->GetVertexInterface(vertexIndex);

	if(export){
		if(export->GetVertexType() == RIGID_BLENDED_TYPE){
			IPhyBlendedRigidVertex *vertex=(IPhyBlendedRigidVertex*)export;
			index=mMeshExport->getNodeIndex(vertex->GetNode(boneIndex));
		}
		else{
			INode *bone=((IPhyRigidVertex*)export)->GetNode();
			index=mMeshExport->getNodeIndex(bone);
		}
		mPhyContextExport->ReleaseVertexInterface(export);
	}

	return index;
}


float GetGeometry::getBoneControllerWeightOfVertex(int boneIndex,int vertexIndex){
	float weight=0;

	IPhyVertexExport *export=mPhyContextExport->GetVertexInterface(vertexIndex);

	if(export){
		if(export->GetVertexType() == RIGID_BLENDED_TYPE){
			IPhyBlendedRigidVertex *vertex=(IPhyBlendedRigidVertex*)export;
			weight=vertex->GetWeight(boneIndex);
		}
		else{
			weight=1;
		}
		mPhyContextExport->ReleaseVertexInterface(export);
	}

	return weight;
}


int GetLights::callback(INode *node){
	Object *obj = node->GetObjectRef();

	// Only grab the lights
	if(obj->SuperClassID()==LIGHT_CLASS_ID){
		// Get max light info
		ObjectState os = node->EvalWorldState(mDumpTime);
		if(!os.obj){
			return TREE_CONTINUE;
		}
		GenLight* maxLight = (GenLight*)os.obj;
		struct LightState ls;
		Interval valid = FOREVER;
		maxLight->EvalLightState(mDumpTime, valid, &ls);
		float red = ls.color.r*ls.intens;
		float green = ls.color.g*ls.intens;
		float blue = ls.color.b*ls.intens;

		// Get light position and orientation
		Matrix3 nm = node->GetNodeTM(mDumpTime);
		AffineParts ap;
		decomp_affine(nm, &ap);
		Point3 pos = ap.t;
		Quat rot = ap.q;
		float x=0,y=0,z=0;
		rot.GetEuler(&x,&y,&z);

		// Create a toadlet light
		toadlet::peeper::Light light;
		light.setPosition(Vector3(pos[0],pos[1],pos[2]));
		light.setDiffuse(Vector4(red, green, blue, 1.0f));
		light.setSpecular(Vector4(red, green, blue, 1.0f));

		if(ls.type==OMNI_LIGHT){
			// No direction
			light.setType(toadlet::peeper::Light::LT_POSITION);
		}
		else if(ls.type==DIR_LIGHT){
			light.setType(toadlet::peeper::Light::LT_DIRECTION);
			// TODO: Calculate a direction vector from the Euler's XYZ
		}
		else{
			// Just ignore it
		}

		mMeshExport->mLights.push_back(light);
	}
	
	return TREE_CONTINUE;
}
