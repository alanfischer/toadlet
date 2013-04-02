#include "RandIsle.h"
#include "PathClimber.h"
#include "TreeSystem.h"
#include "HUD.h"
#include "Resources.h"
#include <toadlet/tadpole/plugins/HopManager.h>
#include <toadlet/tadpole/plugins/HopComponent.h>
#include <toadlet/tadpole/plugins/DecalShadowRenderManager.h>

#define TREE_CAMERA_DISTANCE 80

static const scalar epsilon=0.001f;

/// @todo: Fix:
///  - Android GLES1/2 swapping not working due to different GL libraries being loaded, have to use glesem

/// @todo: These could be removed if nodes supported propagating scopes, similar to worldTransform, worldScope
void clearScopeBit(Node *node,int scope){
	node->setScope(node->getScope()&~scope);
	for(int i=0;i<node->getNumNodes();++i){
		clearScopeBit(node->getNode(i),scope);
	}
}

void setScopeBit(Node *node,int scope){
	node->setScope(node->getScope()|scope);
	for(int i=0;i<node->getNumNodes();++i){
		setScopeBit(node->getNode(i),scope);
	}
}

RandIsle::RandIsle(Application *app,String path):
	mMouseButtons(0),
	mXJoy(0),mYJoy(0),

	mLastPredictedTime(0),

	mPatchNoise(4,4,1,1,256)
{
	mApp=app;
	mPath=path;
}

RandIsle::~RandIsle(){
}

void RandIsle::create(){
	Log::debug("RandIsle::create");

	mEngine=mApp->getEngine();
	mEngine->getArchiveManager()->addDirectory(mPath);

	Resources::init(mEngine);

	mPatchSize=Resources::instance->patchSize;
	scalar scale=16*64/mPatchSize;
	mPatchScale.set(scale,scale,64);

	mScene=new Scene(mEngine);
	mScene->setUpdateListener(this);

	DecalShadowRenderManager::ptr renderManager=new DecalShadowRenderManager(mScene);
	renderManager->setShadowScope(Scope_BIT_SHADOW);
	mScene->setRenderManager(renderManager);

	Simulator *simulator=((HopManager*)mScene->getPhysicsManager())->getSimulator();
	simulator->setGravity(Vector3(0,0,-50));
	simulator->setEpsilon(0.05);
	simulator->setMicroCollisionThreshold(100);

	mTerrain=new TerrainNode(mScene);
	mScene->setRoot(mTerrain);
	mScene->getPhysicsManager()->setTraceable(mTerrain);
	mTerrain->setListener(this);
	mTerrain->setTolerance(Resources::instance->tolerance);
	mTerrain->setCameraUpdateScope(Scope_BIT_MAIN_CAMERA);
	mTerrain->setWaterScope(Scope_BIT_WATER);
	mTerrain->setWaterTransparentScope(Scope_BIT_WATER_TRANSPARENT);
	mTerrain->setMaterialSource(Resources::instance->terrainMaterialSource);
	mTerrain->setWaterMaterial(Resources::instance->waterMaterial);
	mTerrain->setDataSource(this);

	mFollowNode=new Node(mScene);
	mFollower=new SmoothFollower(20);
	mFollower->setOffset(Vector3(0,-20,5));
	mFollower->setTargetOffset(Vector3(0,0,7.7));
	mFollowNode->attach(mFollower);
	mScene->getRoot()->attach(mFollowNode);

	mCamera=new Camera();
//	shared_static_cast<StereoscopicCamera>(mCamera)->setCrossEyed(true);
	mCamera->setAutoProjectionFov(Math::degToRad(Math::fromInt(60)),false,mCamera->getNearDist(),1024);
	mCamera->setScope(~Scope_BIT_HUD | Scope_BIT_MAIN_CAMERA & ~Scope_BIT_WATER_TRANSPARENT);
	mCamera->setDefaultState(mEngine->getMaterialManager()->createRenderState());
	mCamera->setClearColor(Resources::instance->fadeColor);
	mCamera->getDefaultState()->setFogState(FogState(FogState::FogType_LINEAR,Math::ONE,mCamera->getFarDist()/2,mCamera->getFarDist(),mCamera->getClearColor()));
	mFollowNode->attach(new CameraComponent(mCamera));

	if(Resources::instance->reflectTarget!=NULL){
		mReflectCamera=new Camera();
		mReflectCamera->setRenderTarget(Resources::instance->reflectTarget);
		mReflectCamera->setAutoProjectionFov(Math::degToRad(Math::fromInt(60)),false,mCamera->getNearDist(),mCamera->getFarDist());
		mReflectCamera->setScope(~Scope_BIT_HUD & ~Scope_BIT_MAIN_CAMERA & ~Scope_BIT_WATER & ~Scope_BIT_WATER_TRANSPARENT);
		mReflectCamera->setDefaultState(mEngine->getMaterialManager()->createRenderState());
		mReflectCamera->setClearColor(Resources::instance->fadeColor);
		mReflectCamera->getDefaultState()->setFogState(FogState(FogState::FogType_LINEAR,Math::ONE,mCamera->getFarDist()/2,mCamera->getFarDist(),mCamera->getClearColor()));
	}

	if(Resources::instance->refractTarget!=NULL){
		mRefractCamera=new Camera();
		mRefractCamera->setRenderTarget(Resources::instance->refractTarget);
		mRefractCamera->setAutoProjectionFov(Math::degToRad(Math::fromInt(60)),false,mCamera->getNearDist(),mCamera->getFarDist());
		mRefractCamera->setScope(~Scope_BIT_HUD & ~Scope_BIT_MAIN_CAMERA & ~Scope_BIT_WATER | Scope_BIT_WATER_TRANSPARENT);
		mRefractCamera->setDefaultState(mEngine->getMaterialManager()->createRenderState());
		mRefractCamera->setClearColor(Resources::instance->fadeColor);
		mRefractCamera->getDefaultState()->setFogState(FogState(FogState::FogType_LINEAR,Math::ONE,mCamera->getFarDist()/2,mCamera->getFarDist(),mCamera->getClearColor()));
	}

	mHUD=new HUD(mScene,mPlayer,mCamera);
	clearScopeBit(mHUD,Scope_BIT_SHADOW);
	setScopeBit(mHUD,Scope_BIT_HUD);
	mScene->getRoot()->attach(mHUD);

	mHUDCamera=new Camera();
	mHUDCamera->setProjectionOrtho(-1,1,-1,1,-1,1);
	mHUDCamera->setClearFlags(0);

	mSky=new Sky(mScene,Resources::instance->cloudSize,Resources::instance->skyColor,Resources::instance->fadeColor);
	clearScopeBit(mSky,Scope_BIT_SHADOW);
	mScene->getBackground()->attach(mSky);

	VertexBuffer::ptr predictedVertexBuffer=mEngine->getBufferManager()->createVertexBuffer(Buffer::Usage_BIT_STREAM,Buffer::Access_BIT_WRITE,mEngine->getVertexFormats().POSITION_COLOR,512);
	mPredictedVertexData=VertexData::ptr(new VertexData(predictedVertexBuffer));
	mPredictedIndexData=IndexData::ptr(new IndexData(IndexData::Primitive_TRISTRIP,NULL,0,predictedVertexBuffer->getSize()));
	RenderState::ptr renderState=mEngine->getMaterialManager()->createRenderState();
	renderState->setDepthState(DepthState(DepthState::DepthTest_NEVER,false));
	renderState->setRasterizerState(RasterizerState(RasterizerState::CullType_NONE));
	renderState->setMaterialState(MaterialState(true));
	renderState->setBlendState(BlendState::Combination_ALPHA);
	mPredictedMaterial=mEngine->createDiffuseMaterial(NULL,renderState);

	mRustleSound=new AudioComponent(mEngine);
	mRustleSound->setAudioBuffer(Resources::instance->rustle);
	mScene->getRoot()->attach(mRustleSound);
	mRustleSound->play();
	
	mPlayer=new Node(mScene);
	{
		PathClimber *climber=new PathClimber();
		{
			climber->setName("climber");
			climber->setPathClimberListener(this);
		}
		mPlayer->attach(climber);

		PhysicsComponent *physics=mScene->getPhysicsManager()->createPhysicsComponent();
		{
			physics->setBound(new Bound(AABox(2)));

			Segment segment;
			segment.setStartDir(Vector3(10,0,1000),Vector3(0,0,-2000));
			PhysicsCollision result;
			mScene->traceSegment(result,segment,-1,mPlayer);
			result.point.z+=mPlayer->getBound()->getSphere().radius + 5;
			physics->setPosition(result.point);
		}
		mPlayer->attach(physics);

		MeshComponent *mesh=new MeshComponent(mEngine);
		if(Resources::instance->creature!=NULL){
			mesh->setMesh(Resources::instance->creature);
		}
		mPlayer->attach(mesh);

		AnimationAction *jumpAction=new AnimationAction();
		{
			jumpAction->attach(mesh->getSkeleton()->getAnimation(1));
			jumpAction->setCycling(AnimationAction::Cycling_LOOP);
			jumpAction->setStopGently(true);
		}
		mPlayer->attach(new ActionComponent("jump",jumpAction));
	}
	mScene->getRoot()->attach(mPlayer);

	mTerrain->setTarget(mPlayer);

	mMountBound=new Bound();
	mBoundSensor=new BoundingVolumeSensor(mScene);

	mFollower->setTarget(mPlayer);
	mTerrain->setUpdateTargetBias(128/(getPatchSize()*getPatchScale().x));

	Log::alert("Adding props");
	mProps=new Node(mScene);
	for(int i=0;i<Resources::instance->numProps;++i){
		Node::ptr prop=new Node(mScene);
		MeshComponent *mesh=new MeshComponent(mEngine);
		mesh->setMesh(Resources::instance->grass);
		prop->attach(mesh);
		prop->setScope(prop->getScope()&~Scope_BIT_SHADOW);

		mProps->attach(prop);
	}
	mProps->setScope(mProps->getScope()&~Scope_BIT_SHADOW);
	mScene->getRoot()->attach(mProps);
	updateProps();

	Log::alert("Populating terrain");
	while(updatePopulatePatches());

	Log::alert("Updating terrain");
	mTerrain->updatePatches(mCamera);

	InputDevice *joyDevice=mApp->getInputDevice(InputDevice::InputType_JOY);
	if(joyDevice!=NULL){
		joyDevice->setListener(this);
		joyDevice->start();
	}

	Log::debug("RandIsle::create finished");
}

void RandIsle::destroy(){
	Log::debug("RandIsle::destroy");

	InputDevice *joyDevice=mApp->getInputDevice(InputDevice::InputType_JOY);
	if(joyDevice!=NULL){
		joyDevice->stop();
		joyDevice->setListener(NULL);
	}

	if(mScene!=NULL){
		mScene->destroy();
		mScene=NULL;

		Resources::destroy();
	}

	if(mPredictedVertexData!=NULL){
		mPredictedVertexData->destroy();
		mPredictedVertexData=NULL;
	}
	if(mPredictedIndexData!=NULL){
		mPredictedIndexData->destroy();
		mPredictedIndexData=NULL;
	}
	if(mPredictedMaterial!=NULL){
		mPredictedMaterial->destroy();
		mPredictedMaterial=NULL;
	}

	Log::debug("RandIsle::destroy finished");
}

void RandIsle::render(){
	Matrix4x4 matrix;

	Vector3 position=mCamera->getPosition();
	Vector3 forward=mCamera->getForward();
	Vector3 up=mCamera->getUp();

	if(mRefractCamera!=NULL){
		mRefractCamera->setProjectionMatrix(mCamera->getProjectionMatrix());
		mRefractCamera->setLookDir(position,forward,up);
		matrix.reset();
		Math::setMatrix4x4FromTranslate(matrix,0,0,5);
		Math::preMul(matrix,mRefractCamera->getViewMatrix());
		mRefractCamera->setObliqueNearPlaneMatrix(matrix);
	}

	position.z=mTerrain->getWaterLevel()*2-position.z;
	forward.z*=-1;
	up.z*=-1;

	if(mReflectCamera!=NULL){
		mReflectCamera->setProjectionMatrix(mCamera->getProjectionMatrix());
		mReflectCamera->setLookDir(position,forward,up);
		matrix.reset();
		Math::setMatrix4x4FromX(matrix,Math::PI);
		Math::setMatrix4x4FromTranslate(matrix,0,0,-5);
		Math::preMul(matrix,mReflectCamera->getViewMatrix());
		/// @todo: Fix d3d10 obliqness
		//mReflectCamera->setObliqueNearPlaneMatrix(matrix);
	}

	RenderDevice *device=mEngine->getRenderDevice();
	device->beginScene();
	{
		if(mRefractCamera!=NULL && mRefractCamera->ready()){
			mRefractCamera->render(device,mScene);
		}

		if(mReflectCamera!=NULL && mReflectCamera->ready()){
			mReflectCamera->render(device,mScene);
		}

		mCamera->render(device,mScene);

		mHUDCamera->render(device,mScene,mHUD);
	}
	device->endScene();
	device->swap();
}

void RandIsle::update(int dt){
	mScene->update(dt);
}

void RandIsle::logicUpdate(int dt){
	PathClimber *climber=(PathClimber*)mPlayer->getChild("climber");
	PhysicsComponent *physics=mPlayer->getPhysics();

	updateClimber(climber,dt);

//	updateDanger(dt);

	{
		Vector3 windVector(0,0.1f,0);
		float waveLength=0.1f;
		float time=mScene->getTime()/1000.0f;
		TextureState state;
		state.calculation=TextureState::CalculationType_NORMAL;
		Math::setMatrix4x4FromTranslateRotateScale(state.matrix,time*windVector,Quaternion(),Vector3(1.0/waveLength,1.0/waveLength,1.0/waveLength));

		Shader::ShaderType type=(Shader::ShaderType)0;
		int index=0;
		Resources::instance->waterMaterial->getPass()->findTexture(type,index,"waveTex");
		Resources::instance->waterMaterial->getPass()->setTextureState(type,index,state);
	}

	updateProps();

	scalar offset=mPlayer->getBound()->getAABox().getMins().z-2;
	bool inWater=(physics->getPosition().z+offset)<=0;
	if(!inWater && !climber->getMounted() && physics->getGravity()==0){
		physics->setGravity(Math::ONE);
	}
	else if(inWater){
		Vector3 velocity=physics->getVelocity();
		velocity.z=0;
		physics->setVelocity(velocity);
		physics->setGravity(0);
		if(physics->getPosition().z+offset<0){
			Vector3 position=physics->getPosition();
			position.z=-offset;
			physics->setPosition(position);
		}
	}

	if(climber->getSpeed()<0.05){
		if(mPlayer->getActionActive("jump")){
			mPlayer->stopAction("jump");
		}
	}
	else{
		if(mPlayer->getActionActive("jump")==false){
			mPlayer->startAction("jump");
		}
	}

	if(mScene->getLogicFrame()%4==0){
		updatePopulatePatches();
	}

	mScene->logicUpdate(dt);

	playerMove(mPlayer,-mXJoy/8.0,-mYJoy*80);

	mApp->setTitle(String("FPS:")+mCamera->getFramesPerSecond()+" VISIBLE:"+mCamera->getVisibleCount()+" ACTIVE:"+mScene->countActiveNodes());
}

void RandIsle::frameUpdate(int dt){
	mScene->frameUpdate(dt);
}

void RandIsle::updateDanger(int dt){
/*
	int dangerDelayTime=0;
	int dangerTime=5000;

	scalar oldDanger=mClimber->getDanger();
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
*/
}

void RandIsle::updateProps(){
	scalar maxDist=Resources::instance->maxPropDist;
	scalar minDist=Resources::instance->minPropDist;
	Segment segment;
	Vector2 origin(mPlayer->getPhysics()->getPosition().x,mPlayer->getPhysics()->getPosition().y);
	Random r(System::mtime());
	for(int i=0;i<mProps->getNumNodes();++i){
		Node *prop=mProps->getNode(i);
		Vector2 propOrigin(prop->getWorldTranslate().x,prop->getWorldTranslate().y);
		scalar d=Math::length(propOrigin,origin);
		scalar a=Math::ONE-(d-minDist)/(maxDist-minDist);

		if(a<0 || propOrigin==Math::ZERO_VECTOR2){
			scalar p=r.nextScalar(0,Math::TWO_PI);
			if(propOrigin==Math::ZERO_VECTOR2){
				d=r.nextScalar(0,maxDist);
			}
			else{
				d=maxDist;
			}
			Vector3 offset(Math::sin(p)*d,Math::cos(p)*d,0);
			segment.setStartDir(Vector3(origin.x+offset.x,origin.y+offset.y,1000),Vector3(0,0,-2000));
			PhysicsCollision result;
			mScene->traceSegment(result,segment,-1,mPlayer);
			prop->setTranslate(result.point);
			prop->setRotate(Math::Z_UNIT_VECTOR3,r.nextScalar(0,Math::TWO_PI));
			scalar a=Math::ONE-(d-minDist)/(maxDist-minDist);
		}

		if(prop->getWorldTranslate().z<mTerrain->getWaterLevel()){
			a=0;
		}

		for(int j=0;j<prop->getNumVisibles();++j){
			prop->getVisible(j)->getSharedRenderState()->setMaterialState(MaterialState(Vector4(Math::ONE,Math::ONE,Math::ONE,a)));
		}
	}
}

void RandIsle::updateClimber(PathClimber *climber,int dt){
	// Find graph
	if(climber->getPath()==NULL){
		if(climber->getNoClimbTime()<mScene->getLogicTime()){
			// Find closest graph & mount point
			float snapDistance=10;
			Node *closestNode=NULL;
			float closestDistance=1000;
			Path *closestPath=NULL;
			Vector3 closestPoint;

			mMountBound->set(mPlayer->getTranslate(),snapDistance);
			mBoundSensor->setBound(mMountBound);
			SensorResults::ptr results=mBoundSensor->sense();

			for(int i=0;i<results->size();++i){
				Node *node=results->at(i);
				TreeSystem *system=(TreeSystem*)node->getChild("system");
				if(system!=NULL){
					Vector3 point;
					Path *path=system->getClosestPath(point,mPlayer->getTranslate());
					float distance=Math::length(point,mPlayer->getTranslate());

					if(path!=NULL && distance<closestDistance){
						closestDistance=distance;
						closestPath=path;
						closestNode=node;
						closestPoint.set(point);
					}
				}
			}

			if(closestPath!=NULL && closestDistance<snapDistance){
				climber->mount(closestNode,closestPath,closestPoint);
				mRustleSound->play();
				findPathSequence(mPathSequence,climber,climber->getPath(),climber->getPathDirection(),climber->getPathTime());
			}
		}
	}

	// Update graph prediction & camera
	if(climber->getPath()!=NULL){
		Path *path=climber->getPath();
		int direction=climber->getPathDirection();
		scalar time=climber->getPathTime();

		if(mPathSequence.size()>0){
			scalar neighborTime=path->getNeighborTime(mPathSequence[0]);
			if(neighborTime>time && direction<0){
				climber->setPathDirection(1);
			}
			else if(neighborTime<time && direction>0){
				climber->setPathDirection(-1);
			}
		}

		if(Math::length(mLastPredictedRotation,climber->getIdealRotation())>epsilon || mLastPredictedTime+250<mScene->getLogicTime()){
			mLastPredictedRotation=climber->getIdealRotation();
			mLastPredictedTime=mScene->getLogicTime();
			findPathSequence(mPathSequence,climber,climber->getPath(),climber->getPathDirection(),climber->getPathTime());
			updatePredictedPath(climber,dt);
		}

		Vector3 position;
		Math::mul(position,climber->getIdealRotation(),Math::Y_UNIT_VECTOR3);
		Math::mul(position,-TREE_CAMERA_DISTANCE);
		Math::add(position,climber->getMounted()->getWorldTranslate());
		position.z+=climber->getMounted()->getBound()->getAABox().maxs.z/2;

		mFollower->logicUpdate(position,dt);
	}
}

void RandIsle::updatePredictedPath(PathClimber *climber,int dt){
	if(climber->getPath()!=NULL && mPathSequence.size()>0){
		// Update predicted path
		Path *path=climber->getPath();
		scalar oldTime=0,time=climber->getPathTime();
		int direction=climber->getPathDirection();

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
			Math::add(point,climber->getMounted()->getWorldTranslate());
			path->getOrientation(tangent,normal,scale,time);

			Math::sub(forward,point,mCamera->getPosition());
			Math::cross(right,forward,tangent);Math::normalizeCarefully(right,epsilon);

			vba.set3(i+0,0,point-right);
			vba.set3(i+1,0,point+right);

			vba.setRGBA(i+0,1,color.getRGBA());
			vba.setRGBA(i+1,1,color.getRGBA());

			bool skipCheck=(time==nextTime);

			predictTime-=dt;
			oldTime=time;
			time+=dt*direction;

			if(skipCheck==false && climber->passedJunction(direction,oldTime,time,nextTime)){
				// Make sure we get a point on extactly each junction
				predictTime+=Math::abs(time-nextTime); 
				time=nextTime;
			}
			else if(skipCheck==true || climber->passedJunction(direction,oldTime,time,nextTime)){
				scalar extraTime=Math::abs(time-nextTime);
				Path  *nextPath=path->getNeighbor(mPathSequence[np]);
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
		playerJump(mPlayer);
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
		playerMove(mPlayer,-xamount*3,-yamount*80);
	}
}

void RandIsle::inputDetected(const InputData &data){
	if(data.type==InputDevice::InputType_JOY){
		if((data.valid&(1<<InputData::Semantic_JOY_BUTTON_PRESSED))!=0){
			playerJump(mPlayer);
		}
		if((data.valid&(1<<InputData::Semantic_JOY_DIRECTION))!=0){
			mXJoy=data.values[InputData::Semantic_JOY_DIRECTION].x;
			mYJoy=data.values[InputData::Semantic_JOY_DIRECTION].y;
		}
	}
}

void RandIsle::playerJump(Node *player){
	PathClimber *climber=(PathClimber*)player->getChild("climber");

	if(climber->getPath()==NULL){
		Segment segment;
		segment.setStartDir(mPlayer->getTranslate(),Vector3(0,0,-5));
		PhysicsCollision result;
		mScene->traceSegment(result,segment,-1,player);
		if(result.time<Math::ONE){
			Math::mul(result.normal,40);
			player->getPhysics()->setVelocity(player->getPhysics()->getVelocity()+result.normal);
		}
	}
	else{
		climber->dismount();
	}
}

void RandIsle::playerMove(Node *player,scalar dr,scalar ds){
	PathClimber *climber=(PathClimber*)player->getChild("climber");

	Matrix3x3 drm;
	Math::setMatrix3x3FromZ(drm,dr);
	Quaternion q;
	Math::setQuaternionFromMatrix3x3(q,drm);
	Quaternion idealRotation=climber->getIdealRotation();
	Math::postMul(idealRotation,q);
	Math::normalizeCarefully(idealRotation,epsilon);
	climber->setIdealRotation(idealRotation);

	scalar speed=climber->getSpeed();
	speed+=ds;
	speed=Math::clamp(0,80,speed);
	climber->setSpeed(speed);
}

float RandIsle::findPathSequence(Collection<int> &sequence,PathClimber *climber,Path *path,int direction,scalar time){
	Vector3 right,forward,up;
	Math::setAxesFromQuaternion(climber->getIdealRotation(),right,forward,up);
	forward.z+=0.25;
	Math::normalizeCarefully(forward,epsilon);
	sequence.clear();
	scalar result=findPathSequence(sequence,climber,forward,NULL,path,direction,time,true);
	return result;
}

float RandIsle::findPathSequence(Collection<int> &sequence,PathClimber *climber,const Vector3 &forward,Path *previous,Path *path,int direction,scalar time,bool first){
	if(path==NULL){
		return 0;
	}

	scalar closestd=0;
	Collection<int> closestp;
	Vector3 point,tangent;
	path->getPoint(point,time);
	int i;
	for(i=0;i<path->getNumNeighbors();++i){
		Path *next=path->getNeighbor(i);
		scalar nextTime=path->getNeighborTime(i);
		if(next!=NULL && next==previous){
			continue;
		}

		path->getPoint(tangent,nextTime);
		Math::sub(tangent,point);

		scalar d=0;
		Collection<int> p;
		d=Math::dot(tangent,forward);
		d+=findPathSequence(p,climber,forward,path,next,0,next!=NULL?next->getNeighborTime(path):0,false);

		p.insert(p.begin(),i);

		if(closestp.size()==0 || d>closestd){
			closestd=d;
			closestp=p;
		}
	}

	sequence=closestp;
	return closestd;
}

int RandIsle::atJunction(PathClimber *climber,Path *current,Path *next){
	findPathSequence(mPathSequence,climber,climber->getPath(),climber->getPathDirection(),climber->getPathTime());

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
	for(int i=0;i<mPopulatePatches.size();i++){
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

		int size=getPatchSize();
		float ty=(patch->py*size+patch->y - size/2)/(float)size;
		float tx=pathValue(ty);

		float wx=tx*mPatchSize*mPatchScale.x;
		float wy=ty*mPatchSize*mPatchScale.y;

		Segment segment;
		segment.setStartDir(Vector3(wx,wy,1000),Vector3(0,0,-2000));
		PhysicsCollision result;
		mScene->traceSegment(result,segment,-1,NULL);
		if(result.time<Math::ONE && patch->bound->testIntersection(result.point)){
			result.point.z-=5;

			TreeSystem::ptr system;
			Node::ptr tree=new Node(mScene);
			{
				system=new TreeSystem(mScene,wx+wy);
				system->setName("system");
				tree->attach(system);

				tree->setScope(tree->getScope()&~Scope_BIT_SHADOW);
			}
			tree->setTranslate(result.point);
			mScene->getRoot()->attach(tree);

			system->grow();

			/// @todo: Swap between high & low res meshes, add LOD selection to node's renderables choices?
			MeshComponent::ptr mesh=new MeshComponent(mEngine);
			mesh->setMesh(system->getMesh());
			tree->attach(mesh);
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

void RandIsle::terrainPatchCreated(int px,int py,Bound *bound){
	mPopulatePatches.add(PopulatePatch(px,py,px-mTerrain->getTerrainX(),py-mTerrain->getTerrainY(),bound));
}

void RandIsle::terrainPatchDestroyed(int px,int py,Bound *bound){
	mBoundSensor->setBound(bound);
	SensorResults::ptr results=mBoundSensor->sense();
	for(int i=0;i<results->size();++i){
		Node *node=results->at(i);
		if(node->getChild("system")!=NULL && Math::testInside(bound->getAABox(),node->getWorldTranslate())){
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
