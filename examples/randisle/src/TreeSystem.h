#ifndef TREESYSTEM_H
#define TREESYSTEM_H

#include "Path.h"
#include "BranchSystem.h"

class TreeSystem:public BranchSystem,public BranchSystem::BranchListener{
public:
	TOADLET_OBJECT(TreeSystem);

	class TreeBranch:public BranchSystem::Branch,public PathVertex,public PathEdge{
	public:
		TOADLET_IOBJECT(TreeBranch);

		TreeBranch():skipFirst(false),started(false),lastVertex(-1){
			branches.add(this);
		}

		virtual ~TreeBranch(){}

		virtual PointerIteratorRange<PathEdge> getEdges() const{return PointerIteratorRange<PathEdge>(branches);}
		virtual scalar getLength() const{return length;}
		virtual void getPoint(Vector3 &point,scalar time) const{
			scalar t=lerp(times.data(),times.size(),time);
			int it=Math::toInt(t);t-=Math::fromInt(it);
			Math::lerp(point,points[it],points[it+1],t);
		}
		virtual void getOrientation(Vector3 &tangent,Vector3 &normal,Vector3 &scale,scalar time) const{
			scalar t=lerp(times.data(),times.size(),time);
			int it=Math::toInt(t);t-=Math::fromInt(it);
			Math::lerp(tangent,tangents[it],tangents[it+1],t);
			Math::normalize(tangent);
			Math::lerp(normal,normals[it],normals[it+1],t);
			Math::normalize(normal);
			scalar s=Math::lerp(scales[it],scales[it+1],t);
			scale.set(s,s,s);
		}

		virtual PathVertex *getVertex(bool end) const{return end?const_cast<TreeBranch*>(this):parent;}
		virtual scalar getTime(bool end) const{return end?0:parentTime;}

		int getTimeIndex(scalar time) const{return Math::toInt(lerp(times.data(),times.size(),time));}

		const AABox &getBound() const{return bound;}

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
		float parentTime;
		PointerCollection<TreeBranch> branches;
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

	TreeSystem(Engine *engine,int seed,Material::ptr branchMaterial,Material::ptr leafMaterial,BranchSystem::BranchListener *listener=NULL);
	void destroy();

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

	const AABox &getBound() const{return mTreeBranches[0]->getBound();}

	Mesh *getMesh() const{return mMesh;}
	Mesh *getLowMesh() const{return mLowMesh;}

	TreeBranch *getClosestBranch(Vector3 &closestPoint,const Sphere &bound,TreeBranch *path=NULL);
	TreeBranch *traceSegment(PhysicsCollision &result,const Vector3 &position,const Segment &segment,const Vector3 &size,TreeBranch *path=NULL);

	VertexBufferAccessor bvba,lvba;
	IndexBufferAccessor biba,lbiba,liba;

protected:
	void resetCounts();

	Engine *mEngine;

	int mSections;
	bool mCountMode;

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