#include "../egg/Bindings.h"
#include <toadlet/peeper/WindowRenderTargetFormat.h>
#include <toadlet/peeper/RenderTarget.h>
#include <toadlet/peeper/RenderDevice.h>

using namespace emscripten;
using namespace toadlet::egg;
using namespace toadlet::peeper;

extern "C" RenderDevice *new_GLES2RenderDevice();
extern "C" RenderTarget *new_EGLWindowRenderTarget(void *display,void *window,WindowRenderTargetFormat *format);

RenderTarget *new_EGLWindowRenderTarget2(val canvas){
	val::global("Module").set("canvas",canvas);
	WindowRenderTargetFormat format;
	return new_EGLWindowRenderTarget(NULL,NULL,&format);
}

EMSCRIPTEN_BINDINGS(peeper) {
    using namespace emscripten::internal;

	class_<RenderTarget>("RenderTarget")
		.smart_ptr<RenderTarget::ptr>()
	;

	class_<RenderDevice>("RenderDevice")
		.smart_ptr<RenderDevice::ptr>()
		.function("create",&RenderDevice::create, allow_raw_pointers());
	;

	function("new_GLES2RenderDevice",&new_GLES2RenderDevice, allow_raw_pointers());
	function("new_EGLWindowRenderTarget",&new_EGLWindowRenderTarget2, allow_raw_pointers());
}
