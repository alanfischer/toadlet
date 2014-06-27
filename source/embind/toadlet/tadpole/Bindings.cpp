#include "../egg/Bindings.h"
#include <toadlet/tadpole/Action/Action.h>
#include <toadlet/tadpole/Action/AnimationAction.h>
#include <toadlet/tadpole/Colors.h>
#include <toadlet/tadpole/Engine.h>
#include <toadlet/tadpole/Scene.h>
#include <toadlet/tadpole/Mesh.h>
#include <toadlet/tadpole/Transform.h>
#include <toadlet/tadpole/Component.h>
#include <toadlet/tadpole/ActionComponent.h>
#include <toadlet/tadpole/CameraComponent.h>
#include <toadlet/tadpole/MeshComponent.h>
#include <toadlet/tadpole/Node.h>

using namespace emscripten;
using namespace toadlet;
using namespace toadlet::egg;
using namespace toadlet::tadpole;

Texture::ptr TextureManager_createTexture(TextureManager *manager,TextureFormat::ptr format){
	return manager->createTexture(format,(tbyte*)NULL);
}

extern "C" void *DOMArchive();

Archive::ptr makeDOMArchive(Engine *engine){
	volatile bool link=false;
	if(link){DOMArchive();}
	return val::module_property("DOMArchive")(engine).as<Archive::ptr>();
}

Mesh *toMesh(Resource *resource){
	return (Mesh*)resource;
}

Material *toMaterial(Resource *resource){
	return (Material*)resource;
}

EMSCRIPTEN_BINDINGS(tadpole) {
	using namespace emscripten::internal;

    constant("WHITE", Colors::WHITE);
    constant("RED", Colors::RED);
    constant("ROSE", Colors::ROSE);
    constant("ORANGE", Colors::ORANGE);
    constant("YELLOW", Colors::YELLOW);
    constant("MAGENTA", Colors::MAGENTA);
    constant("GREEN", Colors::GREEN);
    constant("CYAN", Colors::CYAN);
    constant("SPRING_GREEN", Colors::SPRING_GREEN);
    constant("CHARTREUSE", Colors::CHARTREUSE);
    constant("BLUE", Colors::BLUE);
    constant("AZURE", Colors::AZURE);
    constant("LIGHT_VIOLET", Colors::LIGHT_VIOLET);
    constant("VIOLET", Colors::VIOLET);
    constant("GREY", Colors::GREY);
    constant("BLACK", Colors::BLACK);
    constant("BROWN", Colors::BROWN);
	
	class_<ResourceData>("ResourceData")
		.smart_ptr<ResourceData::ptr>()
	;

	class_<Mesh>("Mesh")
		.smart_ptr<Mesh::ptr>()
	;

	function("toMesh",&toMesh, allow_raw_pointers());

	class_<Material>("Material")
		.smart_ptr<Material::ptr>()
	;

	function("toMaterial",&toMaterial, allow_raw_pointers());

	class_<Skeleton>("Skeleton")
		.smart_ptr<Skeleton::ptr>()
	;

	class_<Animation>("Animation")
		.smart_ptr<Animation::ptr>()
	;
	
	class_<ResourceManager>("ResourceManager")
		.smart_ptr<ResourceManager::ptr>()

		.function("addResourceArchive",&ResourceManager::addResourceArchive, allow_raw_pointers())
		.function("removeResourceArchive",&ResourceManager::removeResourceArchive, allow_raw_pointers())
		.function("manage",&ResourceManager::manage, allow_raw_pointers())
		.function("find",select_overload<bool(const String&,ResourceRequest*,ResourceData*)>(&ResourceManager::find), allow_raw_pointers())
	;

	class_<ArchiveManager,base<ResourceManager>>("ArchiveManager")
		.smart_ptr<ArchiveManager::ptr>()
	;

	class_<TextureManager,base<ResourceManager>>("TextureManager")
		.smart_ptr<TextureManager::ptr>()

		.function("createTexture",&TextureManager_createTexture, allow_raw_pointers())

		.function("getTexture",&TextureManager::getTexture, allow_raw_pointers())
	;

	class_<MeshManager,base<ResourceManager>>("MeshManager")
		.smart_ptr<MeshManager::ptr>()
	;

	class_<Engine>("Engine")
		.smart_ptr_constructor(&make_ptr<Engine>)
		.function("destroy",&Engine::destroy)

		.function("setHasBackableShader",&Engine::setHasBackableShader)
		.function("setHasBackableFixed",&Engine::setHasBackableFixed)

		.function("installHandlers",&Engine::installHandlers)

		.function("getArchiveManager",&Engine::getArchiveManager, allow_raw_pointers())
		.function("getTextureManager",&Engine::getTextureManager, allow_raw_pointers())
		.function("getMeshManager",&Engine::getMeshManager, allow_raw_pointers())

		.function("setRenderDevice",&Engine::setRenderDevice, allow_raw_pointers())
		.function("getRenderDevice",&Engine::getRenderDevice, allow_raw_pointers())
		.function("setAudioDevice",&Engine::setAudioDevice, allow_raw_pointers())
		.function("getAudioDevice",&Engine::getAudioDevice, allow_raw_pointers())

		.function("createDiffuseMaterial",&Engine::createDiffuseMaterial, allow_raw_pointers())

		.function("createTorusMesh",&Engine::createTorusMesh, allow_raw_pointers())
		.function("createGridMesh",&Engine::createGridMesh, allow_raw_pointers())
	;

	class_<Scene>("Scene")
		.smart_ptr_constructor(&make_ptr<Scene, Engine*>)
		.function("destroy",select_overload<void()>(&Scene::destroy))

		.function("getEngine",&Scene::getEngine, allow_raw_pointers())
		.function("getRoot",&Scene::getRoot, allow_raw_pointers())
		.function("update",&Scene::update)
		.function("render",&Scene::render, allow_raw_pointers())
	;

	class_<Transform>("Transform")
		.smart_ptr_constructor(&make_ptr<Transform>)
		.function("getTranslate",&Transform::getTranslate)
		.function("setTranslate",select_overload<void(const Vector3&)>(&Transform::setTranslate))
		.function("getScale",&Transform::getScale)
		.function("setScale",select_overload<void(const Vector3&)>(&Transform::setScale))
		.function("getRotate",&Transform::getRotate)
		.function("setRotate",select_overload<void(const Quaternion&)>(&Transform::setRotate))
	;

	class_<Camera>("Camera")
		.smart_ptr_constructor(&make_ptr<Camera,Engine*>)
		.function("setClearColor",&Camera::setClearColor)
		.function("setViewport",&Camera::setViewport)
		.function("getViewport",&Camera::getViewport)
		.function("render",&Camera::render, allow_raw_pointers())
	;

	class_<Action>("Action")
		.smart_ptr<Action::ptr>()
	;

	enum_<AnimationAction::Cycling>("Cycling")
		.value("Cycling_NONE", AnimationAction::Cycling_NONE)
		.value("Cycling_LOOP", AnimationAction::Cycling_LOOP)
		.value("Cycling_REFLECT", AnimationAction::Cycling_REFLECT)
	;
	
	class_<AnimationAction,base<Action>>("AnimationAction")
		.smart_ptr_constructor(&make_ptr<AnimationAction,Animation*>)
		.function("setTimeScale",&AnimationAction::setTimeScale)
		.function("getTimeScale",&AnimationAction::getTimeScale)
		.function("setCycling",&AnimationAction::setCycling)
		.function("getCycling",&AnimationAction::getCycling)
		.function("setStopGently",&AnimationAction::setStopGently)
		.function("getStopGently",&AnimationAction::getStopGently)
	;

	class_<Component>("Component")
		.smart_ptr<Component::ptr>()
		.function("destroy",&Component::destroy)
		.function("getName",&Component::getName)
	;

	class_<CameraComponent,base<Component>>("CameraComponent")
		.smart_ptr_constructor(&make_ptr<CameraComponent,Camera*>)
		.function("setClearColor",&CameraComponent::setClearColor)
		.function("getCamera",&CameraComponent::getCamera, allow_raw_pointers())
		.function("setLookAt",&CameraComponent::setLookAt)
		.function("render",&CameraComponent::render, allow_raw_pointers())
	;

	class_<MeshComponent,base<Component>>("MeshComponent")
		.smart_ptr_constructor(&make_ptr<MeshComponent,Engine*>)
		.function("setMeshWithName",select_overload<void(const String&)>(&MeshComponent::setMesh))
		.function("setMeshWithMesh",select_overload<void(Mesh*)>(&MeshComponent::setMesh), allow_raw_pointers())
		.function("getSkeleton",&MeshComponent::getSkeleton, allow_raw_pointers())
	;

	class_<SkeletonComponent,base<Component>>("SkeletonComponent")
		.smart_ptr_constructor(&make_ptr<SkeletonComponent,Engine*,Skeleton*>)
		.function("getAnimationWithName",select_overload<Animation *(const String&)>(&SkeletonComponent::getAnimation), allow_raw_pointers())
		.function("getAnimationWithIndex",select_overload<Animation *(int)>(&SkeletonComponent::getAnimation), allow_raw_pointers())
	;
	
	class_<LightComponent,base<Component>>("LightComponent")
		.smart_ptr_constructor(&make_ptr<LightComponent>)
		.function("setEnabled",&LightComponent::setEnabled)
		.function("getEnabled",&LightComponent::getEnabled)
		.function("setLightState",&LightComponent::setLightState)
		.function("getLightState",&LightComponent::getLightState)
		.function("setDirection",&LightComponent::setDirection)
		.function("getDirection",&LightComponent::getDirection)
	;

	class_<ActionComponent,base<Component>>("ActionComponent")
		.smart_ptr_constructor(&make_ptr<ActionComponent,const String&,Action*>)
	;
	
	class_<BaseComponent,base<Component>>("BaseComponent")
		.function("setName",&BaseComponent::setName)
	;

	class_<Node,base<BaseComponent>>("Node")
		.smart_ptr_constructor(&make_ptr<Node,Scene*>)
		.function("destroy",&Node::destroy)

		.function("getRoot",&Node::getRoot, allow_raw_pointers())

		.function("attach",&Node::attach, allow_raw_pointers())
		.function("remove",&Node::remove, allow_raw_pointers())

		.function("setScope",&Node::setScope)
		.function("getScope",&Node::getScope)

		.function("setTransform",&Node::setTransform, allow_raw_pointers())
		.function("getTransform",&Node::getTransform, allow_raw_pointers())
		.function("getWorldTransform",&Node::getWorldTransform, allow_raw_pointers())

		.function("getNodes", &Node::getNodes)

		.function("startAction",&Node::startAction)
		;

	class_<PartitionNode,base<Node>>("PartitionNode")
		.smart_ptr<PartitionNode::ptr>()
	;

	register_range<Node>("NodeRange");
}
