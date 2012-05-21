%module peeper

%module(directors="1") peeper

%include <toadlet/peeper/RenderTarget.i>
%include <toadlet/peeper/RenderDevice.i>
%{
using namespace toadlet;
using namespace toadlet::peeper;
%}

typedef float scalar;
