#include "PathClimber.h"

static const scalar epsilon=0.001f;

PathClimber::PathClimber():BaseComponent(),
	mPreviousPath(NULL),
	mPath(NULL),
	mPathTime(0),
	mPathDirection(0),
	//mPassedNeighbor(,
	mNoClimbTime(0),
	mSpeed(0),
	//mIdealRotation,
	//mPreviousNormals,
	mNormalIndex(0)
{
	mPreviousNormals.resize(10);
	int i;
	for(i=0;i<mPreviousNormals.size();++i){
		mPreviousNormals[i].set(Math::Z_UNIT_VECTOR3);
	}
}

void PathClimber::logicUpdate(int dt,int scope){
	if(mPath==NULL){
		Quaternion rotate;
		Vector3 right,forward,up;

		Math::mul(forward,mIdealRotation,Math::Y_UNIT_VECTOR3);

		Segment segment;
		segment.setStartDir(mParent->getTranslate(),Vector3(0,0,-5));
		PhysicsCollision result;
		mParent->getScene()->traceSegment(result,segment,-1,mParent);
		if(result.time<Math::ONE){
			up.set(result.normal);
		}
		else{
			up.set(Math::Z_UNIT_VECTOR3);
		}
		mPreviousNormals[(mNormalIndex++)%mPreviousNormals.size()].set(up);
		up.reset();
		int i;
		for(i=0;i<mPreviousNormals.size();++i){
			Math::add(up,mPreviousNormals[i]);
		}
		Math::div(up,Math::fromInt(mPreviousNormals.size()));

		Math::cross(right,forward,up);
		Math::normalizeCarefully(right,epsilon);
		Math::cross(forward,up,right);
		Math::setQuaternionFromAxes(rotate,right,forward,up);
		mParent->setRotate(rotate);

		forward.set(0,mSpeed,mParent->getPhysics()->getVelocity().z);
		if(mSpeed>0 && Math::length(mParent->getPhysics()->getVelocity())==0){ // Check to see if we're stuck on the terrain
			forward.z=40;
		}
		Math::mul(forward,mIdealRotation);
		mParent->getPhysics()->setVelocity(forward);
	}
	else{
		scalar oldPathTime=mPathTime;
		mPathTime+=mPathDirection*Math::mul(Math::fromMilli(dt),mSpeed);

		PathEdge *edge=mPassedEdge;
		if(mPathDirection>0){
			PathVertex::iterator begin=mPath->getEdges().begin(),end=mPath->getEdges().end();
			for(PathVertex::iterator it=begin;it!=end;++it){
				if(it==edge){
					edge=++it;
					break;
				}
			}
		}
		else{
			PathVertex::iterator begin=mPath->getEdges().end(),end=mPath->getEdges().begin();--begin,--end;
			for(PathVertex::iterator it=begin;it!=end;++it){
				if(it==edge){
					edge=--it;
					break;
				}
			}
		}
		scalar neighborTime=edge->getTime(edge->getVertex(true)==mPath);

		if(passedJunction(mPathDirection,oldPathTime,mPathTime,neighborTime)){
			mPassedEdge=edge;
			if(mPassedEdge->getVertex(true)==NULL){
				dismount();
			}
			else{
				int direction=0;
				if(mPathClimberListener!=NULL){
					direction=mPathClimberListener->atJunction(this,mPath,edge->getVertex(edge->getVertex(false)==mPath));
				}
				if(direction!=0){
					scalar extraTime=Math::abs(mPathTime-neighborTime);

					mPreviousPath=mPath;
					mPath=edge->getVertex(mPathDirection==1);
					mPathDirection=direction;

					// I believe there is a case where we could use the extraTime, and end up passing our next destination
					// I would like this code to be closer to RandIsle::updatePredictedPath, ideally a function in PathClimber which Squirrel would then use
					mPathTime=edge->getTime(mPathDirection==1)+direction*extraTime;

					mPassedEdge=findPassedEdge(mPath,mPathDirection,mPathTime);
				}
			}
		}
	}

	if(mPath!=NULL){
		Vector3 result;
		Vector3 right,forward,up,tangent,normal,scale;

		mPath->getPoint(result,mPathTime);
		mPath->getOrientation(tangent,normal,scale,mPathTime);
		tangent*=mPathDirection;

		Quaternion rotate;
		findRotation(rotate,tangent,normal);
		mParent->setRotate(rotate);

		Math::add(result,mMounted->getTranslate());
		Math::setAxesFromQuaternion(mParent->getRotate(),right,forward,up);
		result=result+up*(scale.x-mParent->getPhysics()->getBound()->getAABox().mins.z);
		mParent->getPhysics()->setPosition(result); // Set the physics position, instead of the node position, to take advantage of the physics lerping
	}
}

void PathClimber::mount(Node *system,PathVertex *path,const Vector3 &point){
	dismount();

	mMounted=system;
	mPreviousPath=NULL;
	mPath=path;
	mParent->getPhysics()->setGravity(0);
	mParent->getPhysics()->setVelocity(Math::ZERO_VECTOR3);

	Segment segment;
	path->getPoint(segment.origin,0);
	path->getPoint(segment.direction,path->getLength());
	Math::sub(segment.direction,segment.origin);
	Vector3 ndir;Math::normalizeCarefully(ndir,segment.direction,epsilon);
	Vector3 right,forward,up;
	Math::setAxesFromQuaternion(mIdealRotation,right,forward,up);

	Vector3 localPoint;
	Math::sub(localPoint,point,system->getTranslate());
	mPathTime=Math::length(localPoint,segment.origin);

	bool flip=Math::dot(segment.direction,forward)<0;
	mPathDirection=flip?-1:1;

	mPassedEdge=findPassedEdge(mPath,mPathDirection,mPathTime);

	if(mPathClimberListener!=NULL){
		mPathClimberListener->pathMounted(this);
	}
}

void PathClimber::dismount(){
	if(mPath==NULL){
		return;
	}

	if(mPathClimberListener!=NULL){
		mPathClimberListener->pathDismounted(this);
	}

	Vector3 right,forward,up,scale;
	mPath->getOrientation(forward,up,scale,mPathTime);
	forward*=mPathDirection;
	forward.z=0;
	if(Math::normalizeCarefully(forward,epsilon)){
		Quaternion rotation;
		up.set(Math::Z_UNIT_VECTOR3);
		Math::cross(right,forward,up);
		Math::setQuaternionFromAxes(rotation,right,forward,up);
		setIdealRotation(rotation);
		mParent->setRotate(rotation);
	}
	mParent->getPhysics()->setVelocity(forward*30+Vector3(0,0,40));

	mMounted=NULL;
	mPath=NULL;
	mParent->getPhysics()->setGravity(Math::ONE);
	mNoClimbTime=mParent->getScene()->getLogicTime()+500;
}

void PathClimber::setPathDirection(int direction){
	mPathDirection=direction;
	mPassedEdge=findPassedEdge(mPath,mPathDirection,mPathTime);
}

void PathClimber::setIdealRotation(const Quaternion &idealRotation){
	Vector3 right,forward,up;
	Math::setAxesFromQuaternion(idealRotation,right,forward,up);

	up.set(Math::Z_UNIT_VECTOR3);
	Math::cross(forward,up,right);
	Math::normalizeCarefully(forward,epsilon);
	Math::cross(right,forward,up);
	
	Math::setQuaternionFromAxes(mIdealRotation,right,forward,up);
}

PathEdge *PathClimber::findPassedEdge(PathVertex *vertex,int direction,scalar time){
	PathEdge *edge=NULL;
	if(direction>0){
		edge=vertex->getEdges().end()-1;
		PathVertex::iterator begin=vertex->getEdges().end(),end=vertex->getEdges().begin();--begin,--end;
		for(PathVertex::iterator it=begin;it!=end;--it){
			if(it->getTime(it->getVertex(true)==vertex)>=time){
				edge=it;
				break;
			}
		}
	}
	else{
		edge=vertex->getEdges().begin();
		PathVertex::iterator begin=vertex->getEdges().begin(),end=vertex->getEdges().end();
		for(PathVertex::iterator it=begin;it!=end;++it){
			if(it->getTime(it->getVertex(true)==vertex)<=time){
				edge=it;
				break;
			}
		}
	}
	return edge;
}

void PathClimber::findRotation(Quaternion &r,const Vector3 &tangent,const Vector3 &normal){
	Vector3 forward,up,right;
	if(Math::lengthSquared(normal)<epsilon){
		if(Math::lengthSquared(tangent,Math::Z_UNIT_VECTOR3)<epsilon || Math::lengthSquared(tangent,Math::NEG_Z_UNIT_VECTOR3)<epsilon){
			Math::setAxesFromQuaternion(mIdealRotation,right,forward,up);
			Math::cross(up,right,tangent);
		}
		else{
			Math::cross(right,normal,tangent);
			Math::normalizeCarefully(right,epsilon);
			Math::cross(up,right,tangent);
		}
	}
	else{
		Math::cross(right,normal,tangent);
		Math::normalizeCarefully(right,epsilon);
		Math::cross(up,right,tangent);
	}

	Math::setQuaternionFromAxes(r,right,tangent,up);
}

bool PathClimber::passedJunction(int direction,scalar oldTime,scalar newTime,scalar junctionTime){
	return direction>0?(oldTime<=junctionTime && newTime>junctionTime):(oldTime>=junctionTime && newTime<junctionTime);
}
