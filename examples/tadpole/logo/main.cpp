#if !defined(TOADLET_PLATFORM_IPHONE)
#	include "../../UniformMain.cpp"
#endif

#include <toadlet/peeper/Colors.h>
#include <toadlet/peeper/Viewport.h>
#include <toadlet/peeper/LightEffect.h>
#include <toadlet/tadpole/Engine.h>
#include <toadlet/tadpole/entity/AnimationControllerEntity.h>
#include <toadlet/tadpole/entity/CameraEntity.h>
#include <toadlet/tadpole/entity/MeshEntity.h>
#include <toadlet/tadpole/animation/MeshEntityAnimation.h>
#include <toadlet/pad/Application.h>
#include <toadlet/pad/ApplicationListener.h>
#include <toadlet/egg/Logger.h>

using namespace toadlet::egg;
using namespace toadlet::peeper;
using namespace toadlet::tadpole;
using namespace toadlet::tadpole::animation;
using namespace toadlet::tadpole::entity;
using namespace toadlet::pad;

class Logo:public Application{
public:
	Logo():Application(){
		Engine *engine=getEngine();

		engine->setScene(new Scene(engine));

		meshEntity=MeshEntity::ptr(new MeshEntity(engine));
		meshEntity->load(getEngine()->cacheMesh("../../data/lt.mmsh"));
		meshEntity->getAnimationController()->start();
		engine->getScene()->attach(meshEntity);

		cameraEntity=CameraEntity::ptr(new CameraEntity(engine));
		cameraEntity->setLookDir(Vector3(Math::fromInt(25),-Math::fromInt(100),0),Math::Y_UNIT_VECTOR3,Math::Z_UNIT_VECTOR3);
		cameraEntity->setClearColor(Colors::COLOR_BLUE);
		engine->getScene()->attach(cameraEntity);
	}

	virtual ~Logo(){}

	void resized(int width,int height){
		if(width>0 && height>0){
			if(width>=height){
				cameraEntity->setProjectionFovY(Math::degToRad(Math::fromInt(45)),Math::div(Math::fromInt(width),Math::fromInt(height)),Math::fromInt(10),Math::fromInt(200));
			}
			else{
				cameraEntity->setProjectionFovX(Math::degToRad(Math::fromInt(45)),Math::div(Math::fromInt(height),Math::fromInt(width)),Math::fromInt(10),Math::fromInt(200));
			}
		}
		cameraEntity->setViewport(Viewport(0,0,width,height));
	}

	void render(Renderer *renderer){
		getEngine()->contextUpdate(renderer);

		renderer->beginScene();

		getEngine()->getScene()->render(renderer,cameraEntity);

		renderer->endScene();

		renderer->swapRenderTargetBuffers();
	}

	void update(int dt){
		getEngine()->getScene()->update(dt);
	}

	MeshEntity::ptr meshEntity;
	CameraEntity::ptr cameraEntity;
};

#if !defined(TOADLET_PLATFORM_IPHONE)
int uniformMain(int argc,char **argv){
	Logo app;
	app.setSize(640,480);
	app.setFullscreen(false);
	app.start(true);
	return 0;
}
#endif

