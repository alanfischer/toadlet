#include "Bindings.h"
#include <toadlet/egg/String.h>
#include <toadlet/egg/Log.h>
#include <toadlet/egg/Error.h>
#include <toadlet/egg/Resource.h>
#include <toadlet/egg/math/Math.h>
#include <toadlet/egg/io/Stream.h>
#include <toadlet/egg/io/MemoryStream.h>

using namespace emscripten;
using namespace toadlet;
using namespace toadlet::egg;
using namespace toadlet::egg::math;

void StreamRequest_stringStreamReady(StreamRequest *request,const String &data){
	tbyte *rawdata=new tbyte[data.length()];
	memcpy(rawdata,data.c_str(),data.length());
	request->streamReady(new MemoryStream(rawdata,data.length(),data.length(),true));
}

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

    constant("ZERO_VECTOR3", Math::ZERO_VECTOR3);
    constant("X_UNIT_VECTOR3", Math::X_UNIT_VECTOR3);
    constant("NEG_X_UNIT_VECTOR3", Math::NEG_X_UNIT_VECTOR3);
    constant("Y_UNIT_VECTOR3", Math::Y_UNIT_VECTOR3);
    constant("NEG_Y_UNIT_VECTOR3", Math::NEG_Y_UNIT_VECTOR3);
    constant("Z_UNIT_VECTOR3", Math::Z_UNIT_VECTOR3);
    constant("NEG_Z_UNIT_VECTOR3", Math::NEG_Z_UNIT_VECTOR3);

	class_<Log>("Log")
		.class_function("initialize", select_overload<void(bool)>(
			[](bool silent){Log::initialize();}
		), allow_raw_pointers())
		.class_function("destroy", &Log::destroy)
		.class_function("error", select_overload<void(const char*)>(&Log::error), allow_raw_pointers())
		.class_function("warning", select_overload<void(const char*)>(&Log::warning), allow_raw_pointers())
		.class_function("alert", select_overload<void(const char*)>(&Log::alert), allow_raw_pointers())
		.class_function("debug", select_overload<void(const char*)>(&Log::debug), allow_raw_pointers())
		.class_function("excess", select_overload<void(const char*)>(&Log::excess), allow_raw_pointers())
	;

	class_<Exception>("Exception")
		.constructor<const String&>()
		.function("getError", &Exception::getError)
		.function("getDescription", select_overload<String(Exception*)>(
			[](Exception *ex){return String(ex->getDescription());}
		), allow_raw_pointers())
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

	class_<Resource>("Resource")
		.smart_ptr<Resource::ptr>()
	;

	class_<StreamRequest>("StreamRequest")
		.smart_ptr<StreamRequest::ptr>()
		.allow_subclass<StreamRequestWrapper, StreamRequestWrapper::ptr>()
		.function("streamReady", &StreamRequest::streamReady, allow_raw_pointers())
		.function("stringStreamReady", &StreamRequest_stringStreamReady, allow_raw_pointers())
		.function("streamException", &StreamRequest::streamException)
		.function("streamProgress", &StreamRequest::streamProgress)
	;

	class_<ResourceRequest>("ResourceRequest")
		.smart_ptr<ResourceRequest::ptr>()
		.allow_subclass<ResourceRequestWrapper, ResourceRequestWrapper::ptr>()

		.function("resourceReady", &ResourceRequest::resourceReady, allow_raw_pointers())
		.function("resourceException", &ResourceRequest::resourceException)
		.function("resourceProgress", &ResourceRequest::resourceProgress)
	;

	class_<Archive,base<Resource>>("Archive")
		.smart_ptr<Archive::ptr>()
		.allow_subclass<ArchiveWrapper, ArchiveWrapper::ptr>()

		.function("openStream", &Archive::openStream, allow_raw_pointers())
		.function("openResource", &Archive::openResource, allow_raw_pointers())
	;
}
