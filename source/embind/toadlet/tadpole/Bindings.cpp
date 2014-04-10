#include "../egg/Bindings.h"
#include <toadlet/tadpole/Engine.h>
#include <toadlet/tadpole/Scene.h>
#include <toadlet/tadpole/Mesh.h>
#include <toadlet/tadpole/Transform.h>
#include <toadlet/tadpole/Component.h>
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

EMSCRIPTEN_BINDINGS(tadpole) {
	using namespace emscripten::internal;

	class_<ResourceData>("ResourceData")
		.smart_ptr<ResourceData::ptr>()
	;

	class_<Mesh>("Mesh")
		.smart_ptr<Mesh::ptr>()
	;

	class_<Material>("Material")
		.smart_ptr<Material::ptr>()
	;

	class_<ResourceManager>("ResourceManager")
		.smart_ptr<ResourceManager::ptr>()

		.function("addResourceArchive",&ResourceManager::addResourceArchive, allow_raw_pointers())
		.function("removeResourceArchive",&ResourceManager::removeResourceArchive, allow_raw_pointers())
		.function("manage",&ResourceManager::manage, allow_raw_pointers())
		.function("find",&ResourceManager::find, allow_raw_pointers())
	;

	class_<ArchiveManager,base<ResourceManager>>("ArchiveManager")
		.smart_ptr<ArchiveManager::ptr>()
	;

	class_<TextureManager,base<ResourceManager>>("TextureManager")
		.smart_ptr<TextureManager::ptr>()

		.function("createTexture",&TextureManager_createTexture, allow_raw_pointers())

		.function("getTexture",&TextureManager::getTexture, allow_raw_pointers())
	;

	class_<Engine>("Engine")
		.smart_ptr_constructor(&make_ptr<Engine>)
		.function("destroy",&Engine::destroy)

		.function("setHasBackableShader",&Engine::setHasBackableShader)
		.function("setHasBackableFixed",&Engine::setHasBackableFixed)

		.function("installHandlers",&Engine::installHandlers)

		.function("getArchiveManager",&Engine::getArchiveManager, allow_raw_pointers())
		.function("getTextureManager",&Engine::getTextureManager, allow_raw_pointers())

		.function("setRenderDevice",&Engine::setRenderDevice, allow_raw_pointers())
		.function("setAudioDevice",&Engine::setAudioDevice, allow_raw_pointers())

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

	class_<Component>("Component")
		.smart_ptr<Component::ptr>()
		.function("destroy",&Component::destroy)
		.function("getName",&Component::getName)
	;

	class_<CameraComponent,base<Component>>("CameraComponent")
		.smart_ptr_constructor(&make_ptr<CameraComponent,Camera*>)
		.function("setLookAt",&CameraComponent::setLookAt)
		.function("getCamera",&CameraComponent::getCamera, allow_raw_pointers())
	;

	class_<MeshComponent,base<Component>>("MeshComponent")
		.smart_ptr_constructor(&make_ptr<MeshComponent,Engine*>)
		.function("setMeshWithName",select_overload<void(const String&)>(&MeshComponent::setMesh))
		.function("setMeshWithMesh",select_overload<void(Mesh*)>(&MeshComponent::setMesh), allow_raw_pointers())
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
	;

	class_<PartitionNode,base<Node>>("PartitionNode")
		.smart_ptr<PartitionNode::ptr>()
	;

	register_range<Node>("NodeRange");
}
