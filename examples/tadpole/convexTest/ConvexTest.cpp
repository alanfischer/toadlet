#include "ConvexTest.h"
#include <toadlet/egg/Error.h>
#include <toadlet/egg/Logger.h>
#include <toadlet/egg/System.h>
#include <toadlet/tadpole/entity/MeshEntity.h>


using namespace toadlet;
using namespace toadlet::egg;
using namespace toadlet::hop;
using namespace toadlet::peeper;
using namespace toadlet::tadpole;
using namespace toadlet::tadpole::entity;
using namespace toadlet::pad;

#if defined(TOADLET_PLATFORM_WINCE)
int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPTSTR lpCmdLine,int nCmdShow){
#else
int main(int argc,char **argv){
#endif
	ConvexTest app;
	app.create();
	app.setFullscreen(false);
	app.start(true);
	app.destroy();
	return 0;
}

ConvexTest::ConvexTest(){
}

void ConvexTest::create(){
	Application::create();

	mScene=new HopScene(mEngine);
	mEngine->setScene(mScene);

	mCamera=new CameraEntity(mEngine);
	mCamera->setLookAt(Vector3(0,-Math::fromInt(15),0),Math::ZERO_VECTOR3,Math::Z_UNIT_VECTOR3);
	mScene->attach(mCamera);

	setupTest();
}

ConvexTest::~ConvexTest(){
}

void ConvexTest::update(int dt){
	mNode->setRotate(0,0,Math::ONE,Math::fromMilli(mScene->getVisualTime())*4);

	mScene->update(dt);
}

void ConvexTest::render(Renderer *renderer){
	mEngine->contextUpdate(renderer);

	renderer->beginScene();

	mScene->render(renderer,mCamera);

	renderer->endScene();

	renderer->swap();

	setTitle(String("FPS:")+mCamera->getFramesPerSecond());
}

void ConvexTest::mousePressed(int x,int y,int button){
	stop();
}

void ConvexTest::resized(int width,int height){
	if(width!=0 && height!=0){
		if(width>=height){
			mCamera->setProjectionFovY(Math::degToRad(Math::fromInt(45)),Math::div(Math::fromInt(width),Math::fromInt(height)),Math::fromMilli(100),Math::fromInt(100));
		}
		else{
			mCamera->setProjectionFovX(Math::degToRad(Math::fromInt(45)),Math::div(Math::fromInt(height),Math::fromInt(width)),Math::fromMilli(100),Math::fromInt(100));
		}
	}
}

void ConvexTest::setupTest(){
	if(mNode!=NULL){
		mNode->destroy();
	}
	mNode=new ParentEntity(mEngine);
	mScene->attach(mNode);
	mScene->setRenderCollisionVolumes(true,true,1);

	Shape *sphere=new Shape(Sphere(Math::ONE/2));
	HopEntity::ptr sphereEntity=new HopEntity(mEngine);
	sphereEntity->getSolid()->addShape(sphere);
	sphereEntity->setCoefficientOfRestitution(0.9);
	sphereEntity->setTranslate(Vector3(0.0,0.0,2.0));
	mScene->attach(sphereEntity);

	ConvexSolid cs;
	cs.planes.push_back(Plane(Vector3(1.0,0.0,0.0),0.5));
	cs.planes.push_back(Plane(Vector3(-1.0,0.0,0.0),0.5));
	cs.planes.push_back(Plane(Vector3(0.0,1.0,0.0),0.5));
	cs.planes.push_back(Plane(Vector3(0.0,-1.0,0.0),0.5));
	cs.planes.push_back(Plane(Vector3(1.0,1.0,1.0),0.5));
	cs.planes.push_back(Plane(Vector3(0.0,0.0,-1.0),0.5));
	Shape *cshape=new Shape(cs);
	HopEntity::ptr convexEntity=new HopEntity(mEngine);
	convexEntity->getSolid()->addShape(cshape);
	convexEntity->setCoefficientOfGravity(0);
	convexEntity->setCoefficientOfRestitution(0.9);
	convexEntity->setInfiniteMass();
	mScene->attach(convexEntity);

	//MeshEntity::ptr cubeEntity=new MeshEntity(mEngine);
	//cubeEntity->load(makeConvexSolid(mEngine));
	//mNode->attach(cubeEntity);
	
	mScene->setGravity(Vector3(0.0,0.0,-Math::ONE));
}

Mesh::ptr ConvexTest::makeConvexSolid(Engine *engine){
	VertexFormat vertexFormat(2);
	int formatBit=engine->getIdealFormatBit();
	vertexFormat.addVertexElement(VertexElement(VertexElement::Type_POSITION,formatBit|VertexElement::Format_BIT_COUNT_3));
	vertexFormat.addVertexElement(VertexElement(VertexElement::Type_COLOR,VertexElement::Format_COLOR_RGBA));
	VertexBuffer::ptr vertexBuffer=engine->loadVertexBuffer(VertexBuffer::ptr(new VertexBuffer(Buffer::UsageType_STATIC,Buffer::AccessType_WRITE_ONLY,vertexFormat,8)));
	{
		VertexBufferAccessor vba;
		vba.lock(vertexBuffer,Buffer::LockType_WRITE_ONLY);

		int i=0;
		vba.set3(i,0,-Math::HALF,-Math::HALF,-Math::HALF); vba.setRGBA(i,1,0xFF0000FF); i++;
		vba.set3(i,0, Math::HALF,-Math::HALF,-Math::HALF); vba.setRGBA(i,1,0x00FF00FF); i++;
		vba.set3(i,0,-Math::HALF, Math::HALF,-Math::HALF); vba.setRGBA(i,1,0x0000FFFF); i++;
		vba.set3(i,0, Math::HALF, Math::HALF,-Math::HALF); vba.setRGBA(i,1,0xFFFF00FF); i++;
		vba.set3(i,0,-Math::HALF,-Math::HALF, Math::HALF); vba.setRGBA(i,1,0xFF00FFFF); i++;
		vba.set3(i,0, Math::HALF,-Math::HALF, Math::HALF); vba.setRGBA(i,1,0x00FFFFFF); i++;
		vba.set3(i,0,-Math::HALF, Math::HALF, Math::HALF); vba.setRGBA(i,1,0xFFFFFFFF); i++;
		vba.set3(i,0, Math::HALF, Math::HALF, Math::HALF); vba.setRGBA(i,1,0x000000FF); i++;

		vba.unlock();
	}

	IndexBuffer::ptr indexBuffer=engine->loadIndexBuffer(IndexBuffer::ptr(new IndexBuffer(Buffer::UsageType_STATIC,Buffer::AccessType_WRITE_ONLY,IndexBuffer::IndexFormat_UINT_16,36)));
	{
		IndexBufferAccessor iba;
		iba.lock(indexBuffer,Buffer::LockType_WRITE_ONLY);

		int i=0;
		// Bottom
		iba.set(i++,2);		iba.set(i++,1);		iba.set(i++,0);
		iba.set(i++,3);		iba.set(i++,1);		iba.set(i++,2);

		// Top
		iba.set(i++,4);		iba.set(i++,5);		iba.set(i++,6);
		iba.set(i++,6);		iba.set(i++,5);		iba.set(i++,7);

		// Left
		iba.set(i++,4);		iba.set(i++,2);		iba.set(i++,0);
		iba.set(i++,6);		iba.set(i++,2);		iba.set(i++,4);

		// Right
		iba.set(i++,1);		iba.set(i++,3);		iba.set(i++,5);
		iba.set(i++,5);		iba.set(i++,3);		iba.set(i++,7);

		// Front
		iba.set(i++,0);		iba.set(i++,1);		iba.set(i++,4);
		iba.set(i++,4);		iba.set(i++,1);		iba.set(i++,5);

		// Back
		iba.set(i++,6);		iba.set(i++,3);		iba.set(i++,2);
		iba.set(i++,7);		iba.set(i++,3);		iba.set(i++,6);
	}

	Mesh::SubMesh::ptr subMesh(new Mesh::SubMesh());
	subMesh->indexData=IndexData::ptr(new IndexData(IndexData::Primitive_TRIS,indexBuffer));
	subMesh->material=Material::ptr(new Material());
	subMesh->material->setFaceCulling(Renderer::FaceCulling_BACK);

	Mesh::ptr mesh(new Mesh());
	mesh->subMeshes.resize(1);
	mesh->subMeshes[0]=subMesh;
	mesh->staticVertexData=VertexData::ptr(new VertexData(vertexBuffer));

	return mesh;
}

// http://www.cfxweb.net/modules.php?name=News&file=print&sid=1849
Mesh::ptr ConvexTest::makeSphere(Engine *engine,int bandPower){
	int bandPoints=1<<bandPower;
	int bandMask=bandPoints-2;
	int sectionsInBand=(bandPoints/2)-1;
	int totalPoints=sectionsInBand*bandPoints;
	scalar sectionArc=Math::TWO_PI/sectionsInBand;
	scalar radius=Math::HALF;

	VertexFormat vertexFormat(2);
	int formatBit=engine->getIdealFormatBit();
	vertexFormat.addVertexElement(VertexElement(VertexElement::Type_POSITION,formatBit|VertexElement::Format_BIT_COUNT_3));
	vertexFormat.addVertexElement(VertexElement(VertexElement::Type_COLOR,VertexElement::Format_COLOR_RGBA));
	VertexBuffer::ptr vertexBuffer=engine->loadVertexBuffer(VertexBuffer::ptr(new VertexBuffer(Buffer::UsageType_STATIC,Buffer::AccessType_WRITE_ONLY,vertexFormat,totalPoints)));
	{
		VertexBufferAccessor vba;
		vba.lock(vertexBuffer,Buffer::LockType_WRITE_ONLY);

		int i=0,j=0;
		scalar xAngle=0,yAngle=0;
		for(i=0;i<totalPoints;++i){
			xAngle=Math::fromInt((i&1)+(i>>bandPower));
			yAngle=Math::fromInt(((i&bandMask)>>1)+((i>>bandPower)*(sectionsInBand)));
			xAngle=Math::mul(xAngle,sectionArc/2);
			yAngle=Math::mul(yAngle,-sectionArc);

			vba.set3(j,0,
				Math::mul(-radius,Math::mul(Math::sin(xAngle),Math::sin(yAngle))),
				Math::mul(-radius,Math::cos(xAngle)),
				Math::mul(-radius,Math::mul(Math::sin(xAngle),Math::cos(yAngle))));
			vba.setRGBA(j,1,
				Color::lerp(Colors::RED.getRGBA(),Colors::BLUE.getRGBA(),Math::div(Math::fromInt(i),Math::fromInt(totalPoints))));
			j++;
		}
	}

	Mesh::SubMesh::ptr subMesh(new Mesh::SubMesh());
	subMesh->indexData=IndexData::ptr(new IndexData(IndexData::Primitive_TRISTRIP,NULL,0,vertexBuffer->getSize()));
	subMesh->material=Material::ptr(new Material());
	subMesh->material->setFaceCulling(Renderer::FaceCulling_BACK);

	Mesh::ptr mesh(new Mesh());
	mesh->subMeshes.resize(1);
	mesh->subMeshes[0]=subMesh;
	mesh->staticVertexData=VertexData::ptr(new VertexData(vertexBuffer));

	return mesh;
}
