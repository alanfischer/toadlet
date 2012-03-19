#include "PathClimber.h"

static const scalar epsilon=0.001f;

PathClimber::PathClimber():BaseComponent(),
	mPreviousPath(NULL),
	mPath(NULL),
	mPathTime(0),
	mPathDirection(0),
	mPassedNeighbor(0),
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

bool PathClimber::parentChanged(Node *node){
	mNode=(HopEntity*)node;
	if(mNode!=NULL){
		mScene=(HopScene*)mNode->getScene();
	}
	return true;
}

void PathClimber::logicUpdate(int dt,int scope){
	if(mPath==NULL){
		Quaternion rotate;
		Vector3 right,forward,up;

		Math::mul(forward,mIdealRotation,Math::Y_UNIT_VECTOR3);

		Segment segment;
		segment.setStartDir(mNode->getTranslate(),Vector3(0,0,-5));
		tadpole::Collision result;
		mScene->traceSegment(result,segment,-1,mNode);
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
		mNode->setRotate(rotate);

		forward.set(0,mSpeed,mNode->getVelocity().z);
		if(mSpeed>0 && Math::length(mNode->getVelocity())==0){ // Check to see if we're stuck on the terrain
			forward.z=40;
		}
		Math::mul(forward,mIdealRotation);
		mNode->setVelocity(forward);

//		if((mSolid->getTouching()!=NULL || getCoefficientOfGravity()==0) && mGroundTime==0){
//			mGroundTime=mScene->getLogicTime();
//		}
	}
	else{
		scalar oldPathTime=mPathTime;
		mPathTime+=mPathDirection*Math::mul(Math::fromMilli(dt),mSpeed);
		scalar pathTime=mPathTime;
		scalar neighborTime=mPath->getNeighborTime(mPassedNeighbor+mPathDirection);
		if(passedJunction(mPathDirection,oldPathTime,pathTime,neighborTime)){
			mPassedNeighbor+=mPathDirection;
			PathSystem::Path *neighbor=mPath->getNeighbor(mPassedNeighbor);
			if(neighbor==NULL){
				dismount();
			}
			else{
				int direction=0;
				if(mPathClimberListener!=NULL){
					direction=mPathClimberListener->atJunction(this,mPath,neighbor);
				}
				if(direction!=0){
					scalar extraTime=Math::abs(mPathTime-neighborTime);

					mPreviousPath=mPath;
					mPath=neighbor;
					mPathDirection=direction;

					// I believe there is a case where we could use the extraTime, and end up passing our next destination
					// I would like this code to be closer to RandIsle::updatePredictedPath, ideally a function in PathClimber which Squirrel would then use
					mPathTime=mPath->getNeighborTime(mPreviousPath)+direction*extraTime;

					mPassedNeighbor=findPassedNeighbor(mPath,mPathDirection,mPathTime);
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
		mNode->setRotate(rotate);

		Math::add(result,mMounted->getTranslate());
		Math::setAxesFromQuaternion(mNode->getRotate(),right,forward,up);
		result=result+up*(Math::maxVal(0,scale.x-1)-mNode->getBound().mins.z);
		mNode->setTranslate(result);
	}
}

void PathClimber::mount(Node *system,PathSystem::Path *path,const Vector3 &point){
	dismount();

	mMounted=system;
	mPreviousPath=NULL;
	mPath=path;
	mNode->setCoefficientOfGravity(0);

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

	bool flip=false;
//	if(mGroundTime>0){
//		flip=Math::dot(ndir,Math::Z_UNIT_VECTOR3)<0;
//	}
//	else{
		flip=Math::dot(segment.direction,forward)<0;
//	}
	mPathDirection=flip?-1:1;

	mPassedNeighbor=findPassedNeighbor(mPath,mPathDirection,mPathTime);

	mNode->setVelocity(Math::ZERO_VECTOR3);

//	mGroundTime=0;

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
		mNode->setRotate(rotation);
	}
	mNode->setVelocity(forward*30+Vector3(0,0,40));

	mMounted=NULL;
	mPath=NULL;
	mNode->setCoefficientOfGravity(Math::ONE);
	mNoClimbTime=mScene->getLogicTime()+500;
}

void PathClimber::setPathDirection(int direction){
	mPathDirection=direction;
	mPassedNeighbor=findPassedNeighbor(mPath,mPathDirection,mPathTime);
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

int PathClimber::findPassedNeighbor(PathSystem::Path *path,int direction,scalar time){
	int passedNeighbor=0;
	int i;
	if(direction>0){
		passedNeighbor=path->getNumNeighbors()-1;
		for(i=0;i<path->getNumNeighbors();++i){
			if(path->getNeighborTime(i)>=time){
				passedNeighbor=i-1;
				break;
			}
		}
	}
	else{
		passedNeighbor=0;
		for(i=path->getNumNeighbors()-1;i>=0;--i){
			if(path->getNeighborTime(i)<=time){
				passedNeighbor=i+1;
				break;
			}
		}
	}
	return passedNeighbor;
}

void PathClimber::findRotation(Quaternion &r,const Vector3 &tangent,const Vector3 &normal){
	Vector3 forward,up,right;
	if(Math::lengthSquared(normal)<epsilon){
		if(Math::lengthSquared(tangent,Math::Z_UNIT_VECTOR3)<epsilon || Math::lengthSquared(tangent,Math::NEG_Z_UNIT_VECTOR3)<epsilon){
			Math::setAxesFromQuaternion(mIdealRotation,right,forward,up);
			Math::cross(up,right,tangent);
		}
		else{
			Math::cross(right,tangent,normal);
			Math::normalizeCarefully(right,epsilon);
			Math::cross(up,right,tangent);
		}
	}
	else{
		Math::cross(right,tangent,normal);
		Math::normalizeCarefully(right,epsilon);
		Math::cross(up,right,tangent);
	}

	Math::setQuaternionFromAxes(r,right,tangent,up);
}

bool PathClimber::passedJunction(int direction,scalar oldTime,scalar newTime,scalar junctionTime){
	return direction>0?(oldTime<=junctionTime && newTime>junctionTime):(oldTime>=junctionTime && newTime<junctionTime);
}
