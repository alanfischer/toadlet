#include "Bindings.h"
#include <toadlet/egg/String.h>
#include <toadlet/egg/Log.h>
#include <toadlet/egg/Error.h>
#include <toadlet/egg/Resource.h>
#include <toadlet/egg/Object.h>
#include <toadlet/egg/math/Math.h>
#include <toadlet/egg/io/Stream.h>
#include <toadlet/egg/io/MemoryStream.h>
#include <toadlet/egg/io/BaseArchive.h>

using namespace emscripten;
using namespace toadlet;
using namespace toadlet::egg;
using namespace toadlet::egg::math;

String Exception_getDescription(const Exception &ex){
	return ex.getDescription();
}

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

	class_<Log>("Log")
		.class_function("initialize", &Log::initialize)
		.class_function("destroy", &Log::destroy)
		.class_function("error", select_overload<void(const String&)>(&Log::error))
		.class_function("warning", select_overload<void(const String&)>(&Log::warning))
		.class_function("alert", select_overload<void(const String&)>(&Log::alert))
		.class_function("debug", select_overload<void(const String&)>(&Log::debug))
		.class_function("excess", select_overload<void(const String&)>(&Log::excess))
	;

	class_<Exception>("Exception")
		.function("getError", &Exception::getError)
		.function("getDescription", &Exception_getDescription)
	;

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

	class_<Resource>("Resource")
		.smart_ptr<Resource::ptr>()
	;

	class StreamRequestWrapper: public Object,public wrapper<StreamRequest> {
	public:
		TOADLET_IOBJECT(StreamRequestWrapper);
		EMSCRIPTEN_WRAPPER(StreamRequestWrapper);

		void streamReady(Stream *stream){
			return call<void>("streamReady",Stream::ptr(stream));
		}

		void streamException(const Exception &ex){
			return call<void>("streamException",ex);
		}

		void streamProgress(float progress){
			return call<void>("streamProgress",progress);
		}
	};

	class_<StreamRequest>("StreamRequest")
		.smart_ptr<StreamRequest::ptr>()
		.allow_subclass<StreamRequestWrapper, StreamRequestWrapper::ptr>()
		.function("streamReady", &StreamRequest::streamReady, allow_raw_pointers())
		.function("stringStreamReady", &StreamRequest_stringStreamReady, allow_raw_pointers())
		.function("streamException", &StreamRequest::streamException)
		.function("streamProgress", &StreamRequest::streamProgress)
	;

	class ResourceRequestWrapper: public Object,public wrapper<ResourceRequest> {
	public:
		TOADLET_IOBJECT(ResourceRequestWrapper);
		EMSCRIPTEN_WRAPPER(ResourceRequestWrapper);

		void resourceReady(Resource *resource){
			return call<void>("resourceReady",Resource::ptr(resource));
		}

		void resourceException(const Exception &ex){
			return call<void>("resourceException",ex);
		}

		void resourceProgress(float progress){
			return call<void>("resourceProgress",progress);
		}
	};

	class_<ResourceRequest>("ResourceRequest")
		.smart_ptr<ResourceRequest::ptr>()
		.allow_subclass<ResourceRequestWrapper, ResourceRequestWrapper::ptr>()

		.function("resourceReady", &ResourceRequest::resourceReady, allow_raw_pointers())
		.function("resourceException", &ResourceRequest::resourceException)
		.function("resourceProgress", &ResourceRequest::resourceProgress)
	;

	class ArchiveWrapper: public wrapper<BaseArchive> {
	public:
		TOADLET_IOBJECT(ArchiveWrapper);
		EMSCRIPTEN_WRAPPER(ArchiveWrapper);

		bool openStream(const String &name,StreamRequest *request){
			return call<bool>("openStream",name,StreamRequest::ptr(request));
		}

		bool openResource(const String &name,ResourceRequest *request){
			return call<bool>("openResource",name,ResourceRequest::ptr(request));
		}

		const Collection<String> &getEntries(){return mEntries;}

	protected:
		Collection<String> mEntries;
	};

	class_<Archive,base<Resource>>("Archive")
		.smart_ptr<Archive::ptr>()
		.allow_subclass<ArchiveWrapper, ArchiveWrapper::ptr>()

		.function("openStream", &Archive::openStream, allow_raw_pointers())
		.function("openResource", &Archive::openResource, allow_raw_pointers())
	;
}
