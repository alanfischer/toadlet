%module peeper

%module(directors="1") peeper

%include <toadlet/peeper/RenderTarget.i>
%include <toadlet/peeper/RenderDevice.i>
%include <toadlet/peeper/RenderState.i>
%include <toadlet/peeper/GLRenderTarget.i>
%include <toadlet/peeper/WindowRenderTargetFormat.i>
%{
using namespace toadlet;
using namespace toadlet::peeper;
%}

typedef float scalar;
