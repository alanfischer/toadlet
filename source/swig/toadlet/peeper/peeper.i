%module peeper

%module(directors="1") peeper

%include <arrays_java.i>
%include <../toadlet_egg.i>
%include <enumtypeunsafe.swg>
%javaconst(1);

%include <toadlet/peeper/RenderTarget.i>
%include <toadlet/peeper/RenderDevice.i>
%include <toadlet/peeper/RenderState.i>
%include <toadlet/peeper/TextureFormat.i>
%include <toadlet/peeper/GLRenderTarget.i>
%include <toadlet/peeper/WindowRenderTargetFormat.i>
%{
using namespace toadlet;
using namespace toadlet::peeper;
%}
