#include "Viewer.h"

/*
class Axis:public Node,Renderable{
public:
	TOADLET_NODE(Axis,Node);

	void create(){
		Super::create();

		mVertexBuffer.resize(VertexBuffer::VT_POSITION,4);
		mIndexBuffer.resize(2);
		mVertexBuffer.position(0)=Vector3(0.0,0.0,0.0);
		mVertexBuffer.position(1)=Vector3(1.0,0.0,0.0);
		mVertexBuffer.position(2)=Vector3(0.0,1.0,0.0);
		mVertexBuffer.position(3)=Vector3(0.0,0.0,1.0);
	}

	void render(RenderQueue *q,Frustum *f) const{
		q->addRenderable(this);
	}

	void render(Renderer *r) const{
		r->setLighting(false);
		r->disableTextureStage(0);
		LightEffect le;
		le.trackColor=true;
		r->setLightEffect(le);

		mIndexBuffer.index(0)=0;
		mIndexBuffer.index(1)=1;
		r->setColor(Color::COLOR_RED);
		r->renderPrimitive(PRIMITIVE_LINES,&mVertexBuffer,&mIndexBuffer);

		mIndexBuffer.index(0)=0;
		mIndexBuffer.index(1)=2;
		r->setColor(Color::COLOR_GREEN);
		r->renderPrimitive(PRIMITIVE_LINES,&mVertexBuffer,&mIndexBuffer);

		mIndexBuffer.index(0)=0;
		mIndexBuffer.index(1)=3;
		r->setColor(Color::COLOR_BLUE);
		r->renderPrimitive(PRIMITIVE_LINES,&mVertexBuffer,&mIndexBuffer);
	}

	const Transform &getTransform() const{return mWorldTransform;}
	const Material *getMaterial() const{return NULL;}
	bool isTransparent() const{return true;}
	bool castsShadows() const{return false;}

protected:
	mutable VertexBuffer mVertexBuffer;
	mutable IndexBuffer mIndexBuffer;
};
*/

Viewer::Viewer(Application *app):
	mMouseX(0),mMouseY(0),
	mDrag(false),
	mDistance(0),
	mZoom(false)
{
	mApp=app;
	mEngine=mApp->getEngine();

 	mScene=Scene::ptr(new Scene(mEngine));

	mCamera=mEngine->createNodeType(CameraNode::type(),mScene);
	mCamera->setClearColor(Colors::ORANGE);
	mScene->getRoot()->attach(mCamera);

	mParent=mEngine->createNodeType(ParentNode::type(),mScene);
	mScene->getRoot()->attach(mParent);

	mScene->setAmbientColor(Vector4(Math::QUARTER,Math::QUARTER,Math::QUARTER,Math::ONE));

	mLight=mEngine->createNodeType(LightNode::type(),mScene);
	LightState state;
	state.type=LightState::Type_DIRECTION;
	state.direction.set(Math::Y_UNIT_VECTOR3);
	state.diffuseColor.set(Math::ONE_VECTOR4);
	state.specularColor.set(Math::ONE_VECTOR4);
	mLight->setLightState(state);
	mScene->getRoot()->attach(mLight);
}

Viewer::~Viewer(){
	destroy();
}

void Viewer::setNode(MeshNode::ptr node){
	mParent->attach(node);

	mDistance=Math::length(node->getBound().getSphere().origin)+node->getBound().getSphere().radius*2;

	updateCamera();
}

void Viewer::destroy(){
	if(mScene!=NULL){
		mScene->destroy();
		mScene=NULL;
	}
}

void Viewer::update(int dt){
	mScene->update(dt);
}

void Viewer::render(){
	RenderDevice *device=mEngine->getRenderDevice();

	device->beginScene();
		mCamera->render(device);
	device->endScene();
	device->swap();
}

void Viewer::mouseMoved(int x,int y){
	int diffx=mMouseX-x;
	int diffy=mMouseY-y;
	
	if(mDrag){
		scalar dx=Math::fromInt(diffx)/100;
		scalar dy=Math::fromInt(diffy)/100;

		EulerAngle ex(0,-dx,-dy);
		Quaternion r;
		Math::setQuaternionFromEulerAngleXYZ(r,ex);
		Math::postMul(r,mParent->getRotate());
		mParent->setRotate(r);
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

void Viewer::mousePressed(int x,int y,int button){
	if(button==0){
		mDrag=true;
	}
	else if(button==2){
		mZoom=true;
	}
}
	
void Viewer::mouseReleased(int x,int y,int button){
	if(button==0){
		mDrag=false;
	}
	else if(button==2){
		mZoom=false;
	}
}

void Viewer::resized(int width,int height){
	scalar radius=mParent!=NULL?Math::length(mParent->getWorldBound().getSphere().origin)+mParent->getWorldBound().getSphere().radius*2:0;
	scalar epsilon=0.001;
	scalar nearDistance=mDistance-2*radius;
	scalar farDistance=mDistance+2*radius;
	if(nearDistance<epsilon){nearDistance=epsilon;}
	if(farDistance<nearDistance+epsilon){farDistance=nearDistance+epsilon;}
	if(mCamera!=NULL && width!=0 && height!=0){
		if(width>=height){
			mCamera->setProjectionFovY(Math::degToRad(Math::fromInt(45)),Math::div(Math::fromInt(width),Math::fromInt(height)),nearDistance,farDistance);
		}
		else{
			mCamera->setProjectionFovX(Math::degToRad(Math::fromInt(45)),Math::div(Math::fromInt(height),Math::fromInt(width)),nearDistance,farDistance);
		}
		mCamera->setViewport(Viewport(0,0,width,height));
	}
}

void Viewer::focusLost(){
	mDrag=false;
	mZoom=false;
}

void Viewer::updateCamera(){
	Vector3 eye(Math::Y_UNIT_VECTOR3);
	Math::mul(eye,-mDistance);
	mCamera->setLookAt(eye,Math::ZERO_VECTOR3,Math::Z_UNIT_VECTOR3);
	resized(mApp->getWidth(),mApp->getHeight());
}
