#include "Bindings.h"
#include <toadlet/egg/String.h>
#include <toadlet/egg/Log.h>
#include <toadlet/egg/Error.h>
#include <toadlet/egg/math/Math.h>
#include <toadlet/egg/io/Archive.h>

using namespace emscripten;
using namespace toadlet::egg;
using namespace toadlet::egg::math;

EMSCRIPTEN_BINDINGS(egg) {
	using namespace emscripten::internal;

	_embind_register_std_wstring(TypeID<String>::get(), sizeof(stringchar), "String");

	value_array<Vector2>("Vector2")
		.element(&Vector2::x)
		.element(&Vector2::y)
	;

	value_array<Vector3>("Vector3")
		.element(&Vector3::x)
		.element(&Vector3::y)
		.element(&Vector3::z)
	;

	value_array<Vector4>("Vector4")
		.element(&Vector4::x)
		.element(&Vector4::y)
		.element(&Vector4::z)
		.element(&Vector4::w)
	;

	value_array<Quaternion>("Quaternion")
		.element(&Quaternion::x)
		.element(&Quaternion::y)
		.element(&Quaternion::z)
		.element(&Quaternion::w)
	;

	class_<Log>("Log")
		.class_function("initialize", &Log::initialize)
		.class_function("destroy", &Log::destroy)
		.class_function("error", select_overload<void(const String&)>(&Log::error))
		.class_function("warning", select_overload<void(const String&)>(&Log::warning))
		.class_function("alert", select_overload<void(const String&)>(&Log::alert))
		.class_function("debug", select_overload<void(const String&)>(&Log::debug))
		.class_function("excess", select_overload<void(const String&)>(&Log::excess))
	;

	class_<Exception>("Exception");

	class_<Error>("Error")
		.class_function("unknown", select_overload<Exception(const String&)>(&Error::unknown))
		.class_function("tassert", select_overload<Exception(const String&)>(&Error::tassert))
		.class_function("invalidParameters", select_overload<Exception(const String&)>(&Error::invalidParameters))
		.class_function("nullPointer", select_overload<Exception(const String&)>(&Error::nullPointer))
		.class_function("unimplemented", select_overload<Exception(const String&)>(&Error::unimplemented))
	;

	class_<Stream>("Stream")
		.smart_ptr<Stream::ptr>()

		.function("close", &Stream::close)
		.function("closed", &Stream::closed)

		.function("readable", &Stream::readable)
		.function("read", &Stream::read, allow_raw_pointers())

		.function("writeable", &Stream::writeable)
		.function("write", &Stream::write, allow_raw_pointers())

		.function("reset", &Stream::reset)
		.function("length", &Stream::length)
		.function("position", &Stream::position)
		.function("seek", &Stream::seek)
		.function("flush", &Stream::flush)
	;

//	class_<Archive>("Archive")
//		.smart_ptr<Archive::ptr>()

//		.function("openStream", &Archive::openStream)
//	;
}
