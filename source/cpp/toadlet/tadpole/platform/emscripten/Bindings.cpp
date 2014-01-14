#include <toadlet/egg/platform/emscripten/Bindings.h>
#include <toadlet/tadpole/Node.h>

using namespace emscripten;
using namespace toadlet::egg;
using namespace toadlet::tadpole;

Transform::ptr w_getTransform(Node::ptr node){return node->getTransform();}

EMSCRIPTEN_BINDINGS(tadpole) {
    using namespace emscripten::internal;

	class_<Transform>("Transform")
		.smart_ptr_constructor(&make_ptr<Transform>)
        .function("getTranslate", &Transform::getTranslate)
        .function("setTranslate", select_overload<void(const Vector3&)>(&Transform::setTranslate))
        .function("getScale", &Transform::getScale)
        .function("setScale", select_overload<void(const Vector3&)>(&Transform::setScale))
        .function("getRotate", &Transform::getRotate)
        .function("setRotate", select_overload<void(const Quaternion&)>(&Transform::setRotate))
	;

    class_<Component>("Component")
        .smart_ptr<Component::ptr>()
        .function("destroy", &Component::destroy)
	;

    class_<Node,base<Component>>("Node")
		.smart_ptr_constructor(&make_ptr<Node>)
        .function("destroy", &Node::destroy, allow_raw_pointers())

		.function("getRoot", &Node::getRoot, allow_raw_pointers())

        .function("attach", &Node::attach, allow_raw_pointers())
        .function("remove", &Node::remove, allow_raw_pointers())

		.function("setScope", &Node::setScope)
		.function("getScope", &Node::getScope)

		.function("setTransform", &Node::setTransform, allow_raw_pointers())
		.function("getTransform", &Node::getTransform, allow_raw_pointers())
		.function("getWorldTransform", &Node::getWorldTransform, allow_raw_pointers())

		.function("getNodes", &Node::getNodes)
    ;

	register_range<Node>("NodeRange");
}
