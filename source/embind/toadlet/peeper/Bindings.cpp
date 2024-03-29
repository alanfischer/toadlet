#include "../egg/Bindings.h"
#include <toadlet/peeper/WindowRenderTargetFormat.h>
#include <toadlet/peeper/LightState.h>
#include <toadlet/peeper/RenderTarget.h>
#include <toadlet/peeper/RenderDevice.h>
#include <toadlet/peeper/RenderState.h>
#include <toadlet/peeper/Texture.h>
#include <toadlet/peeper/Viewport.h>

using namespace emscripten;
using namespace toadlet::egg;
using namespace toadlet::peeper;

extern "C" RenderDevice *new_GLES2RenderDevice();
extern "C" RenderTarget *new_EGLWindowRenderTarget(void *display,void *window,WindowRenderTargetFormat *format,RenderTarget *shareTarget);

RenderTarget *new_EGLWindowRenderTarget2(val canvas){
	val::global("Module").set("canvas",canvas);
	WindowRenderTargetFormat format;
	format.flags=2;
	return new_EGLWindowRenderTarget(NULL,NULL,&format,NULL);
}

EMSCRIPTEN_BINDINGS(peeper) {
	using namespace emscripten::internal;

	value_array<Viewport>("Viewport")
		.element(&Viewport::x)
		.element(&Viewport::y)
		.element(&Viewport::width)
		.element(&Viewport::height)
		.element(&Viewport::empty)
	;

	class_<RenderTarget>("RenderTarget")
		.smart_ptr<RenderTarget::ptr>("RenderTargetPtr")
	;

	class_<RenderDevice>("RenderDevice")
		.smart_ptr<RenderDevice::ptr>("RenderDevice_ptr")
		.function("create",&RenderDevice::create, allow_raw_pointers())
		.function("beginScene",&RenderDevice::beginScene)
		.function("endScene",&RenderDevice::endScene)
		.function("swap",&RenderDevice::swap);
	;

	function("new_GLES2RenderDevice",&new_GLES2RenderDevice, allow_raw_pointers());
	function("new_EGLWindowRenderTarget",&new_EGLWindowRenderTarget2, allow_raw_pointers());

	class_<RenderState>("RenderState")
		.smart_ptr<RenderState::ptr>("RenderState_ptr")
	;
    
	enum_<TextureFormat::Dimension>("Dimension")
		.value("Dimension_UNKNOWN", TextureFormat::Dimension_UNKNOWN)
		.value("Dimension_D1", TextureFormat::Dimension_D1)
		.value("Dimension_D2", TextureFormat::Dimension_D2)
		.value("Dimension_D3", TextureFormat::Dimension_D3)
		.value("Dimension_CUBE", TextureFormat::Dimension_CUBE)
	;

	enum_<TextureFormat::Format>("Format")
		.value("Format_RGBA_8", TextureFormat::Format_RGBA_8)
	;

	class_<TextureFormat>("TextureFormat")
		.smart_ptr_constructor("TextureFormat_ptr",&make_ptr<TextureFormat,int,int,int,int,int,int>)
	;

	class_<Texture,base<Resource>>("Texture")
		.smart_ptr<Texture::ptr>("Texture_ptr")
	;

	function("toTexture",select_overload<Texture*(Resource*)>(
		[](Resource *resource){return (Texture*)resource;}
	), allow_raw_pointers());

	enum_<LightState::Type>("LightType")
		.value("Type_DIRECTION", LightState::Type_DIRECTION)
		.value("Type_POINT", LightState::Type_POINT)
		.value("Type_SPOT", LightState::Type_SPOT)
	;
	
    class_<LightState>("LightState")
        .constructor()
		.property("type",&LightState::type)
		.property("specularColor",&LightState::specularColor)
		.property("diffuseColor",&LightState::diffuseColor)
		.property("constantAttenuation",&LightState::constantAttenuation)
		.property("linearAttenuation",&LightState::linearAttenuation)
		.property("quadraticAttenuation",&LightState::quadraticAttenuation)
		.property("spotInnerRadius",&LightState::spotInnerRadius)
		.property("spotOuterRadius",&LightState::spotOuterRadius)
		.property("spotFalloff",&LightState::spotFalloff)
		.property("radius",&LightState::radius)
		.property("position",&LightState::position)
		.property("direction",&LightState::direction)
	;
}
