#include "RandIsle.h"
#include "PathClimber.h"
#include "Tree.h"
#include "HUD.h"
#include "Resources.h"

#define TREE_CAMERA_DISTANCE 80

/// @todo: Use the leaf bump textures
/// @todo: Optimize iPad rendering so we don't get a speedup from the 'manual path' in render()

static const scalar epsilon=0.001f;

RandIsle::RandIsle(Application *app,String path):
	mMouseButtons(0),
	mXJoy(0),mYJoy(0),

	mPatchNoise(4,4,1,1,256)
{
	mApp=app;
	mPath=path;
}

RandIsle::~RandIsle(){
}

void RandIsle::create(){
	Logger::debug("RandIsle::create");

	mEngine=mApp->getEngine();
	mEngine->setDirectory(mPath);

	Resources::init(mEngine);

	mPatchSize=Resources::instance->patchSize;
	scalar scale=16*64/mPatchSize;
	mPatchScale.set(scale,scale,64);

	mScene=HopScene::ptr(new HopScene(mEngine));
	mScene->setUpdateListener(this);
	mScene->setRangeLogicDT(30,30);
	mScene->setExcessiveDT(100);
	mScene->setGravity(Vector3(0,0,-50));
	mScene->setEpsilon(0.03125);
	mScene->getSimulator()->setMicroCollisionThreshold(100);

	mTerrain=mEngine->createNodeType(TerrainNode::type(),mScene);
	mScene->setRoot(mTerrain);
	mScene->setTraceable(mTerrain);
	mTerrain->setListener(this);
	mTerrain->setTolerance(Resources::instance->tolerance);
	mTerrain->setCameraUpdateScope(Scope_BIT_MAIN_CAMERA);
	mTerrain->setWaterScope(Scope_BIT_WATER);
	mTerrain->setMaterialSource(Resources::instance->terrainMaterialSource);
	mTerrain->setWaterMaterial(Resources::instance->waterMaterial);
	mTerrain->setDataSource(this);

	mFollowNode=mEngine->createNodeType(ParentNode::type(),mScene);
	mFollower=SmoothFollower::ptr(new SmoothFollower(30));
	mFollower->setOffset(Vector3(0,-20,5));
	mFollower->setTargetOffset(Vector3(0,0,7.7));
	mFollowNode->addNodeListener(mFollower);
	mScene->getRoot()->attach(mFollowNode);

	mCamera=mEngine->createNodeType(CameraNode::type(),mScene);
	mCamera->setProjectionFovX(Math::degToRad(Math::fromInt(60)),Math::ONE,mCamera->getNearDist(),1024);
	mCamera->setScope(~Scope_HUD | Scope_BIT_MAIN_CAMERA);
	mCamera->setDefaultState(mEngine->getMaterialManager()->createRenderState());
	mCamera->setClearColor(Resources::instance->fadeColor);
	mCamera->getDefaultState()->setFogState(FogState(FogState::FogType_LINEAR,Math::ONE,mCamera->getFarDist()/2,mCamera->getFarDist(),mCamera->getClearColor()));
	mFollowNode->attach(mCamera);

	mReflectCamera=mEngine->createNodeType(CameraNode::type(),mScene);
	mReflectCamera->setRenderTarget(Resources::instance->reflectTarget);
	mReflectCamera->setProjectionFovX(Math::degToRad(Math::fromInt(60)),Math::ONE,mCamera->getNearDist(),mCamera->getFarDist());
	mReflectCamera->setScope(~Scope_HUD & ~Scope_BIT_MAIN_CAMERA & ~Scope_BIT_WATER);
	mReflectCamera->setDefaultState(mEngine->getMaterialManager()->createRenderState());
	mReflectCamera->setClearColor(Resources::instance->fadeColor);
	mReflectCamera->getDefaultState()->setFogState(FogState(FogState::FogType_LINEAR,Math::ONE,mCamera->getFarDist()/2,mCamera->getFarDist(),mCamera->getClearColor()));
	mScene->getRoot()->attach(mReflectCamera);

	mRefractCamera=mEngine->createNodeType(CameraNode::type(),mScene);
	mRefractCamera->setRenderTarget(Resources::instance->refractTarget);
	mRefractCamera->setProjectionFovX(Math::degToRad(Math::fromInt(60)),Math::ONE,mCamera->getNearDist(),mCamera->getFarDist());
	mRefractCamera->setScope(~Scope_HUD & ~Scope_BIT_MAIN_CAMERA & ~Scope_BIT_WATER);
	mRefractCamera->setDefaultState(mEngine->getMaterialManager()->createRenderState());
	mRefractCamera->setClearColor(Resources::instance->fadeColor);
	mRefractCamera->getDefaultState()->setFogState(FogState(FogState::FogType_LINEAR,Math::ONE,mCamera->getFarDist()/2,mCamera->getFarDist(),mCamera->getClearColor()));
	mScene->getRoot()->attach(mRefractCamera);

	mHUD=mEngine->createNodeType(HUD::type(),mScene);
	mHUD->setProjectionOrtho(-1,1,-1,1,-1,1);
	mHUD->setClearFlags(0);
	mHUD->setScope(Scope_HUD);
	mScene->getRoot()->attach(mHUD);

	mSky=(Sky*)mEngine->allocNodeType(Sky::type())->create(mScene,Resources::instance->cloudSize,Resources::instance->skyColor,Resources::instance->fadeColor);
	mScene->getBackground()->attach(mSky);

	VertexBuffer::ptr predictedVertexBuffer=mEngine->getBufferManager()->createVertexBuffer(Buffer::Usage_BIT_STREAM,Buffer::Access_BIT_WRITE,mEngine->getVertexFormats().POSITION_COLOR,512);
	mPredictedVertexData=VertexData::ptr(new VertexData(predictedVertexBuffer));
	mPredictedIndexData=IndexData::ptr(new IndexData(IndexData::Primitive_TRISTRIP,NULL,0,predictedVertexBuffer->getSize()));
	mPredictedMaterial=mEngine->getMaterialManager()->createMaterial();
	mPredictedMaterial->getPass()->setDepthState(DepthState(DepthState::DepthTest_NEVER,false));
	mPredictedMaterial->getPass()->setRasterizerState(RasterizerState(RasterizerState::CullType_NONE));
	mPredictedMaterial->getPass()->setMaterialState(MaterialState(true));
	mPredictedMaterial->getPass()->setBlendState(BlendState::Combination_ALPHA);

	mRustleSound=mEngine->createNodeType(AudioNode::type(),mScene);
	mRustleSound->setAudioBuffer(Resources::instance->rustle);
	mScene->getRoot()->attach(mRustleSound);
	
	mPlayer=mEngine->createNodeType(PathClimber::type(),mScene);
	mPlayer->setPathClimberListener(this);
	mPlayer->addShape(Shape::ptr(new Shape(AABox(2))));
	{
		Segment segment;
		segment.origin.set(10,0,1000);
		segment.direction.set(0,0,-2000);
		tadpole::Collision result;
		mScene->traceSegment(result,segment,-1,mPlayer);
		result.point.z+=mPlayer->getShape()->getSphere().radius;
		mPlayer->setPosition(result.point);
	}
	mScene->getRoot()->attach(mPlayer);
	mTerrain->setTarget(mPlayer);

	mHUD->setTarget(mPlayer,mCamera);
	mFollower->setTarget(mFollowNode,mPlayer);
	mTerrain->setUpdateTargetBias(128/(getPatchSize()*getPatchScale().x));

	Logger::alert("Populating terrain");
	while(updatePopulatePatches());

	Logger::alert("Updating terrain");
	mTerrain->updatePatches(mCamera);

	mPlayer->setSpeed(40);

	InputDevice *joyDevice=mApp->getInputDevice(InputDevice::InputType_JOY);
	if(joyDevice!=NULL){
		joyDevice->setListener(this);
		joyDevice->start();
	}

	Logger::debug("RandIsle::create finished");
}

void RandIsle::destroy(){
	Logger::debug("RandIsle::destroy");

	InputDevice *joyDevice=mApp->getInputDevice(InputDevice::InputType_JOY);
	if(joyDevice!=NULL){
		joyDevice->stop();
		joyDevice->setListener(NULL);
	}

	mScene->destroy();

	if(mPredictedVertexData!=NULL){
		mPredictedVertexData->destroy();
		mPredictedVertexData=NULL;
	}
	if(mPredictedIndexData!=NULL){
		mPredictedIndexData->destroy();
		mPredictedIndexData=NULL;
	}
	if(mPredictedMaterial!=NULL){
		mPredictedMaterial->release();
		mPredictedMaterial=NULL;
	}

	Resources::destroy();

	Logger::debug("RandIsle::destroy finished");
}

void RandIsle::resized(int width,int height){
	if(mCamera!=NULL && width>0 && height>0){
		if(width>=height){
			scalar ratio=Math::div(Math::fromInt(width),Math::fromInt(height));
			mCamera->setProjectionFovY(Math::degToRad(Math::fromInt(75)),ratio,mCamera->getNearDist(),mCamera->getFarDist());
			mReflectCamera->setProjectionFovY(Math::degToRad(Math::fromInt(75)),ratio,mCamera->getNearDist(),mCamera->getFarDist()*2);
		}
		else{
			scalar ratio=Math::div(Math::fromInt(height),Math::fromInt(width));
			mCamera->setProjectionFovX(Math::degToRad(Math::fromInt(75)),ratio,mCamera->getNearDist(),mCamera->getFarDist());
			mReflectCamera->setProjectionFovX(Math::degToRad(Math::fromInt(75)),ratio,mCamera->getNearDist(),mCamera->getFarDist()*2);
		}
		mCamera->setViewport(0,0,width,height);
	}
	if(mHUD!=NULL && width>0 && height>0){
		if(width>=height){
			scalar ratio=Math::div(Math::fromInt(width),Math::fromInt(height));
			mHUD->setProjectionOrtho(-ratio,ratio,-Math::ONE,Math::ONE,-Math::ONE,Math::ONE);
		}
		else{
			scalar ratio=Math::div(Math::fromInt(height),Math::fromInt(width));
			mHUD->setProjectionOrtho(-Math::ONE,Math::ONE,-ratio,ratio,-Math::ONE,Math::ONE);
		}
		mHUD->setViewport(0,0,width,height);
	}
}

void RandIsle::render(){
	Matrix4x4 matrix;

	Vector3 position=mCamera->getWorldTranslate();
	Vector3 forward=mCamera->getForward();
	Vector3 up=mCamera->getUp();

	mRefractCamera->setProjectionMatrix(mReflectCamera->getProjectionMatrix());
	mRefractCamera->setLookDir(position,forward,up);
	mRefractCamera->updateAllWorldTransforms();
	matrix.reset();
	Math::setMatrix4x4FromTranslate(matrix,0,0,5);
	Math::preMul(matrix,mRefractCamera->getViewMatrix());
	mRefractCamera->setObliqueNearPlaneMatrix(matrix);

	position.z*=-1;
	forward.z*=-1;
	up.z*=-1;

	mReflectCamera->setLookDir(position,forward,up);
	mReflectCamera->updateAllWorldTransforms();
	matrix.reset();
	Math::setMatrix4x4FromX(matrix,Math::PI);
	Math::setMatrix4x4FromTranslate(matrix,0,0,-5);
	Math::preMul(matrix,mReflectCamera->getViewMatrix());
	mReflectCamera->setObliqueNearPlaneMatrix(matrix);

	RenderDevice *device=mApp->getRenderDevice();
	device->beginScene();
	{
		mRefractCamera->render(device);

		mReflectCamera->render(device);

		mCamera->render(device);

		mHUD->render(device);
	}
	device->endScene();
	device->swap();
}

void RandIsle::update(int dt){
	mScene->update(dt);
}

void RandIsle::logicUpdate(int dt){
	// Find graph
	if(mPlayer->getPath()==NULL){
		// TODO: Use sensor
		if(mPlayer->getHealth()>0 && mPlayer->getNoClimbTime()<mScene->getLogicTime()){
			// Find closest graph & mount point
			float snapDistance=10;
			Sphere playerSphere=Sphere(mPlayer->getPosition(),snapDistance);
			ParentNode *closestSystem=NULL;
			float closestDistance=1000;
			PathSystem::Path *closestPath=NULL;
			Vector3 closestPoint;
			Node *node;
			for(node=mScene->getRoot()->getFirstChild();node!=NULL;node=node->getNext()){
				if((node->getScope()==Scope_TREE) && node->getWorldBound().testIntersection(playerSphere)){
					Tree *system=(Tree*)node;
					Vector3 point;
					PathSystem::Path *path=system->getClosestPath(point,mPlayer->getPosition());
					float distance=Math::length(point,mPlayer->getPosition());

					if(path!=NULL && distance<closestDistance){
						closestDistance=distance;
						closestPath=path;
						closestSystem=system;
						closestPoint.set(point);
					}
				}
			}

			if(closestPath!=NULL && closestDistance<snapDistance){
				mPlayer->mount(closestSystem,closestPath,closestPoint);
				findPathSequence(mPathSequence,mPlayer,mPlayer->getPath(),mPlayer->getPathDirection(),mPlayer->getPathTime());
			}
		}
	}

	// Update graph prediction & camera
	if(mPlayer->getPath()!=NULL){
		PathSystem::Path *path=mPlayer->getPath();
		int direction=mPlayer->getPathDirection();
		scalar time=mPlayer->getPathTime();

		if(mPathSequence.size()>0){
			scalar neighborTime=path->getNeighborTime(mPathSequence[0]);
			if(neighborTime>time && direction<0){
				mPlayer->setPathDirection(1);
			}
			else if(neighborTime<time && direction>0){
				mPlayer->setPathDirection(-1);
			}
		}

		if(mPlayer->getSpeed()>=5){
			wiggleLeaves((Tree*)mPlayer->getMounted(),mPlayer->getWorldBound().getSphere());
		}

		findPathSequence(mPathSequence,mPlayer,mPlayer->getPath(),mPlayer->getPathDirection(),mPlayer->getPathTime());

		updatePredictedPath();

		Vector3 position;
		Math::mul(position,mPlayer->getIdealRotation(),Math::Y_UNIT_VECTOR3);
		Math::mul(position,-TREE_CAMERA_DISTANCE);
		Math::add(position,mPlayer->getMounted()->getWorldTranslate());
		position.z+=mPlayer->getMounted()->getBound().getAABox().maxs.z/2;

		mFollower->logicUpdated(position,dt);
	}

//	updateDanger(dt);

	scalar offset=mPlayer->getBound().getMins().z-2;
	bool inWater=(mPlayer->getPosition().z+offset)<=0;
	if(!inWater && mPlayer->getCoefficientOfGravity()==0){
		mPlayer->setCoefficientOfGravity(Math::ONE);
	}
	else if(inWater){
		Vector3 velocity=mPlayer->getVelocity();
		velocity.z=0;
		mPlayer->setVelocity(velocity);
		mPlayer->setCoefficientOfGravity(0);
		if(mPlayer->getPosition().z+offset<0){
			Vector3 position=mPlayer->getPosition();
			position.z=-offset;
			mPlayer->setPosition(position);
		}
	}

	if(mScene->getLogicFrame()%4==0){
		updatePopulatePatches();
	}

	mScene->logicUpdate(dt);

	playerMove(-mXJoy/8.0,-mYJoy*80);

	mApp->setTitle(String("FPS:")+mCamera->getFramesPerSecond()+" VISIBLE:"+mCamera->getVisibleCount()+" ACTIVE:"+mScene->countActiveNodes());
}

void RandIsle::frameUpdate(int dt){
	Vector3 right,forward,up;
	Math::setAxesFromQuaternion(mPlayer->getRotate(),right,forward,up);
#if 0
	if(tl || tr){
		Matrix3x3 dr;
		if(tl && !tr){
			Math::setMatrix3x3FromZ(dr,Math::fromMilli(dt)*3);
		}
		else if(tr && !tl){
			Math::setMatrix3x3FromZ(dr,-Math::fromMilli(dt)*3);
		}
		Quaternion q;
		Math::setQuaternionFromMatrix3x3(q,dr);
		Quaternion idealRotation=mPlayer->getIdealRotation();
		Math::postMul(idealRotation,q);
		Math::normalizeCarefully(idealRotation,epsilon);
		mPlayer->setIdealRotation(idealRotation);
	}
#endif

	mScene->frameUpdate(dt);

/*
	Material::ptr waterMaterial=Resources::instance->waterMaterial;
	if(waterMaterial!=NULL){
		TextureState textureState;
		waterMaterial->getPass()->getTextureState(Shader::ShaderType_FRAGMENT,0,textureState);
		Math::setMatrix4x4FromTranslate(textureState.matrix,Math::sin(Math::fromMilli(mScene->getTime())/4)/4,0,0);
		waterMaterial->getPass()->setTextureState(Shader::ShaderType_FRAGMENT,0,textureState);

		waterMaterial->getPass()->getTextureState(Shader::ShaderType_FRAGMENT,1,textureState);
		Math::setMatrix4x4FromTranslate(textureState.matrix,0,Math::sin(Math::fromMilli(mScene->getTime())/4)/4,0);
		waterMaterial->getPass()->setTextureState(Shader::ShaderType_FRAGMENT,1,textureState);
	}
*/
	if(mPlayer->getPath()!=NULL){
		// In this case the listener is not attached, so we need to update it after the rest of the scene
		mFollower->frameUpdated(mFollowNode,dt);
	}
}

void RandIsle::updateDanger(int dt){
	int dangerDelayTime=0;
	int dangerTime=5000;

	scalar oldDanger=mPlayer->getDanger();
	scalar danger=0;
	if(mPlayer->getGroundTime()>0){
		int time=mScene->getLogicTime()-mPlayer->getGroundTime();
		if(time>=dangerDelayTime){
			danger=Math::fromMilli(time-dangerDelayTime)/Math::fromMilli(dangerTime);
		}
	}

	if(danger<oldDanger){
		danger=oldDanger-Math::fromMilli(dt);
		if(danger<0){
			danger=0;
		}
	}

	danger=Math::clamp(0,Math::ONE,danger);

	mPlayer->setDanger(danger);
}

void RandIsle::updatePredictedPath(){
	if(mPlayer->getPath()!=NULL && mPathSequence.size()>0){
		// Update predicted path
		PathSystem::Path *path=mPlayer->getPath();
		scalar oldTime=0,time=mPlayer->getPathTime();
		int direction=mPlayer->getPathDirection();

		Vector3 point;
		Vector3 forward,right,tangent,normal,scale;
		scalar predictTime=400;
		Vector4 color(0x00FF00FF);

		VertexBufferAccessor vba(mPredictedVertexData->getVertexBuffer(0),Buffer::Access_BIT_WRITE);

		scalar nextTime=path->getNeighborTime(mPathSequence[0]);

		float dt=1.0;
		int np=0,i=0;
		for(i=0;i<vba.getSize()-2 && path!=NULL && predictTime>0;i+=2){
			path->getPoint(point,time);
			Math::add(point,mPlayer->getMounted()->getWorldTranslate());
			path->getOrientation(tangent,normal,scale,time);

			Math::sub(forward,point,mCamera->getWorldTranslate());
			Math::cross(right,forward,tangent);Math::normalizeCarefully(right,epsilon);

			vba.set3(i+0,0,point-right);
			vba.set3(i+1,0,point+right);

			vba.setRGBA(i+0,1,color.getRGBA());
			vba.setRGBA(i+1,1,color.getRGBA());

			bool skipCheck=(time==nextTime);

			predictTime-=dt;
			oldTime=time;
			time+=dt*direction;

			if(skipCheck==false && mPlayer->passedJunction(direction,oldTime,time,nextTime)){
				// Make sure we get a point on extactly each junction
				predictTime+=Math::abs(time-nextTime); 
				time=nextTime;
			}
			else if(skipCheck==true || mPlayer->passedJunction(direction,oldTime,time,nextTime)){
				scalar extraTime=Math::abs(time-nextTime);
				PathSystem::Path  *nextPath=path->getNeighbor(mPathSequence[np]);
				np++;
				if(np<mPathSequence.size()){
					nextTime=nextPath->getNeighborTime(mPathSequence[np]);

					time=nextPath->getNeighborTime(path);
					direction=(nextTime>time)?1:-1;
					time+=direction*extraTime;
					// We have to check the direction again, in case we passed our destination with the extraTime
					direction=(nextTime>time)?1:-1;
				}
				path=nextPath;
			}
		}
		mPredictedIndexData->count=i;

		vba.unlock();
	}
}

void RandIsle::keyPressed(int key){
	if(key==Application::Key_ESC){
		mApp->stop();
	}
	else if(key==' '){
		playerJump();
	}
}

void RandIsle::keyReleased(int key){
}

void RandIsle::mousePressed(int x,int y,int button){
	mMouseButtons|=1<<button;

	mApp->setDifferenceMouse(true);
}

void RandIsle::mouseReleased(int x,int y,int button){
	mMouseButtons&=~(1<<button);

	mApp->setDifferenceMouse(false);
}

void RandIsle::mouseMoved(int x,int y){
	scalar xamount=(float)x/(float)mApp->getWidth();
	scalar yamount=(float)y/(float)mApp->getHeight();

	if(mMouseButtons>0){
		playerMove(-xamount*3,-yamount*80);
	}
}

void RandIsle::inputDetected(const InputData &data){
	if(data.type==InputDevice::InputType_JOY){
		if((data.valid&(1<<InputData::Semantic_JOY_BUTTON_PRESSED))!=0){
			playerJump();
		}
		if((data.valid&(1<<InputData::Semantic_JOY_DIRECTION))!=0){
			mXJoy=data.values[InputData::Semantic_JOY_DIRECTION].x;
			mYJoy=data.values[InputData::Semantic_JOY_DIRECTION].y;
		}
	}
}

void RandIsle::playerJump(){
	if(mPlayer->getPath()==NULL){
		Segment segment;
		segment.setStartDir(mPlayer->getPosition(),Vector3(0,0,-5));
		tadpole::Collision result;
		mScene->traceSegment(result,segment,-1,mPlayer);
		if(result.time<Math::ONE){
			Math::mul(result.normal,40);
			mPlayer->setVelocity(mPlayer->getVelocity()+result.normal);
		}
	}
	else{
		mPlayer->dismount();
	}
}

void RandIsle::playerMove(scalar dr,scalar ds){
	Matrix3x3 drm;
	Math::setMatrix3x3FromZ(drm,dr);
	Quaternion q;
	Math::setQuaternionFromMatrix3x3(q,drm);
	Quaternion idealRotation=mPlayer->getIdealRotation();
	Math::postMul(idealRotation,q);
	Math::normalizeCarefully(idealRotation,epsilon);
	mPlayer->setIdealRotation(idealRotation);

	scalar speed=mPlayer->getSpeed();
	speed+=ds;
	speed=Math::clamp(0,80,speed);
	mPlayer->setSpeed(speed);
}

float RandIsle::findPathSequence(Collection<int> &sequence,PathClimber *player,PathSystem::Path *path,int direction,scalar time){
	Vector3 right,forward,up;
	Math::setAxesFromQuaternion(player->getIdealRotation(),right,forward,up);
	forward.z+=0.25;
	Math::normalizeCarefully(forward,epsilon);
	sequence.clear();
	scalar result=findPathSequence(sequence,player,forward,NULL,path,direction,time,true);
	return result;
}

float RandIsle::findPathSequence(Collection<int> &sequence,PathClimber *player,const Vector3 &forward,PathSystem::Path *previous,PathSystem::Path *path,int direction,scalar time,bool first){
	if(path==NULL){
		return 0;
	}

	scalar closestd=0;
	Collection<int> closestp;
	Vector3 point,tangent;
	path->getPoint(point,time);
	int i;
	for(i=0;i<path->getNumNeighbors();++i){
		PathSystem::Path *next=path->getNeighbor(i);
		scalar nextTime=path->getNeighborTime(i);
		if(next!=NULL && next==previous){
			continue;
		}

		path->getPoint(tangent,nextTime);
		Math::sub(tangent,point);

		scalar d=0;
		Collection<int> p;
		d=Math::dot(tangent,forward);
		d+=findPathSequence(p,player,forward,path,next,0,next!=NULL?next->getNeighborTime(path):0,false);

		p.insert(p.begin(),i);

		if(closestp.size()==0 || d>closestd){
			closestd=d;
			closestp=p;
		}
	}

	sequence=closestp;
	return closestd;
}

void RandIsle::wiggleLeaves(Tree *tree,const Sphere &bound){
	Sphere localBound;
	Math::sub(localBound,bound,tree->getWorldTranslate());
	localBound.radius=20.0f;
	if(tree->wiggleLeaves(localBound) && mRustleSound->getPlaying()==false){
		mRustleSound->setGain(Math::ONE);
		mRustleSound->play();
	}
}

void RandIsle::pathMounted(PathClimber *climber){
//	((Tree*)climber->getGraph())->setHighlight(true);
	mFollowNode->removeNodeListener(mFollower);
}

void RandIsle::pathDismounted(PathClimber *climber){
	mFollowNode->addNodeListener(mFollower);
//	((Tree*)climber->getGraph())->setHighlight(false);
}

int RandIsle::atJunction(PathClimber *climber,PathSystem::Path *current,PathSystem::Path *next){
	findPathSequence(mPathSequence,mPlayer,mPlayer->getPath(),mPlayer->getPathDirection(),mPlayer->getPathTime());

	if(mPathSequence.size()>0 && current->getNeighbor(mPathSequence[0])==next){
		mPathSequence.removeAt(0);
		if(mPathSequence.size()>0){
			if(next->getNeighborTime(mPathSequence[0])>current->getNeighborTime(next)){
				return 1;
			}
			else{
				return -1;
			}
		}
		else{
			return 1;
		}
	}
	else{
		return 0;
	}
}

bool RandIsle::updatePopulatePatches(){
	int i;
	for(i=0;i<mPopulatePatches.size();i++){
		PopulatePatch *patch=&mPopulatePatches[i];
		int step=12;

		if(patch->y<0){
			if(patch->dy>=0){
				patch->y=0;
			}
			else{
				patch->y=getPatchSize()-step;
			}
		}

		// TODO: Could I replace some of this math with Terrain->toWorldXY()?
		float ty=(float)(patch->py*mPatchSize+patch->y - mPatchSize/2)/(float)mPatchSize;
		float tx=pathValue(ty);

		float wx=tx*mPatchSize*mPatchScale.x;
		float wy=ty*mPatchSize*mPatchScale.y;

		Segment segment;
		segment.origin.set(wx,wy,1000);
		segment.direction.set(0,0,-2000);
		tadpole::Collision result;
		mScene->traceSegment(result,segment,-1,NULL);
		if(result.time<Math::ONE && patch->bound.testIntersection(result.point)){
			result.point.z-=5;
			Tree::ptr tree=(Tree*)(new Tree())->create(mScene,wx+wy,mScene->getRoot(),result.point);
		}

		if(patch->dy>=0){
			patch->y+=step;
		}
		else{
			patch->y-=step;
		}

		if((patch->dy>=0 && patch->y>=getPatchSize()) || (patch->dy<0 && patch->y<=0)){
			mPopulatePatches.removeAt(i);
			i--;
		}
	}

	return mPopulatePatches.size()>0;
}

void RandIsle::terrainPatchCreated(int px,int py,const Bound &bound){
	mPopulatePatches.add(PopulatePatch(px,py,px-mTerrain->getTerrainX(),py-mTerrain->getTerrainY(),bound));
}

void RandIsle::terrainPatchDestroyed(int px,int py,const Bound &bound){
	// TODO: Use bounding volume sensor
	Node::ptr node;
	for(node=mScene->getRoot()->getFirstChild();node!=NULL;node=node->getNext()){
		if((node->getScope()==Scope_TREE) && Math::testInside(bound.getAABox(),node->getWorldTranslate())){
			node->destroy();
		}
	}
}

bool RandIsle::getPatchHeightData(scalar *data,int px,int py){
	int size=getPatchSize();
	for(int x=0;x<size;++x){
		for(int y=0;y<size;++y){
			float tx=(float)(px*size+x - size/2)/(float)size;
			float ty=(float)(py*size+y - size/2)/(float)size;
			scalar v=terrainValue(tx,ty);
			data[y*size+x]=v;
		}
	}
	return true;
}

bool RandIsle::getPatchLayerData(tbyte *data,int px,int py){
	int size=getPatchSize();
	for(int x=0;x<size;++x){
		for(int y=0;y<size;++y){
			float tx=(float)(px*size+x - size/2)/(float)size;
			float ty=(float)(py*size+y - size/2)/(float)size;
			scalar v=terrainValue(tx,ty);
			int layer=0;
			if(v<-0.0){
				layer=0;
			}
			else if(v<0.15){
					layer=1;
			}
			else{
				layer=2;
			}
			data[y*size+x]=Resources::instance->terrainMaterialSource->getDiffuseTexture(layer)!=NULL?layer:0;
		}
	}
	return true;
}

scalar RandIsle::terrainValue(float tx,float ty){
	float path=pathValue(ty);
	float value=mPatchNoise.perlin2(tx/2,ty/2);
	float fade=Math::abs(tx-path)*1.5f;
	fade=Math::clamp(0,1,fade);
	value=Math::lerp(value,-1,fade) + 0.5f;
	return value;
}

scalar RandIsle::pathValue(float ty){
	return mPatchNoise.perlin1(ty/4);
}

Applet *createApplet(Application *app){
	String path;
	String lookFor="/grass.png";
	const char *paths[]={"../../res","../res","res",".",NULL};
	int i;
	for(i=0;paths[i]!=NULL;++i){
		if(FileStream(paths[i]+lookFor,FileStream::Open_BIT_READ|FileStream::Open_BIT_BINARY).closed()==false){
			path=paths[i];
			break;
		}
	}

	return new RandIsle(app,path);
}

void destroyApplet(Applet *applet){delete applet;}
