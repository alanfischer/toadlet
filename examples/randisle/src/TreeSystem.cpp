#include "TreeSystem.h"
#include "Resources.h"

TreeSystem::TreeSystem(Scene *scene,int seed):
	mEngine(NULL),
	mScene(NULL),

	mSections(0),
	mCountMode(false),

	//mSystem,
	//mBranchVertexStart,
	//mBranchNodeStart,
	//mBranchStarted,
	//mNodes,

	mBranchVertexCount(0),
	mBranchIndexCount(0),
	mLowBranchIndexCount(0),
	//mBranchVertexBuffer,
	//mBranchIndexBuffer,
	//mLowBranchIndexBuffer,
	//mBranchMaterial,

	mLeafVertexCount(0),
	mLeafIndexCount(0),
	//mLeafVertexBuffer,
	//mLeafIndexBuffer,
	//mLeafMaterial,
	//mLeaves,
	mHasWiggleLeaves(false),

	mLowMod(0)
{
	mEngine=scene->getEngine();
	mScene=scene;

	mSections=6;
	mLowMod=2;

	mBranchMaterial=Resources::instance->treeBranch;
	mLeafMaterial=Resources::instance->treeLeaf;

	mSystem=new BranchSystem(seed);
	mSystem->setBranchListener(this);

	mBound=new Bound();
	mWorldBound=new Bound();
}

void TreeSystem::destroy(){
	if(mBranchVertexBuffer!=NULL){
		mBranchVertexBuffer->destroy();
		mBranchVertexBuffer=NULL;
	}
	if(mBranchIndexBuffer!=NULL){
		mBranchIndexBuffer->destroy();
		mBranchIndexBuffer=NULL;
	}
	if(mLowBranchIndexBuffer!=NULL){
		mLowBranchIndexBuffer->destroy();
		mLowBranchIndexBuffer=NULL;
	}
	if(mLeafVertexBuffer!=NULL){
		mLeafVertexBuffer->destroy();
		mLeafVertexBuffer=NULL;
	}
	if(mLeafIndexBuffer!=NULL){
		mLeafIndexBuffer->destroy();
		mLeafIndexBuffer=NULL;
	}

	mBranchMaterial=NULL;
	mLeafMaterial=NULL;
	
	mSystem=NULL;
	mTreeBranches.clear();
	mLeaves.clear();

	BaseComponent::destroy();
}

void TreeSystem::parentChanged(Node *node){
	if(mParent!=NULL){
		mParent->spacialRemoved(this);
	}

	BaseComponent::parentChanged(node);

	if(mParent!=NULL){
		mParent->spacialAttached(this);
	}
}

void TreeSystem::grow(){
	mCountMode=true;
	resetCounts();
	mSystem->start();
	while(mSystem->update(30));
	mCountMode=false;

	mBranchVertexBuffer=mEngine->getBufferManager()->createVertexBuffer(Buffer::Usage_BIT_STATIC,Buffer::Access_BIT_WRITE,mEngine->getVertexFormats().POSITION_NORMAL_TEX_COORD,mBranchVertexCount);
	mBranchIndexBuffer=mEngine->getBufferManager()->createIndexBuffer(Buffer::Usage_BIT_STATIC,Buffer::Access_BIT_WRITE,IndexBuffer::IndexFormat_UINT16,mBranchIndexCount);
	mLowBranchIndexBuffer=mEngine->getBufferManager()->createIndexBuffer(Buffer::Usage_BIT_STATIC,Buffer::Access_BIT_WRITE,IndexBuffer::IndexFormat_UINT16,mLowBranchIndexCount);
	mLeafVertexBuffer=mEngine->getBufferManager()->createVertexBuffer(Buffer::Usage_BIT_STREAM,Buffer::Access_READ_WRITE,mEngine->getVertexFormats().POSITION_NORMAL_TEX_COORD,mLeafVertexCount);
	mLeafIndexBuffer=mEngine->getBufferManager()->createIndexBuffer(Buffer::Usage_BIT_STATIC,Buffer::Access_BIT_WRITE,IndexBuffer::IndexFormat_UINT16,mLeafIndexCount);
	mLeaves.resize(mLeafVertexCount/4);
	{
		bvba.lock(mBranchVertexBuffer,Buffer::Access_BIT_WRITE);
		biba.lock(mBranchIndexBuffer,Buffer::Access_BIT_WRITE);
		lbiba.lock(mLowBranchIndexBuffer,Buffer::Access_BIT_WRITE);
		lvba.lock(mLeafVertexBuffer,Buffer::Access_BIT_WRITE);
		liba.lock(mLeafIndexBuffer,Buffer::Access_BIT_WRITE);

		resetCounts();
		mSystem->start();
		while(mSystem->update(30));

		liba.unlock();
		lvba.unlock();
		lbiba.unlock();
		biba.unlock();
		bvba.unlock();
	}

	calculateNormals(mTreeBranches[0]);

	mergeBounds(mTreeBranches[0]);

	mBound->set(mTreeBranches[0]->bound);

	mMesh=new Mesh();
	mMesh->setStaticVertexData(new VertexData(mBranchVertexBuffer));
	mMesh->setBound(mBound);
	{
		Mesh::SubMesh::ptr subMesh=new Mesh::SubMesh();
		subMesh->indexData=new IndexData(IndexData::Primitive_TRISTRIP,mBranchIndexBuffer);
		subMesh->material=mBranchMaterial;
		mMesh->addSubMesh(subMesh);
	}
	{
		Mesh::SubMesh::ptr subMesh=new Mesh::SubMesh();
		subMesh->vertexData=new VertexData(mLeafVertexBuffer);
		subMesh->indexData=new IndexData(IndexData::Primitive_TRIS,mLeafIndexBuffer);
		subMesh->material=mLeafMaterial;
		mMesh->addSubMesh(subMesh);
	}
	mMesh=shared_static_cast<Mesh>(mEngine->getMeshManager()->manage(mMesh));

	mLowMesh=new Mesh();
	mLowMesh->setStaticVertexData(new VertexData(mBranchVertexBuffer));
	mLowMesh->setBound(mBound);
	{
		Mesh::SubMesh::ptr subMesh=new Mesh::SubMesh();
		subMesh->indexData=new IndexData(IndexData::Primitive_TRISTRIP,mLowBranchIndexBuffer);
		subMesh->material=mBranchMaterial;
		mLowMesh->addSubMesh(subMesh);
	}
	{
		Mesh::SubMesh::ptr subMesh=new Mesh::SubMesh();
		subMesh->vertexData=new VertexData(mLeafVertexBuffer);
		subMesh->indexData=new IndexData(IndexData::Primitive_TRIS,mLeafIndexBuffer);
		subMesh->material=mLeafMaterial;
		mLowMesh->addSubMesh(subMesh);
	}
	mLowMesh=shared_static_cast<Mesh>(mEngine->getMeshManager()->manage(mLowMesh));

	alignLeaves(Math::ZERO_VECTOR3,Math::X_UNIT_VECTOR3,true);
}

BranchSystem::Branch::ptr TreeSystem::branchCreated(BranchSystem::Branch *parent){
	TreeBranch *parentTreeBranch=(TreeBranch*)parent;

	// Our start is the previous section start, if negative then its the root branch
	TreeBranch::ptr treeBranch(new TreeBranch());
	treeBranch->started=(parent==NULL);
	treeBranch->skipFirst=(parent!=NULL);
	treeBranch->lastVertex=mBranchVertexCount-mSections-1;
	treeBranch->parent=parentTreeBranch;

	if(parentTreeBranch!=NULL){
		parentTreeBranch->children.add(treeBranch);
		parentTreeBranch->childrenTimes.add(parentTreeBranch->length);
	}

	if(!mCountMode){
		mTreeBranches.add(treeBranch);
	}

	return treeBranch;
}

void TreeSystem::branchDestroyed(BranchSystem::Branch *branch){
}

void TreeSystem::branchBuild(BranchSystem::Branch *branch){
	TreeBranch *treeBranch=(TreeBranch*)branch;

	if(treeBranch->points.size()>0){
		Segment segment;
		segment.origin=branch->position + mParent->getWorldTranslate();
		segment.direction=branch->position - treeBranch->points[treeBranch->points.size()-1];
		PhysicsCollision result;
		mScene->traceSegment(result,segment,-1,NULL);
		if(result.time<Math::ONE){
			branch->life=0;
		}
	}

	Vector3 branchUp,branchNormal,branchBinormal;
	BranchSystem::getBranchVectors(branch,branchUp,branchNormal,branchBinormal);

	if(treeBranch->points.size()==0){
		treeBranch->bound.set(branch->position,branch->position);
	}
	else{
		treeBranch->bound.merge(branch->position);
	}

	treeBranch->points.add(branch->position);
	Vector3 tangent;
	Math::normalizeCarefully(tangent,branch->velocity,mScene->getEpsilon());
	treeBranch->tangents.add(tangent);
	treeBranch->normals.add((branch->length==0 && treeBranch->parent==NULL)?Math::X_UNIT_VECTOR3:Math::ZERO_VECTOR3);
	treeBranch->scales.add(branch->scale);
	treeBranch->times.add(branch->length);

	// Branches & Nodes
	int i;
	if(treeBranch->skipFirst==true){
		// Skip the first building, because it is in the same position as the parent branch
		treeBranch->skipFirst=false;
		return;
	}
	else if(branch->scale<=0){ // Tie off the end
		if(!mCountMode){
			biba.set(mBranchIndexCount+0,mBranchVertexCount);
			biba.set(mBranchIndexCount+1,mBranchVertexCount);
		}
		mBranchIndexCount+=2;
		if(!mCountMode){
			lbiba.set(mLowBranchIndexCount+0,mBranchVertexCount);
			lbiba.set(mLowBranchIndexCount+1,mBranchVertexCount);
		}
		mLowBranchIndexCount+=2;
	}
	else if(treeBranch->started==false){
		treeBranch->started=true;

		if(!mCountMode){
			biba.set(mBranchIndexCount+0,treeBranch->lastVertex);
			biba.set(mBranchIndexCount+1,treeBranch->lastVertex);
		}
		mBranchIndexCount+=2;
		if(!mCountMode){
			lbiba.set(mLowBranchIndexCount+0,treeBranch->lastVertex);
			lbiba.set(mLowBranchIndexCount+1,treeBranch->lastVertex);
		}
		mLowBranchIndexCount+=2;

		for(i=0;i<=mSections;++i){
			if(!mCountMode){
				biba.set(mBranchIndexCount+0,treeBranch->lastVertex+i);
				biba.set(mBranchIndexCount+1,mBranchVertexCount+mSections+1+i);
			}
			mBranchIndexCount+=2;
			if((i%mLowMod)==0){
				if(!mCountMode){
					lbiba.set(mLowBranchIndexCount+0,treeBranch->lastVertex+i);
					lbiba.set(mLowBranchIndexCount+1,mBranchVertexCount+mSections+1+i);
				}
				mLowBranchIndexCount+=2;
			}
		}
	}
	else{
		for(i=0;i<=mSections;++i){
			if(!mCountMode){
				biba.set(mBranchIndexCount+0,mBranchVertexCount+i);
				biba.set(mBranchIndexCount+1,mBranchVertexCount+mSections+1+i);
			}
			mBranchIndexCount+=2;
			if((i%mLowMod)==0){
				if(!mCountMode){
					lbiba.set(mLowBranchIndexCount+0,mBranchVertexCount+i);
					lbiba.set(mLowBranchIndexCount+1,mBranchVertexCount+mSections+1+i);
				}
				mLowBranchIndexCount+=2;
			}
		}
	}

	for(i=0;i<=mSections;++i){
		if(!mCountMode){
			float f=(float)i/(float)mSections * Math::TWO_PI;
			Vector3 part1,part2;
			Math::mul(part1,branchNormal,Math::sin(f)*branch->scale);
			Math::mul(part2,branchBinormal,Math::cos(f)*branch->scale);

			Vector3 point;
			Math::add(point,part1);
			Math::add(point,part2);
			Math::add(point,branch->position);
			bvba.set3(mBranchVertexCount,0,point);

			Vector3 normal;
			Math::sub(normal,point,branch->position);
			Math::normalizeCarefully(normal,0);
			bvba.set3(mBranchVertexCount,1,normal);

			bvba.set2(mBranchVertexCount,2,(float)i/(float)mSections * 4,branch->barkTime * 4);
		}
		mBranchVertexCount++;
		treeBranch->lastVertex=mBranchVertexCount;
	}
}

void TreeSystem::branchLeaf(BranchSystem::Branch *branch,const Vector3 &offset,scalar scale){
	TreeBranch *treeBranch=(TreeBranch*)branch;

	Vector3 branchForward,branchNormal,branchBinormal;
	BranchSystem::getBranchVectors(branch,branchForward,branchNormal,branchBinormal);

	Math::normalizeCarefully(branchNormal,offset,mScene->getEpsilon());
	Math::mul(branchNormal,scale);

	Vector3 point;
	Math::add(point,branch->position,offset);

	scalar leafWiggleOffset=mSystem->getRandom()->nextFloat(0,1);

	if(!mCountMode){
		// Points are recalulated in alignLeaves
		// lvba.set3(mLeafVertexCount+0,0,point1);
		lvba.set3(mLeafVertexCount+0,1,branchForward);
		lvba.set2(mLeafVertexCount+0,2,0,Math::ONE);

		// lvba.set3(mLeafVertexCount+1,0,point2);
		lvba.set3(mLeafVertexCount+1,1,branchForward);
		lvba.set2(mLeafVertexCount+1,2,Math::ONE,Math::ONE);

		// lvba.set3(mLeafVertexCount+2,0,point3);
		lvba.set3(mLeafVertexCount+2,1,branchForward);
		lvba.set2(mLeafVertexCount+2,2,0,0);

		// lvba.set3(mLeafVertexCount+3,0,point4);
		lvba.set3(mLeafVertexCount+3,1,branchForward);
		lvba.set2(mLeafVertexCount+3,2,Math::ONE,0);

		liba.set(mLeafIndexCount+0,mLeafVertexCount+0);
		liba.set(mLeafIndexCount+1,mLeafVertexCount+1);
		liba.set(mLeafIndexCount+2,mLeafVertexCount+2);
		liba.set(mLeafIndexCount+3,mLeafVertexCount+2);
		liba.set(mLeafIndexCount+4,mLeafVertexCount+1);
		liba.set(mLeafIndexCount+5,mLeafVertexCount+3);

		int leafIndex=mLeafVertexCount/4;
		mLeaves[leafIndex].segment.setStartDir(point,branchNormal);
		mLeaves[leafIndex].wiggleOffset=leafWiggleOffset;

		treeBranch->leaves.add(leafIndex);
	}
	mLeafVertexCount+=4;
	mLeafIndexCount+=6;
}

void TreeSystem::alignLeaves(const Vector3 &eyePoint,const Vector3 &eyeForward,bool individual){
	Vector3 normal,up;

	lvba.lock(mLeafVertexBuffer,Buffer::Access_BIT_WRITE);
		int i;
		for(i=0;i<mLeaves.size();++i){
			const Segment &segment=mLeaves[i].segment;
			if(individual){
				Math::sub(normal,segment.origin,eyePoint);
				Math::normalizeCarefully(normal,mScene->getEpsilon());
				Math::cross(up,normal,segment.direction);
			}
			else{
				Math::cross(up,eyeForward,segment.direction);
			}

			lvba.set3(i*4+0,0,segment.origin+segment.direction+up/2);
			lvba.set3(i*4+1,0,segment.origin+segment.direction-up/2);
			lvba.set3(i*4+2,0,segment.origin+up/2);
			lvba.set3(i*4+3,0,segment.origin-up/2);
		}
	lvba.unlock();
}

void TreeSystem::wiggleUpdate(int dt){
	if(mHasWiggleLeaves){
		mHasWiggleLeaves=false; // Gets reset each run through

		Vector3 normal,up;

		Vector3 eyePoint=Math::ZERO_VECTOR3;
		Vector3 eyeForward=Math::X_UNIT_VECTOR3;
		bool individual=true;

		lvba.lock(mLeafVertexBuffer,Buffer::Access_BIT_WRITE);

		int i;
		for(i=0;i<mLeaves.size();++i){
			Leaf *leaf=&mLeaves[i];

			scalar time=leaf->wiggleTime;
			if(time>0){
				mHasWiggleLeaves=true;
				time-=dt;
				if(time<0) time=0;
				leaf->wiggleTime=time;
				scalar ftime=Math::fromMilli(time);

				Segment segment=leaf->segment;
				scalar l=Math::length(segment.direction);
				segment.direction.x+=Math::sin(ftime*8*leaf->wiggleOffset)*4;
				segment.direction.y+=Math::sin(ftime*8*leaf->wiggleOffset)*4;
				Math::normalizeCarefully(segment.direction,mScene->getEpsilon());
				segment.direction*=l;
				if(individual){
					Math::sub(normal,segment.origin,eyePoint);
					Math::normalizeCarefully(normal,mScene->getEpsilon());
					Math::cross(up,normal,segment.direction);
				}
				else{
					Math::cross(up,eyeForward,segment.direction);
				}

				lvba.set3(i*4+0,0,segment.origin+segment.direction+up/2);
				lvba.set3(i*4+1,0,segment.origin+segment.direction-up/2);
				lvba.set3(i*4+2,0,segment.origin+up/2);
				lvba.set3(i*4+3,0,segment.origin-up/2);
			}
		}

		lvba.unlock();
	}
}

void TreeSystem::calculateNormals(TreeBranch *branch){
	Vector3 tangent,neighborTangent,normal,binormal;

	int i;
	for(i=1;i<branch->getNumNeighbors()-1;++i){ // We do the start and end point separately
		TreeBranch *neighbor=(TreeBranch*)branch->getNeighbor(i);
		scalar neighborTime=branch->getNeighborTime(i);

		tangent=branch->tangents[branch->getTimeIndex(neighborTime)];
		neighborTangent=neighbor->tangents[0];

		Math::add(normal,tangent,neighborTangent);
		Math::normalizeCarefully(normal,mScene->getEpsilon());
		Math::neg(normal);

		Math::cross(binormal,tangent,normal);
		Math::normalizeCarefully(binormal,mScene->getEpsilon());
		Math::cross(normal,binormal,tangent);

		TOADLET_ASSERT(Math::length(normal)!=0);

		branch->normals[branch->getTimeIndex(neighborTime)]=normal;
		neighbor->normals[0]=normal;

		calculateNormals(neighbor);
	}

	branch->normals[branch->normals.size()-1]=Math::Z_UNIT_VECTOR3;

	TOADLET_ASSERT(Math::length(branch->normals[0])!=0);
	TOADLET_ASSERT(Math::length(branch->normals[branch->normals.size()-1])!=0);

	int j,k;
	for(i=0;i<branch->normals.size()-1;i=j){
		for(j=i+1;j<branch->normals.size();++j){
			if(Math::lengthSquared(branch->normals[j])>0) break;
		}

		for(k=i+1;k<j;++k){
			tangent=branch->tangents[k];
			Math::lerp(normal,branch->normals[i],branch->normals[j],(float)(k-i)/(float)(j-i));

			Math::cross(binormal,tangent,normal);
			Math::normalizeCarefully(binormal,mScene->getEpsilon());
			Math::cross(normal,binormal,tangent);

			branch->normals[k]=normal;
		}
	}
}

void TreeSystem::mergeBounds(TreeBranch *branch){
	int i;
	for(i=0;i<branch->children.size();++i){
		mergeBounds(branch->children[i]);
		branch->bound.merge(branch->children[i]->bound);
	}
}

bool TreeSystem::wiggleLeaves(const Sphere &bound,TreeSystem::TreeBranch *branch){
	if(branch==NULL){
		branch=mTreeBranches[0];
	}

	bool result=false;

	if(Math::testIntersection(bound,branch->bound)){
		int i;
		for(i=0;i<branch->children.size();++i){
			result|=wiggleLeaves(bound,branch->children[i]);
		}

		Vector3 endPoint;
		branch->getPoint(endPoint,branch->getLength());
		if(Math::testInside(bound,endPoint)){
			for(i=0;i<branch->leaves.size();++i){
				Leaf *leaf=&mLeaves[branch->leaves[i]];
				if(leaf->wiggleTime==0){
					leaf->wiggleTime=1000;
				}
			}
			mHasWiggleLeaves=true;
			result|=true;
		}
	}

	return result;
}

Path *TreeSystem::getClosestPath(Vector3 &closestPoint,const Vector3 &point){
	Sphere bound;
	Math::sub(bound.origin,point,mParent->getWorldTranslate());
	bound.radius=50;
	Path *path=getClosestPath(closestPoint,bound,mTreeBranches[0]);
	Math::add(closestPoint,mParent->getWorldTranslate());
	return path;
}

Path *TreeSystem::getClosestPath(Vector3 &closestPoint,const Sphere &bound,TreeBranch *path){
	if(Math::testIntersection(bound,path->bound)){
		Path *closestPath=NULL;
		scalar closestDistance=1000;

		Vector3 point;
		scalar dt=1.0;
		scalar t;
		for(t=0;t<path->getLength();t+=dt){
			path->getPoint(point,t);
			scalar d=Math::length(point,bound.origin);
			if(d<closestDistance){
				closestDistance=d;
				closestPath=path;
				closestPoint.set(point);
			}
		}

		int i;
		for(i=0;i<path->children.size();++i){
			Vector3 childPoint;
			Path *childPath=getClosestPath(childPoint,bound,path->children[i]);
			scalar childDistance=Math::length(childPoint,bound.origin);

			if(childPath!=NULL && childDistance<closestDistance){
				closestDistance=childDistance;
				closestPath=childPath;
				closestPoint.set(childPoint);
			}
		}

		return closestPath;
	}
	else{
		return NULL;
	}
}

void TreeSystem::transformChanged(Transform *transform){
	if(mParent==NULL){
		return;
	}

	mWorldBound->transform(mBound,mParent->getWorldTransform());
}

void TreeSystem::traceSegment(PhysicsCollision &result,const Vector3 &position,const Segment &segment,const Vector3 &size){
	traceSegment(result,position,segment,size,mTreeBranches[0]);
}

Path *TreeSystem::traceSegment(PhysicsCollision &result,const Vector3 &position,const Segment &segment,const Vector3 &size,TreeBranch *path){
	result.time=Math::findIntersection(segment,path->bound,result.point,result.normal);

	Path *closestPath;

	int i;
	for(i=0;i<path->children.size();++i){
		Path *childPath=traceSegment(result,position,segment,size,path->children[i]);

//		if(childPath!=NULL && childDistance<closestDistance){
	//		closestDistance=childDistance;
		//	closestPath=childPath;
			//closestPoint.set(childPoint);
		//}
	}

	return closestPath;
}

void TreeSystem::resetCounts(){
	mBranchVertexCount=0;
	mBranchIndexCount=0;
	mLowBranchIndexCount=0;
	mLeafVertexCount=0;
	mLeafIndexCount=0;
}
