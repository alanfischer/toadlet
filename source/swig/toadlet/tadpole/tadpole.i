%module tadpole

%module(directors="1") tadpole

%include <arrays_java.i>


// Ribbit accessors
namespace toadlet{
namespace ribbit{
class AudioDevice;
%typemap(jtype) AudioDevice * "long"
%typemap(jstype) AudioDevice * "us.toadlet.ribbit.AudioDevice"
%typemap(in) AudioDevice * "$1=*(toadlet::ribbit::AudioDevice**)&$input;"
%typemap(javain) AudioDevice * "us.toadlet.ribbit.AudioDevice.getCPtr($javainput)"
}
}
using namespace toadlet::ribbit;


// Peeper accessors
namespace toadlet{
namespace peeper{
class RenderDevice;
%typemap(jtype) RenderDevice * "long"
%typemap(jstype) RenderDevice * "us.toadlet.peeper.RenderDevice"
%typemap(in) RenderDevice * "$1=*(toadlet::peeper::RenderDevice**)&$input;"
%typemap(javain) RenderDevice * "us.toadlet.peeper.RenderDevice.getCPtr($javainput)"
}
}
using namespace toadlet::peeper;


namespace toadlet{
namespace peeper{
class RenderState;
%typemap(jtype) RenderState * "long"
%typemap(jstype) RenderState * "us.toadlet.peeper.RenderState"
%typemap(javaout) RenderState * "{return new us.toadlet.peeper.RenderState($jnicall,$owner);}"
}
}
using namespace toadlet::peeper;


%include <toadlet/tadpole/Engine.i>
%include <toadlet/tadpole/Scene.i>
%include <toadlet/tadpole/Component.i>
%include <toadlet/tadpole/ActionComponent.i>
%include <toadlet/tadpole/AnimationActionComponent.i>
%include <toadlet/tadpole/AudioComponent.i>
%include <toadlet/tadpole/Node.i>
%include <toadlet/tadpole/CameraNode.i>
%include <toadlet/tadpole/MeshNode.i>
%include <toadlet/tadpole/LightNode.i>
%include <toadlet/tadpole/animation/Animation.i>
%include <toadlet/tadpole/animation/BaseAnimation.i>
%include <toadlet/tadpole/animation/MeshAnimation.i>
%{
using namespace toadlet;
using namespace toadlet::tadpole;
using namespace toadlet::tadpole::animation;
using namespace toadlet::tadpole::node;
%}

typedef float scalar;
