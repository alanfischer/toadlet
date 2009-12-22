#include "Viewer.h"

using namespace toadlet;
using namespace toadlet::egg;
using namespace toadlet::peeper;
using namespace toadlet::tadpole;
using namespace toadlet::tadpole::node;
using namespace toadlet::pad;

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

Viewer::Viewer():Application(),
	mMouseX(0),mMouseY(0),
	mDrag(false),
	mDistance(0),
	mZoom(false)
{}

void Viewer::start(MeshNode::ptr meshNode){
	mEngine->setScene(mEngine->createNodeType(SceneNode::type()));
	mParent=mEngine->createNodeType(ParentNode::type());
	mEngine->getScene()->getRootNode()->attach(mParent);
	mParent->attach(meshNode);

	mCamera=mEngine->createNodeType(CameraNode::type());
	mEngine->getScene()->getRootNode()->attach(mCamera);

	mDistance=meshNode->getBoundingRadius()*2;

	mEngine->getScene()->setAmbientColor(Color(Math::QUARTER,Math::QUARTER,Math::QUARTER,Math::ONE));

	mLight=mEngine->createNodeType(LightNode::type());
	mLight->setLightType(Light::Type_DIRECTION);
	mLight->setDirection(Math::Y_UNIT_VECTOR3);
	mLight->setDiffuseColor(Colors::WHITE);
	mLight->setSpecularColor(Colors::WHITE);
	mEngine->getScene()->getRootNode()->attach(mLight);

	updateCamera();

	Application::start(true);
}

void Viewer::update(int dt){
	mEngine->getScene()->update(dt);
}

void Viewer::render(Renderer *renderer){
	renderer->beginScene();

	mEngine->getScene()->render(renderer,mCamera,NULL);

	renderer->endScene();

	renderer->swap();
}

void Viewer::mouseMoved(int x,int y){
	int diffx=mMouseX-x;
	int diffy=mMouseY-y;
	
	if(mDrag){
		scalar dx=Math::fromInt(diffx)/100;
		scalar dy=Math::fromInt(diffy)/100;

		EulerAngle ex(-dy,0.0,-dx);
		Matrix3x3 mx;
		Matrix3x3 r;
		Math::setMatrix3x3FromEulerAngleXYZ(mx,ex);
		Math::mul(r,mx,mParent->getRotate());
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
	if(width!=0 && height!=0){
		if(width>=height){
			mCamera->setProjectionFovY(Math::degToRad(Math::fromInt(45)),Math::div(Math::fromInt(width),Math::fromInt(height)),Math::ONE,mDistance*2);
		}
		else{
			mCamera->setProjectionFovX(Math::degToRad(Math::fromInt(45)),Math::div(Math::fromInt(height),Math::fromInt(width)),Math::ONE,mDistance*2);
		}
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
}
