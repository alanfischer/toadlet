#include "../egg/Bindings.h"
#include <toadlet/tadpole/Engine.h>
#include <toadlet/tadpole/Scene.h>
#include <toadlet/tadpole/Transform.h>
#include <toadlet/tadpole/Component.h>
#include <toadlet/tadpole/Node.h>

using namespace emscripten;
using namespace toadlet::egg;
using namespace toadlet::tadpole;

EMSCRIPTEN_BINDINGS(tadpole) {
    using namespace emscripten::internal;

	class_<Engine>("Engine")
		.smart_ptr_constructor(&make_ptr<Engine>)
		.function("destroy",&Engine::destroy)

		.function("installHandlers",&Engine::installHandlers)
		.function("setRenderDevice",&Engine::setRenderDevice, allow_raw_pointers())
		.function("setAudioDevice",&Engine::setAudioDevice, allow_raw_pointers())
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

    class_<Component>("Component")
        .smart_ptr<Component::ptr>()
        .function("destroy",&Component::destroy)
	;

    class_<Node,base<Component>>("Node")
		.smart_ptr_constructor(&make_ptr<Node>)
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

	register_range<Node>("NodeRange");
}
