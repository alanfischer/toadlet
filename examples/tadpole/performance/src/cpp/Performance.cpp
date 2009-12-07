#include "Performance.h"

Performance::Performance():
	test(0)
{}


Performance::~Performance(){
}

void Performance::create(){
	Application::create();

	getEngine()->setScene((Scene*)(new Scene())->create(getEngine()));

	cameraNode=getEngine()->createNodeType(CameraNode::type());
	cameraNode->setLookAt(Vector3(0,-Math::fromInt(5),0),Math::ZERO_VECTOR3,Math::Z_UNIT_VECTOR3);
	getEngine()->getScene()->attach(cameraNode);

	setupTest(test++,10);
}

void Performance::update(int dt){
	parentNode->setRotate(0,0,Math::ONE,Math::fromMilli(getEngine()->getScene()->getRenderTime())*4);

	getEngine()->getScene()->update(dt);
}

void Performance::render(Renderer *renderer){
	getEngine()->contextUpdate(renderer);

	renderer->beginScene();

	getEngine()->getScene()->render(renderer,cameraNode);

	renderer->endScene();

	renderer->swap();

	setTitle(String("FPS:")+(int)(cameraNode->getFramesPerSecond()/Math::ONE));
}

void Performance::mousePressed(int x,int y,int button){
	if(setupTest(test++,0)==false){
		stop();
	}
}

void Performance::resized(int width,int height){
	if(cameraNode!=NULL && width!=0 && height!=0){
		if(width>=height){
			cameraNode->setProjectionFovY(Math::degToRad(Math::fromInt(45)),Math::div(Math::fromInt(width),Math::fromInt(height)),Math::fromMilli(100),Math::fromInt(100));
		}
		else{
			cameraNode->setProjectionFovX(Math::degToRad(Math::fromInt(45)),Math::div(Math::fromInt(height),Math::fromInt(width)),Math::fromMilli(100),Math::fromInt(100));
		}
	}
}

bool Performance::setupTest(int test,int intensity){
	if(parentNode!=NULL){
		parentNode->destroy();
	}
	parentNode=getEngine()->createNodeType(ParentNode::type());
	getEngine()->getScene()->attach(parentNode);

	bool result=true;
	for(int i=0;i<intensity+1;++i){
		switch(test){
			case 0:
				parentNode->attach(setupMinimumTest());
			break;
			case 1:
				parentNode->attach(setupFillrateTest());
			break;
			case 2:
				parentNode->attach(setupVertexrateTest());
			break;
			case 3:
				parentNode->attach(setupDynamicTest());
			break;
			default:
				result=false;
			break;
		}
	}

	return result;
}

Node::ptr Performance::setupMinimumTest(){
	MeshNode::ptr cubeNode=getEngine()->createNodeType(MeshNode::type());
	cubeNode->setMesh(getEngine()->getMeshManager()->createBox(AABox(-Math::HALF,-Math::HALF,-Math::HALF,Math::HALF,Math::HALF,Math::HALF)));
	return cubeNode;
}

Node::ptr Performance::setupFillrateTest(){
	int i;
	Mesh::ptr cubeMesh=getEngine()->getMeshManager()->createBox(AABox(-Math::HALF,-Math::HALF,-Math::HALF,Math::HALF,Math::HALF,Math::HALF));
	ParentNode::ptr node=getEngine()->createNodeType(ParentNode::type());

	for(i=0;i<10;++i){
		MeshNode::ptr cubeNode=getEngine()->createNodeType(MeshNode::type());
		cubeNode->setMesh(cubeMesh);
		cubeNode->setScale(Math::fromInt(5),Math::fromInt(5),Math::fromInt(5));
		node->attach(cubeNode);
	}

	for(i=0;i<node->getNumChildren();++i){
		node->getChild(i)->setRotate(0,0,Math::ONE,Math::div(Math::fromInt(i)*Math::TWO_PI,Math::fromInt(node->getNumChildren())));
	}

	return node;
}

Node::ptr Performance::setupVertexrateTest(){
	MeshNode::ptr sphereNode=getEngine()->createNodeType(MeshNode::type());
	sphereNode->setMesh(getEngine()->getMeshManager()->createSphere(Sphere(Math::HALF)));
	return sphereNode;
}

Node::ptr Performance::setupDynamicTest(){
	// TODO
	Node::ptr node=getEngine()->createNodeType(Node::type());
	return node;
}

#if defined(TOADLET_PLATFORM_WINCE)
int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPTSTR lpCmdLine,int nCmdShow){
#else
int main(int argc,char **argv){
#endif
	Performance app;
	app.setFullscreen(false);
	app.create();
	app.start(true);
	app.destroy();
	return 0;
}
