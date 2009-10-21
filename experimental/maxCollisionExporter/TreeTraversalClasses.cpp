#include "TreeTraversalClasses.h"
#include "../MAXUtilities/MAXUtilities.h"

#include "DECOMP.H"

#define EPSILON 0.0001

using namespace toadlet::egg;
using namespace toadlet::egg::math;
using namespace toadlet::egg::math::Math;
using namespace toadlet::tadpole::skeleton;

// Simple debugging function
static void assertionFailed(bool b, char *err){
	if(!b){
		throw std::runtime_error(err);
	}
}


int IndexNodes::callback(INode *node){
	assertionFailed(!(node)->IsRootNode(), "Encountered a root node in IndexNodes!");

	if(mCollisionExport->skipNode(node)){
		return TREE_CONTINUE;
	}

	mCollisionExport->setNodeIndex(node, mNodeIndex);

	++mNodeIndex;

	mCollisionExport->mNumNodes=mNodeIndex;
	
	return TREE_CONTINUE;
}


int GetGeometry::callback(INode *node){
	// See if this node should be skipped
	if(mCollisionExport->skipNode(node)){
		return TREE_CONTINUE;
	}

	String nodeName(node->GetName());
	Object *obj=node->EvalWorldState(mDumpTime).obj;
	Interval interval;
	INode *nodeParent=node->GetParentNode();
	const Matrix3 nm = node->GetNodeTM(mDumpTime);
	const Matrix3 pm = nodeParent->GetNodeTM(mDumpTime);
	const Matrix3 nml = nm * Inverse(pm);

	// Get local affine decomposition in max
	AffineParts ap;
	decomp_affine(nml, &ap);
	Point3 maxPosition=ap.t;
	Point3 maxScale=ap.k;
	Quat maxRotation=ap.q;

	// Assign to toadlet objects
	Vector3 translation=makeVector3FromMaxPoint3(maxPosition);
	math::Quaternion quatRotation=makeQuaternionFromMaxQuat(maxRotation);
	Matrix3x3 matrixRotation=makeMatrix3x3(quatRotation);
	Vector3 scale=makeVector3FromMaxPoint3(maxScale);

	Matrix4x4 transform;
	setMatrix4x4(transform,matrixRotation,translation);

	if(obj->ClassID()==Class_ID(BOXOBJ_CLASS_ID,0)){
		float x=0,y=0,z=0;
		obj->GetParamBlock()->GetValue(BOXOBJ_WIDTH,mDumpTime,x,interval);
		obj->GetParamBlock()->GetValue(BOXOBJ_LENGTH,mDumpTime,y,interval);
		obj->GetParamBlock()->GetValue(BOXOBJ_HEIGHT,mDumpTime,z,interval);

		BoxShapeData boxData(Vector3(x*scale.x*0.5,y*scale.y*0.5,z*scale.z*0.5));
		Vector3 offset(0,0,z*0.5f*scale.z);
		offset=matrixRotation*offset;
		setMatrix4x4(transform,matrixRotation,translation+offset);
		boxData.transform=transform;

		mCollisionExport->mShapeCollection.addShapeData(boxData);
	}
	else if(obj->ClassID()==Class_ID(SPHERE_CLASS_ID,0)){
		if(fabs(scale.x-scale.y)>EPSILON || fabs(scale.x-scale.z)>EPSILON){
			MessageBox(0,"Non-uniform scale on sphere!  Not exporting this solid\n","Error",0);
			return TREE_CONTINUE;
		}

		float radius=0;
		obj->GetParamBlock()->GetValue(SPHERE_RADIUS,mDumpTime,radius,interval);

		SphereShapeData sphereData(radius*scale.x);
		sphereData.transform=transform;

		mCollisionExport->mShapeCollection.addShapeData(sphereData);
	}
	else if(strcmp(obj->GetObjectName(),"Capsule")==0){ // No CAPSULE_CLASS_ID
		if(fabs(scale.x-scale.y)>EPSILON || fabs(scale.x-scale.z)>EPSILON){
			MessageBox(0,"Non-uniform scale on capsule!  Not exporting this solid\n","Error",0);
			return TREE_CONTINUE;
		}

		float radius=0;
		obj->GetParamBlock()->GetValue(0,mDumpTime,radius,interval);

		float length=0;
		obj->GetParamBlock()->GetValue(1,mDumpTime,length,interval);

		CylinderShapeData cylinderData(Vector3(radius*scale.y,radius*scale.z,length*scale.x*0.5f));
		Vector3 offset(0,0,(length*scale.x+radius*scale.x*2)*0.5f);
		offset=matrixRotation*offset;
		setMatrix4x4(transform,matrixRotation,translation+offset);
		cylinderData.transform=transform;

		mCollisionExport->mShapeCollection.addShapeData(cylinderData);
	}
	// If its a trimesh, then just use it as a point hull
	else if(obj->CanConvertToType(triObjectClassID)){
		TriObject *triobj=(TriObject*)obj->ConvertToType(mDumpTime,triObjectClassID);

		Mesh *mesh=&triobj->mesh;

		Matrix3 objectMatrix = node->GetObjectTM(mDumpTime);

		bool negScale=(DotProd(CrossProd(objectMatrix.GetRow(0),objectMatrix.GetRow(1)),objectMatrix.GetRow(2))<0.0)?1:0;
		int vi0=0,vi1=1,vi2=2;
		if(negScale){
			vi0=2;
			vi1=1;
			vi2=0;
		}

		Collection<Vector3> points(mesh->getNumVerts());

		int i;
		for(i=0;i<mesh->getNumVerts();++i){
			Point3 p=mesh->getVert(i);
			p=p*objectMatrix;

			points[i]=Vector3(p.x,p.y,p.z);
		}

		HullShapeData hullData(points);

		// The transform is put into the points, so we don't need it here
		//hullData.transform=transform;

		mCollisionExport->mShapeCollection.addShapeData(hullData);
	}
/*
	else if(obj->CanConvertToType(triObjectClassID)){
		TriObject *triobj=(TriObject*)obj->ConvertToType(mDumpTime,triObjectClassID);

		Mesh *mesh=&triobj->mesh;

		Matrix3 objectMatrix = node->GetObjectTM(mDumpTime);

		bool negScale=(DotProd(CrossProd(objectMatrix.GetRow(0),objectMatrix.GetRow(1)),objectMatrix.GetRow(2))<0.0)?1:0;
		int vi0=0,vi1=1,vi2=2;
		if(negScale){
			vi0=2;
			vi1=1;
			vi2=0;
		}

		Collection<Vector3> vertexes;
		Collection<int> indexes;

		int i;
		for(i=0;i<mesh->numFaces;++i){
			Face *face=&mesh->faces[i];

			int v0=face->getVert(vi0);
			int v1=face->getVert(vi1);
			int v2=face->getVert(vi2);

			indexes.push_back(v0);
			indexes.push_back(v1);
			indexes.push_back(v2);
		}

		for(i=0;i<indexes.size();++i){
			if(indexes[i]>=vertexes.size()){
				vertexes.resize(indexes[i]+1);
			}

			Point3 v=mesh->getVert(indexes[i]);
			v=v*objectMatrix;
			vertexes[indexes[i]]=Vector3(v.x,v.y,v.z);
		}

		MeshShapeData meshData;
		meshData.numVertices=vertexes.size();

		meshData.vertexArray=new float[vertexes.size()*3];

		for(i=0;i<vertexes.size();++i){
			meshData.vertexArray[i*3+0]=vertexes[i].x;
			meshData.vertexArray[i*3+1]=vertexes[i].y;
			meshData.vertexArray[i*3+2]=vertexes[i].z;
		}

		meshData.numTriangles=indexes.size()/3;
		meshData.triangleArray=new unsigned int[indexes.size()];
		memcpy(meshData.triangleArray,(unsigned int*)&indexes[0],indexes.size()*sizeof(unsigned int));

		// The transform is put into the vertexes, so we don't need it here
		//meshData.offset=toOpalMatrix(transform);

		mCollisionExport->mSolidData.addShape(meshData);
	}
*/
	return TREE_CONTINUE;
}
