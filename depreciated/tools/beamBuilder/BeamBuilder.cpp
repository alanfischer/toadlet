#include "BeamBuilder.h"

BeamBuilder::BeamBuilder():Application(),
 	mMouseX(0),mMouseY(0),
	mDrag(false),
	mDistance(Math::fromInt(10)),
	mZoom(false)
{}

void BeamBuilder::create(int argc,char **argv){
	Application::create();
	
	setFullscreen(false);
	setSize(640,480);

	mEngine->setScene(new Scene(mEngine));

	mCamera=CameraEntity::ptr(new CameraEntity(mEngine));
	mEngine->getScene()->attach(mCamera);

	mParticles=new ParticleEntity(mEngine);
	mEngine->getScene()->attach(mParticles);

	updateCamera();

	Collection<Vector3> points;
	if(argc>=2){
		for(int i=1;i<argc;++i){
			mEngine->loadParticleFile(points,argv[i]);
		}
	}

	if(points.size()>0){
		mParticles->setColor(Colors::BLUE);
		mParticles->setScale(Math::fromMilli(200));
		mParticles->start(2,points.size(),true,&points[0],NULL);
		mParticles->getRenderMaterial()->setTextureStage(0,TextureStage::ptr(new TextureStage(mEngine->loadTexture("fancyWhite.png"))));
		mParticles->getRenderMaterial()->setBlend(Blend::Combination_ALPHA_ADDITIVE);
		mParticles->getRenderMaterial()->setDepthWrite(false);
	}
	else{
		//loadBeam();
		loadAxe();
	}
}


void BeamBuilder::update(int dt){
	mEngine->getScene()->update(dt);
}

void BeamBuilder::render(Renderer *renderer){
	mEngine->contextUpdate(renderer);

	renderer->beginScene();

	mEngine->getScene()->render(renderer,mCamera);

	renderer->endScene();

	renderer->swap();
}

void BeamBuilder::mouseMoved(int x,int y){
	int diffx=mMouseX-x;
	int diffy=mMouseY-y;
	
	if(mDrag){
		scalar dx=Math::fromInt(diffx)/100;
		scalar dy=Math::fromInt(diffy)/100;

		EulerAngle ex(-dy,0.0,-dx);
		Matrix3x3 offset;
		Math::setMatrix3x3FromEulerAngleXYZ(offset,ex);
		mRotate=mRotate*offset;

		updateCamera();
	}
	if(mZoom){
		scalar dy=Math::fromInt(diffy)/10;

		mDistance-=dy;
		if(mDistance<0){
			mDistance=0;
		}

		updateCamera();
	}
	
	mMouseX=x;
	mMouseY=y;
}

void BeamBuilder::mousePressed(int x,int y,int b){
	if(b==0){
		mDrag=true;
	}
	else if(b==2){
		mZoom=true;
	}
}
	
void BeamBuilder::mouseReleased(int x,int y,int b){
	if(b==0){
		mDrag=false;
	}
	else if(b==2){
		mZoom=false;
	}
}

void BeamBuilder::focusLost(){
	mDrag=false;
	mZoom=false;
}

void BeamBuilder::updateCamera(){
	Vector3 eye(0,-mDistance,0);
	Math::mul(eye,mRotate);
	mCamera->setLookAt(eye,Math::ZERO_VECTOR3,Math::Z_UNIT_VECTOR3);
}

void BeamBuilder::loadBeam(){
	scalar bottomZ	= Math::ONE*2;
	scalar edgeX	= Math::ONE;
	scalar edgeY	= Math::HALF;
	scalar farEdgeX	= Math::ONE+Math::HALF;
	scalar edgeZ	= Math::HALF;
	scalar topZ		= Math::ONE+Math::HALF;
	scalar topX		= Math::HALF;
	scalar centerZ	= 0;

	const int numPoints=44;
	Vector3 points[numPoints];

	points[0]=Vector3(0,0,-bottomZ);
	points[1]=Vector3(-edgeX,-edgeY,edgeZ);

	points[2]=Vector3(0,0,-bottomZ);
	points[3]=Vector3(-farEdgeX,0,edgeZ);

	points[4]=Vector3(0,0,-bottomZ);
	points[5]=Vector3(-edgeX,edgeY,edgeZ);


	points[6]=Vector3(0,0,-bottomZ);
	points[7]=Vector3(edgeX,-edgeY,edgeZ);

	points[8]=Vector3(0,0,-bottomZ);
	points[9]=Vector3(farEdgeX,0,edgeZ);

	points[10]=Vector3(0,0,-bottomZ);
	points[11]=Vector3(edgeX,edgeY,edgeZ);


	points[12]=Vector3(-edgeX,-edgeY,edgeZ);
	points[13]=Vector3(-farEdgeX,0,edgeZ);

	points[14]=Vector3(-farEdgeX,0,edgeZ);
	points[15]=Vector3(-edgeX,edgeY,edgeZ);

	points[16]=Vector3(-edgeX,edgeY,edgeZ);
	points[17]=Vector3(0,0,centerZ);

	points[18]=Vector3(0,0,centerZ);
	points[19]=Vector3(edgeX,edgeY,edgeZ);

	points[20]=Vector3(edgeX,edgeY,edgeZ);
	points[21]=Vector3(farEdgeX,0,edgeZ);

	points[22]=Vector3(farEdgeX,0,edgeZ);
	points[23]=Vector3(edgeX,-edgeY,edgeZ);

	points[24]=Vector3(edgeX,-edgeY,edgeZ);
	points[25]=Vector3(0,0,centerZ);

	points[26]=Vector3(0,0,centerZ);
	points[27]=Vector3(-edgeX,-edgeY,edgeZ);


	points[28]=Vector3(-edgeX,-edgeY,edgeZ);
	points[29]=Vector3(-topX,0,topZ);

	points[30]=Vector3(-farEdgeX,0,edgeZ);
	points[31]=Vector3(-topX,0,topZ);

	points[32]=Vector3(-edgeX,edgeY,edgeZ);
	points[33]=Vector3(-topX,0,topZ);


	points[34]=Vector3(edgeX,-edgeY,edgeZ);
	points[35]=Vector3(topX,0,topZ);

	points[36]=Vector3(farEdgeX,0,edgeZ);
	points[37]=Vector3(topX,0,topZ);

	points[38]=Vector3(edgeX,edgeY,edgeZ);
	points[39]=Vector3(topX,0,topZ);


	points[40]=Vector3(-topX,0,topZ);
	points[41]=Vector3(0,0,centerZ);

	points[42]=Vector3(topX,0,topZ);
	points[43]=Vector3(0,0,centerZ);

	mParticles->setColor(Colors::BLUE);
	mParticles->setScale(Math::fromMilli(200));
	mParticles->start(2,numPoints,true,points,NULL);
	mParticles->getRenderMaterial()->setTextureStage(0,TextureStage::ptr(new TextureStage(mEngine->loadTexture("fancyWhite.png"))));
	mParticles->getRenderMaterial()->setBlend(Blend::Combination_ALPHA_ADDITIVE);
	mParticles->getRenderMaterial()->setDepthWrite(false);
}


void BeamBuilder::loadAxe(){
	scalar q	= Math::HALF/2;
	scalar h	= Math::HALF;
	scalar o	= Math::ONE;

	const int numPoints=26;
	Vector3 points[numPoints];

	points[0]=Vector3(0,q,h);
	points[1]=Vector3(0,-q,h);
	points[2]=Vector3(0,-q,h);
	points[3]=Vector3(0,-q,-h);
	points[4]=Vector3(0,-q,-h);
	points[5]=Vector3(0,q,-h);
	points[6]=Vector3(0,q,-h);
	points[7]=Vector3(0,q,h);
	
	points[8]=Vector3(0,q,h);
	points[9]=Vector3(o,q,h);
	points[10]=Vector3(0,-q,h);
	points[11]=Vector3(o,-q,h);
	points[12]=Vector3(0,-q,-h);
	points[13]=Vector3(o,-q,-h);
	points[14]=Vector3(0,q,-h);
	points[15]=Vector3(o,q,-h);

	points[16]=Vector3(o,q,h);
	points[17]=Vector3(2*o,0,h+q);
	points[18]=Vector3(o,-q,h);
	points[19]=Vector3(2*o,0,h+q);
	points[20]=Vector3(o,-q,-h);
	points[21]=Vector3(2*o,0,-h-q);
	points[22]=Vector3(o,q,-h);
	points[23]=Vector3(2*o,0,-h-q);

	points[24]=Vector3(2*o,0,h+q);
	points[25]=Vector3(2*o,0,-h-q);

	mParticles->setColor(Colors::BLUE);
	mParticles->setScale(Math::fromMilli(200));
	mParticles->start(2,numPoints,true,points,NULL);
	mParticles->getRenderMaterial()->setTextureStage(0,TextureStage::ptr(new TextureStage(mEngine->loadTexture("fancyWhite.png"))));
	mParticles->getRenderMaterial()->setBlend(Blend::Combination_ALPHA_ADDITIVE);
	mParticles->getRenderMaterial()->setDepthWrite(false);
}
