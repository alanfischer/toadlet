#if !defined(TOADLET_PLATFORM_IPHONE)
#	include "../../UniformMain.cpp"
#endif

#include <toadlet/peeper/Colors.h>
#include <toadlet/peeper/VertexData.h>
#include <toadlet/peeper/IndexData.h>
#include <toadlet/peeper/Viewport.h>
#include <toadlet/peeper/LightEffect.h>
#include <toadlet/tadpole/Engine.h>
#include <toadlet/tadpole/entity/CameraEntity.h>
#include <toadlet/tadpole/entity/MeshEntity.h>
#include <toadlet/pad/Application.h>
#include <toadlet/egg/Logger.h>

using namespace toadlet::egg;
using namespace toadlet::peeper;
using namespace toadlet::tadpole;
using namespace toadlet::tadpole::entity;
using namespace toadlet::pad;

class TadpoleTest:public Application{
public:
	void create(){
		Application::create();

		mEngine->setScene(new Scene(mEngine));

		{
			VertexFormat vertexFormat(2);
			vertexFormat.addVertexElement(VertexElement(VertexElement::Type_POSITION,VertexElement::Format(mEngine->getIdealFormatBit()|VertexElement::Format_BIT_COUNT_3)));
			vertexFormat.addVertexElement(VertexElement(VertexElement::Type_COLOR,VertexElement::Format_COLOR_RGBA));
			VertexBuffer::ptr triVertexBuffer=mEngine->loadVertexBuffer(VertexBuffer::ptr(new VertexBuffer(Buffer::UsageType_STATIC,Buffer::AccessType_WRITE_ONLY,vertexFormat,3)));
			{
				VertexBufferAccessor a(triVertexBuffer,Buffer::LockType_WRITE_ONLY);
				a.set3(0,0, 0,Math::ONE,0);				a.setABGR(0,1, 0x000000FF);
				a.set3(1,0, -Math::ONE,-Math::ONE,0);	a.setABGR(1,1, 0x0000FF00);
				a.set3(2,0, Math::ONE,-Math::ONE,0);	a.setABGR(2,1, 0x00FF0000);
			}
			VertexData::ptr triVertexData(new VertexData(triVertexBuffer));

			IndexData::ptr triIndexData(new IndexData(IndexData::Primitive_TRISTRIP,NULL,0,3));

			Mesh::ptr mesh(new Mesh());
			mesh->staticVertexData=triVertexData;

			Mesh::SubMesh::ptr subMesh(new Mesh::SubMesh());
			mesh->subMeshes.push_back(subMesh);
			subMesh->indexData=triIndexData;

			Material::ptr material(new Material());
			subMesh->material=material;

			MeshEntity::ptr meshEntity(new MeshEntity(mEngine));
			meshEntity->load(mesh);
			meshEntity->setTranslate(-Math::fromMilli(1500),0,-Math::fromMilli(6000));
			mEngine->getScene()->attach(meshEntity);
		}

		{
			VertexFormat vertexFormat(1);
			vertexFormat.addVertexElement(VertexElement(VertexElement::Type_POSITION,VertexElement::Format(mEngine->getIdealFormatBit()|VertexElement::Format_BIT_COUNT_3)));
			VertexBuffer::ptr quadVertexBuffer=mEngine->loadVertexBuffer(VertexBuffer::ptr(new VertexBuffer(Buffer::UsageType_STATIC,Buffer::AccessType_WRITE_ONLY,vertexFormat,4)));
			{
				VertexBufferAccessor a(quadVertexBuffer,Buffer::LockType_WRITE_ONLY);

				a.set3(0,0, -Math::ONE,Math::ONE,0);
				a.set3(1,0, -Math::ONE,-Math::ONE,0);
				a.set3(2,0, Math::ONE,Math::ONE,0);
				a.set3(3,0, Math::ONE,-Math::ONE,0);
			}
			VertexData::ptr quadVertexData(new VertexData(quadVertexBuffer));

			IndexData::ptr quadIndexData(new IndexData(IndexData::Primitive_TRISTRIP,NULL,0,4));

			Mesh::ptr mesh(new Mesh());
			mesh->staticVertexData=quadVertexData;

			Mesh::SubMesh::ptr subMesh(new Mesh::SubMesh());
			mesh->subMeshes.push_back(subMesh);
			subMesh->indexData=quadIndexData;

			Material::ptr material(new Material());
			subMesh->material=material;
			mEngine->getScene()->setAmbientColor(Color(Math::HALF,Math::HALF,Math::ONE,0));
			LightEffect effect;
			effect.ambient=Color(Math::ONE,Math::ONE,Math::ONE,Math::ONE);
			material->setLightEffect(effect);
			material->setLighting(true);

			MeshEntity::ptr meshEntity(new MeshEntity(mEngine));
			meshEntity->load(mesh);
			meshEntity->setTranslate(Math::fromMilli(1500),0,-Math::fromMilli(6000));
			mEngine->getScene()->attach(meshEntity);
		}

		cameraEntity=CameraEntity::ptr(new CameraEntity(mEngine));
		mEngine->getScene()->attach(cameraEntity);
	}

	void update(int dt){
		mEngine->getScene()->update(dt);
	}

	void render(Renderer *renderer){
		mEngine->contextUpdate(renderer);

		renderer->beginScene();

		mEngine->getScene()->render(renderer,cameraEntity);

		renderer->endScene();

		renderer->swap();
	}

	void resized(int width,int height){
		if(width>=height){
			cameraEntity->setProjectionFovY(Math::degToRad(Math::fromInt(45)),Math::div(Math::fromInt(width),Math::fromInt(height)),Math::fromMilli(100),Math::fromInt(100));
		}
		else{
			cameraEntity->setProjectionFovX(Math::degToRad(Math::fromInt(45)),Math::div(Math::fromInt(height),Math::fromInt(width)),Math::fromMilli(100),Math::fromInt(100));
		}
	}

	CameraEntity::ptr cameraEntity;
};

#if !defined(TOADLET_PLATFORM_IPHONE)
int uniformMain(int argc,char **argv){
	TadpoleTest test;
	test.create();
	test.setFullscreen(false);
	test.setSize(640,480); 
	test.start(true);
	test.destroy();
	return 0;
}
#endif

