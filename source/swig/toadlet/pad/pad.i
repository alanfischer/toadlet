%module pad

%module(directors="1") pad

%include <arrays_java.i>
%include <../toadlet_egg.i>
%include <enumtypeunsafe.swg>
%javaconst(1);

// Peeper accessors
namespace toadlet{
namespace peeper{
class RenderDevice;
%typemap(jtype) RenderDevice * "long"
%typemap(jstype) RenderDevice * "us.toadlet.peeper.RenderDevice"
%typemap(javaout) RenderDevice * "{return new us.toadlet.peeper.RenderDevice($jnicall, $owner);}"
}
}
using namespace toadlet::peeper;


%include <toadlet/pad/Applet.i>

%{
using namespace toadlet;
using namespace toadlet::pad;
%}

%inline %{
#include <toadlet/peeper/RenderDevice.h>
extern "C" toadlet::peeper::RenderDevice *new_GLES1RenderDevice();
extern "C" toadlet::peeper::RenderDevice *new_GLES2RenderDevice();
%}
