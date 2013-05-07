#ifndef TREESYSTEM_H
#define TREESYSTEM_H

#include <toadlet/peeper.h>
#include <toadlet/tadpole.h>
#include "Path.h"
#include "BranchSystem.h"

class TreeSystem:public BaseComponent,public BranchSystem::BranchListener,public Spacial,public DetailTraceable{
public:
	TOADLET_OBJECT(TreeSystem);

	class TreeBranch:public BranchSystem::Branch,public Path{
	public:
		TOADLET_OBJECT(TreeBranch);

		TreeBranch():skipFirst(false),started(false),lastVertex(-1){}

		virtual ~TreeBranch(){}

		virtual scalar getLength(){return length;}
		virtual void getPoint(Vector3 &point,scalar time){
			scalar t=lerp(times.begin(),times.size(),time);
			int it=Math::toInt(t);t-=Math::fromInt(it);
			Math::lerp(point,points[it],points[it+1],t);
		}
		virtual void getOrientation(Vector3 &tangent,Vector3 &normal,Vector3 &scale,scalar time){
			scalar t=lerp(times.begin(),times.size(),time);
			int it=Math::toInt(t);t-=Math::fromInt(it);
			Math::lerp(tangent,tangents[it],tangents[it+1],t);
			Math::normalize(tangent);
			Math::lerp(normal,normals[it],normals[it+1],t);
			Math::normalize(normal);
			scalar s=Math::lerp(scales[it],scales[it+1],t);
			scale.set(s,s,s);
		}
		virtual int getTimeIndex(scalar time){
			scalar t=lerp(times.begin(),times.size(),time);
			return Math::toInt(t);
		}

		// Count start and end as neighbors, even if they have null paths
		virtual int getNumNeighbors(){return children.size()+2;}
		virtual Path *getNeighbor(int i){
			if(i==0){
				return parent;
			}
			else if(i-1<children.size()){
				return children[i-1];
			}
			else{
				return NULL;
			}
		}
		virtual scalar getNeighborTime(int i){
			if(i==0){
				return 0;
			}
			else if(i-1<children.size()){
				return childrenTimes[i-1];
			}
			else{
				return length;
			}
		}
		virtual scalar getNeighborTime(Path *neighbor){
			if(neighbor==parent){
				return 0;
			}
			else{
				int i;
				for(i=0;i<children.size();++i){
					if(children[i]==neighbor){
						return childrenTimes[i];
					}
				}
				return 0;
			}
		}

		// Returns x.y, where x,x+1 are the points to lerp between, and y is the amount
		static scalar lerp(scalar *times,int length,scalar time){
			if(length<=1) return 0;
			int i;
			time=Math::clamp(times[0],times[length-1],time);
			for(i=1;time-times[i]>0;++i);
			time=Math::div(time-times[i-1],times[i]-times[i-1]);
			return (i-1)+time;
		}

		bool skipFirst;
		bool started;
		int lastVertex;
		Collection<Vector3> points;
		Collection<Vector3> tangents;
		Collection<Vector3> normals;
		Collection<scalar> scales;
		Collection<scalar> times;
		TreeBranch *parent;
		Collection<TreeBranch::ptr> children;
		Collection<scalar> childrenTimes;
		Collection<int> leaves;
		AABox bound;
	};

	class Leaf{
	public:
		Leaf():wiggleTime(0),wiggleOffset(0){}

		Segment segment;
		int wiggleTime;
		scalar wiggleOffset;
	};

	TreeSystem(Scene *scene,int seed);
	void destroy();

	void parentChanged(Node *node);

	void grow();

	BranchSystem::Branch::ptr branchCreated(BranchSystem::Branch *parent);
	void branchDestroyed(BranchSystem::Branch *branch);
	void branchBuild(BranchSystem::Branch *branch);
	void branchLeaf(BranchSystem::Branch *branch,const Vector3 &offset,scalar scale);
	void alignLeaves(const Vector3 &eyePoint,const Vector3 &eyeForward,bool individual);
	void calculateNormals(TreeBranch *branch);
	void mergeBounds(TreeBranch *branch);
	bool wiggleLeaves(const Sphere &bound,TreeBranch *branch=NULL);
	void wiggleUpdate(int dt);

	Path *getClosestPath(Vector3 &closestPoint,const Vector3 &point);
	Path *getClosestPath(Vector3 &closestPoint,const Sphere &bound,TreeBranch *path);

	Mesh *getMesh() const{return mMesh;}
	Mesh *getLowMesh() const{return mLowMesh;}

	// Spacial
	Transform *getTransform() const{return NULL;}
	Bound *getBound() const{return mBound;}
	Transform *getWorldTransform() const{return NULL;}
	Bound *getWorldBound() const{return mWorldBound;}
	void transformChanged(Transform *transform);

	// DetailTraceable
	Bound *getTraceableBound() const{return mBound;}
	void traceSegment(PhysicsCollision &result,const Vector3 &position,const Segment &segment,const Vector3 &size);

	VertexBufferAccessor bvba,lvba;
	IndexBufferAccessor biba,lbiba,liba;

protected:
	void resetCounts();

	Path *traceSegment(PhysicsCollision &result,const Vector3 &position,const Segment &segment,const Vector3 &size,TreeBranch *path);

	Engine *mEngine;
	Scene *mScene;

	Bound::ptr mBound;
	Bound::ptr mWorldBound;

	int mSections;
	bool mCountMode;

	BranchSystem::ptr mSystem;
	Collection<TreeBranch::ptr> mTreeBranches;
	Collection<int> mBranchVertexStart;
	Collection<int> mBranchNodeStart;
	Collection<bool> mBranchStarted;

	int mBranchVertexCount;
	int mBranchIndexCount;
	int mLowBranchIndexCount;
	VertexBuffer::ptr mBranchVertexBuffer;
	IndexBuffer::ptr mBranchIndexBuffer;
	IndexBuffer::ptr mLowBranchIndexBuffer;
	Material::ptr mBranchMaterial;

	int mLeafVertexCount;
	int mLeafIndexCount;
	VertexBuffer::ptr mLeafVertexBuffer;
	IndexBuffer::ptr mLeafIndexBuffer;
	Material::ptr mLeafMaterial;
	Collection<Leaf> mLeaves;
	bool mHasWiggleLeaves;

	int mLowMod;
	Mesh::ptr mMesh;
	Mesh::ptr mLowMesh;
};

#endif