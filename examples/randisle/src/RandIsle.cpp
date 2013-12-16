#include "RandIsle.h"
#include "PathClimber.h"
#include "TreeComponent.h"
#include "HUD.h"
#include "Resources.h"
#include <toadlet/tadpole/plugins/HopManager.h>
#include <toadlet/tadpole/plugins/HopComponent.h>
#include <toadlet/tadpole/plugins/DecalShadowRenderManager.h>

#define TREE_CAMERA_DISTANCE 80

static const scalar epsilon=0.001f;

class SnowComponent:public BaseComponent{
public:
	SnowComponent(ParticleComponent *particles,int skip):
		mParticles(particles),
		mSkip(skip),
		mNext(0)
	{
		mSnowData.resize(mParticles->getNumParticles());
		for(int i=0;i<mParticles->getNumParticles();++i){
			ParticleComponent::Particle *p=mParticles->getParticle(i);
			mSnowData[i].oldPosition=mSnowData[i].newPosition=Vector3(p->x,p->y,p->z);
		}
	}

	void frameUpdate(int dt,int scope){
		Scene *scene=mParent->getScene();

		for(int i=0;i<mParticles->getNumParticles();i++){
			ParticleComponent::Particle *p=mParticles->getParticle(i);

			Vector3 position(p->x,p->y,p->z);

			float t=float((i-mNext+mSkip)%mSkip)/float(mSkip);
			t = 1.0 - (t - scene->getLogicFraction()/float(mSkip));
			if(mSnowData[i].traceTime>0){
				t = Math::clamp(0, Math::ONE, t / mSnowData[i].traceTime);
			}
			Math::lerp(position,mSnowData[i].oldPosition,mSnowData[i].newPosition,t);

			p->x=position.x;p->y=position.y;p->z=position.z;
		}
	}

	void logicUpdate(int dt,int scope){
		Scene *scene=mParent->getScene();

		for(int i=0;i<mParticles->getNumParticles();i++){
			ParticleComponent::Particle *p=mParticles->getParticle(i);

			Vector3 position(p->x,p->y,p->z);
			Vector3 velocity(p->vx,p->vy,p->vz);

			float t=float((i-mNext+mSkip)%mSkip)/float(mSkip);
			t = 1.0 - t;
			if(mSnowData[i].traceTime>0){
				t = Math::clamp(0, Math::ONE, t / mSnowData[i].traceTime);
			}
			Math::lerp(position,mSnowData[i].oldPosition,mSnowData[i].newPosition,t);

			if((i%mSkip)==mNext){
				Segment segment;
				segment.origin=position;
				segment.direction=velocity*Math::fromMilli(dt) * mSkip;

				PhysicsCollision result;
				scene->getPhysicsManager()->traceSegment(result,segment,-1,NULL);
				mSnowData[i].traceTime=result.time;

				mSnowData[i].oldPosition=mSnowData[i].newPosition;

				if(result.time<1){
					mSnowData[i].newPosition=result.point;
				}
				else{
					mSnowData[i].newPosition=position+segment.direction;
				}
			}

			p->x=position.x;p->y=position.y;p->z=position.z;
			p->vx=velocity.x;p->vy=velocity.y;p->vz=velocity.z;
		}

		mNext++;
		if(mNext>=mSkip){
			mNext=0;
		}
	}

protected:
	class SnowData{
	public:
		SnowData():traceTime(0){}

		Vector3 oldPosition;
		Vector3 newPosition;
		scalar traceTime;
	};

	ParticleComponent::ptr mParticles;
	Collection<SnowData> mSnowData;
	int mSkip,mNext;
};

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

	mCamera=new Camera(mEngine);
//	shared_static_cast<StereoscopicCamera>(mCamera)->setCrossEyed(true);
	mCamera->setAutoProjectionFov(Math::degToRad(Math::fromInt(60)),false,mCamera->getNearDist(),1024);
	mCamera->setScope(~Scope_BIT_HUD | (Scope_BIT_MAIN_CAMERA & ~Scope_BIT_WATER_TRANSPARENT));
	mCamera->setClearColor(Resources::instance->fadeColor);
	mCamera->getDefaultState()->setFogState(FogState(FogState::FogType_LINEAR,Math::ONE,mCamera->getFarDist()/2,mCamera->getFarDist(),mCamera->getClearColor()));
	mFollowNode->attach(new CameraComponent(mCamera));

	if(Resources::instance->reflectTarget!=NULL){
		mReflectCamera=new Camera(mEngine);
		mReflectCamera->setRenderTarget(Resources::instance->reflectTarget);
		mReflectCamera->setAutoProjectionFov(Math::degToRad(Math::fromInt(60)),false,mCamera->getNearDist(),mCamera->getFarDist());
		mReflectCamera->setScope(~Scope_BIT_HUD & ~Scope_BIT_MAIN_CAMERA & ~Scope_BIT_WATER & ~Scope_BIT_WATER_TRANSPARENT);
		mReflectCamera->setClearColor(Resources::instance->fadeColor);
		mReflectCamera->getDefaultState()->setFogState(FogState(FogState::FogType_LINEAR,Math::ONE,mCamera->getFarDist()/2,mCamera->getFarDist(),mCamera->getClearColor()));
	}

	if(Resources::instance->refractTarget!=NULL){
		mRefractCamera=new Camera(mEngine);
		mRefractCamera->setRenderTarget(Resources::instance->refractTarget);
		mRefractCamera->setAutoProjectionFov(Math::degToRad(Math::fromInt(60)),false,mCamera->getNearDist(),mCamera->getFarDist());
		mRefractCamera->setScope((~Scope_BIT_HUD & ~Scope_BIT_MAIN_CAMERA & ~Scope_BIT_WATER) | Scope_BIT_WATER_TRANSPARENT);
		mRefractCamera->setClearColor(Resources::instance->fadeColor);
		mRefractCamera->getDefaultState()->setFogState(FogState(FogState::FogType_LINEAR,Math::ONE,mCamera->getFarDist()/2,mCamera->getFarDist(),mCamera->getClearColor()));
	}

	mHUD=new HUD(mScene,mPlayer,mCamera);
	mHUD->setScope(Scope_BIT_HUD);
	mScene->getRoot()->attach(mHUD);

	mHUDCamera=new Camera(mEngine);
	mHUDCamera->setProjectionOrtho(-1,1,-1,1,-1,1);
	mHUDCamera->setClearFlags(0);

	mSky=new Sky(mScene,Resources::instance->cloudSize,Resources::instance->skyColor,Resources::instance->fadeColor);
	mSky->setScope(mSky->getScope()&~Scope_BIT_SHADOW);
	mScene->getBackground()->attach(mSky);

	VertexBuffer::ptr predictedVertexBuffer=mEngine->getBufferManager()->createVertexBuffer(Buffer::Usage_BIT_STREAM,Buffer::Access_BIT_WRITE,mEngine->getVertexFormats().POSITION_COLOR,512);
	mPredictedVertexData=new VertexData(predictedVertexBuffer);
	mPredictedIndexData=new IndexData(IndexData::Primitive_TRISTRIP,NULL,0,0);
	RenderState::ptr renderState=mEngine->getMaterialManager()->createRenderState();
	renderState->setDepthState(DepthState(DepthState::DepthTest_ALWAYS,false));
	renderState->setRasterizerState(RasterizerState(RasterizerState::CullType_NONE));
	renderState->setMaterialState(MaterialState(true,true));
	renderState->setBlendState(BlendState::Combination_ALPHA);
	Material::ptr predictedMaterial=mEngine->createDiffuseMaterial(NULL,renderState);
	
	mPredictedNode=new Node(mScene);
	{
		mPredictedComponent=new MeshComponent(mEngine);
		{
			Mesh::ptr mesh=new Mesh();
			Mesh::SubMesh::ptr subMesh=new Mesh::SubMesh();
			subMesh->vertexData=mPredictedVertexData;
			subMesh->indexData=mPredictedIndexData;
			subMesh->material=predictedMaterial;
			mesh->setBound(new Bound(Bound::Type_INFINITE));
			mesh->addSubMesh(subMesh);
			mPredictedComponent->setMesh(mesh);
		}
		mPredictedNode->attach(mPredictedComponent);
	}
	mScene->getRoot()->attach(mPredictedNode);

	mPlayer=new Node(mScene);
	{
		PathClimber *climber=new PathClimber();
		{
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
		if(mesh->getSkeleton() && mesh->getSkeleton()->getNumAnimations()>1){
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

if(false){
Node::ptr node=new Node(mScene);
CameraComponent::ptr cc=new CameraComponent(mCamera);
node->attach(cc);
mPlayer->attach(node);
cc->setLookDir(Vector3(0,0,0),Vector3(0,1,0),Vector3(0,0,1));
}

	Log::alert("Adding props");
	mProps=new Node(mScene);
	for(int i=0;i<Resources::instance->numProps;++i){
		Node::ptr prop=new Node(mScene);
		MeshComponent *mesh=new MeshComponent(mEngine);
		mesh->setMesh(Resources::instance->grass);
		mesh->setSharedRenderState(NULL);
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
/*
	Node::ptr snow=new Node(mScene);
	{
		ParticleComponent::ptr particles=new ParticleComponent(mScene);
		particles->setNumParticles(2000,ParticleComponent::ParticleType_SPRITE,1);
		particles->setMaterial(Resources::instance->acorn);
		Random r;
		for(int i=0;i<particles->getNumParticles();++i){
			ParticleComponent::Particle *p=particles->getParticle(i);
			p->x=r.nextFloat(-100,100);p->y=r.nextFloat(-100,100);p->z=200;
			p->vx=r.nextFloat(-1,1);p->vy=r.nextFloat(-1,1);p->vz=r.nextFloat(0,-15);
		}
		snow->attach(particles);

		snow->attach(new SnowComponent(particles,50));
	}
	mScene->getRoot()->attach(snow);
*/
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

	Log::debug("RandIsle::destroy finished");
}

void RandIsle::render(){
	TOADLET_PROFILE_AUTOSCOPE();

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
	TOADLET_PROFILE_AUTOSCOPE();

	PathClimber *climber=mPlayer->getChildType<PathClimber>();
	PhysicsComponent *physics=mPlayer->getPhysics();

	updateClimber(climber,dt);

	updateDanger(dt);

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
}

void RandIsle::frameUpdate(int dt){
	TOADLET_PROFILE_AUTOSCOPE();

	{
		Vector3 windVector(0,0.1f,0);
		float waveLength=0.1f;
		float time=Math::fromMilli(mScene->getTime());
		TextureState state;
		state.calculation=TextureState::CalculationType_NORMAL;
		Math::setMatrix4x4FromTranslateRotateScale(state.matrix,time*windVector,Quaternion(),Vector3(1.0/waveLength,1.0/waveLength,1.0/waveLength));

		Shader::ShaderType type=(Shader::ShaderType)0;
		int index=0;
		Material::ptr water=Resources::instance->waterMaterial;
		if(water && water->getPass()!=NULL){
			RenderPass::ptr pass=water->getPass();
			pass->findTexture(type,index,"waveTex");
			pass->setTextureState(type,index,state);
		}
	}

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
	tforeach(AnyPointerIterator<Node>,prop,mProps->getNodes()){
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
			a=Math::ONE-(d-minDist)/(maxDist-minDist);
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
			PathVertex *closestVertex=NULL;
			Vector3 closestPoint;

			mMountBound->set(mPlayer->getTranslate(),snapDistance);
			mBoundSensor->setBound(mMountBound);
			SensorResults::ptr results=mBoundSensor->sense();

			tforeach(SensorResults::iterator,node,results){
				TreeComponent *tree=node->getChildType<TreeComponent>();
				if(tree!=NULL){
					Vector3 point;
					PathVertex *vertex=tree->getClosestBranch(point,mPlayer->getTranslate());
					float distance=Math::length(point,mPlayer->getTranslate());

					if(vertex!=NULL && distance<closestDistance){
						closestDistance=distance;
						closestVertex=vertex;
						closestNode=node;
						closestPoint.set(point);
					}
				}
			}

			if(closestVertex!=NULL && closestDistance<snapDistance){
				climber->mount(closestNode,closestVertex,closestPoint);
				findPathSequence(mPathSequence,climber,climber->getPath(),climber->getPathDirection(),climber->getPathTime());
			}

		}
	}

	// Update graph prediction & camera
	if(climber->getPath()!=NULL){
		PathVertex *vertex=climber->getPath();
		int direction=climber->getPathDirection();
		scalar time=climber->getPathTime();

		if(mPathSequence.size()>0){
			scalar edgeTime=mPathSequence[0]->getTime(mPathSequence[0]->getVertex(true)==vertex);
			if(edgeTime>time && direction<0){
				climber->setPathDirection(1);
			}
			else if(edgeTime<time && direction>0){
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
		PathVertex *vertex=climber->getPath();
		scalar oldTime=0,time=climber->getPathTime();
		int direction=climber->getPathDirection();

		Vector3 point;
		Vector3 forward,right,tangent,normal,scale;
		scalar predictTime=400;
		Vector4 color(0x00FF00FF);

		VertexBufferAccessor vba(mPredictedVertexData->getVertexBuffer(0),Buffer::Access_BIT_WRITE);

		scalar nextTime=mPathSequence[0]->getTime(mPathSequence[0]->getVertex(true)==vertex);

		float dt=1.0;
		int np=0,i=0;
		for(i=0;i<vba.getSize()-2 && vertex!=NULL && predictTime>0;i+=2){
			vertex->getPoint(point,time);
			Math::add(point,climber->getMounted()->getWorldTranslate());
			vertex->getOrientation(tangent,normal,scale,time);

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
				PathVertex *nextVertex=mPathSequence[np]->getVertex(mPathSequence[np]->getVertex(false)==vertex);
				np++;
				if(np<mPathSequence.size()){
					nextTime=mPathSequence[np]->getTime(mPathSequence[np]->getVertex(false)==vertex);

					time=mPathSequence[np]->getTime(mPathSequence[np]->getVertex(true)==vertex);
					direction=(nextTime>time)?1:-1;
					time+=direction*extraTime;
					// We have to check the direction again, in case we passed our destination with the extraTime
					direction=(nextTime>time)?1:-1;
				}
				vertex=nextVertex;
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

		Profile::getInstance()->outputTimings();
		Profile::getInstance()->clearTimings();
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
	PathClimber *climber=player->getChildType<PathClimber>();

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
	PathClimber *climber=player->getChildType<PathClimber>();

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

float RandIsle::findPathSequence(PointerCollection<PathEdge> &sequence,PathClimber *climber,PathVertex *vertex,int direction,scalar time){
	Vector3 right,forward,up;
	Math::setAxesFromQuaternion(climber->getIdealRotation(),right,forward,up);
	forward.z+=0.25;
	Math::normalizeCarefully(forward,epsilon);
	sequence.clear();
	scalar result=findPathSequence(sequence,climber,forward,NULL,vertex,direction,time,true);
	return result;
}

float RandIsle::findPathSequence(PointerCollection<PathEdge> &sequence,PathClimber *climber,const Vector3 &forward,PathVertex *previous,PathVertex *vertex,int direction,scalar time,bool first){
	if(vertex==NULL){
		return 0;
	}

	sequence.clear();
	scalar closestd=0;
	Vector3 point,tangent;
	vertex->getPoint(point,time);

	tforeach(PathVertex::iterator,next,vertex->getEdges()){
		if(next!=NULL && next->getVertex(next->getVertex(false)==vertex)==previous){
			continue;
		}
		scalar nextTime=next->getTime(next->getVertex(true)==vertex);

		vertex->getPoint(tangent,nextTime);
		Math::sub(tangent,point);

		scalar d=0;
		PointerCollection<PathEdge> p;
		d=Math::dot(tangent,forward);
		d+=findPathSequence(p,climber,forward,vertex,next->getVertex(next->getVertex(false)==vertex),0,next!=NULL?next->getTime(next->getVertex(true)==vertex):0,false);

		p.insert(p.begin(),(PathEdge*)next);

		if(sequence.size()==0 || d>closestd){
			closestd=d;
			sequence=p;
		}
	}

	return closestd;
}

int RandIsle::atJunction(PathClimber *climber,PathVertex *current,PathVertex *next){
	findPathSequence(mPathSequence,climber,climber->getPath(),climber->getPathDirection(),climber->getPathTime());

	if(mPathSequence.size()>0 && mPathSequence[0]->getVertex(mPathSequence[0]->getVertex(false)==current)==next){
		mPathSequence.removeAt(0);
		if(mPathSequence.size()>0){
			if(mPathSequence[0]->getTime(mPathSequence[0]->getVertex(true)==next) > mPathSequence[0]->getTime(mPathSequence[0]->getVertex(true)==current)){
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

			TreeComponent::ptr treeComponent;
			Node::ptr tree=new Node(mScene);
			{
				treeComponent=new TreeComponent(mScene,wx+wy,Resources::instance->treeBranch,Resources::instance->treeLeaf);
				tree->attach(treeComponent);

				tree->setScope(tree->getScope()&~Scope_BIT_SHADOW);
			}
			tree->setTranslate(result.point);
			mScene->getRoot()->attach(tree);

			treeComponent->grow();
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
	tforeach(SensorResults::iterator,node,results){
		if(node->getChildType<TreeComponent>()!=NULL && Math::testInside(bound->getAABox(),node->getWorldTranslate())){
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
			data[y*size+x]=terrainValue(tx,ty);
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
	const char *paths[]={"../../randisle/res","../randisle/res","../../res","../res","res",".",NULL};
	int i;
	for(i=0;paths[i]!=NULL;++i){
		if(FileStream(paths[i]+lookFor,FileStream::Open_BIT_READ|FileStream::Open_BIT_BINARY).closed()==false){
			path=paths[i];
			break;
		}
	}

	return new RandIsle(app,path);
}
