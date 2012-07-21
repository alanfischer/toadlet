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
%typemap(javaout) AudioDevice * "{return new us.toadlet.ribbit.AudioDevice($jnicall, $owner);}"
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
%typemap(javaout) RenderDevice * "{return new us.toadlet.peeper.RenderDevice($jnicall, $owner);}"
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


%include <toadlet/tadpole/Track.i>
%include <toadlet/tadpole/Sequence.i>
%include <toadlet/tadpole/Engine.i>
%include <toadlet/tadpole/Scene.i>
%include <toadlet/tadpole/Component.i>
%include <toadlet/tadpole/ActionComponent.i>
%include <toadlet/tadpole/AnimationActionComponent.i>
%include <toadlet/tadpole/AudioComponent.i>
%include <toadlet/tadpole/Node.i>
%include <toadlet/tadpole/CameraComponent.i>
%include <toadlet/tadpole/MeshComponent.i>
%include <toadlet/tadpole/LightComponent.i>
%include <toadlet/tadpole/animation/Animation.i>
%include <toadlet/tadpole/animation/BaseAnimation.i>
%include <toadlet/tadpole/animation/MeshAnimation.i>
%include <toadlet/tadpole/animation/MaterialAnimation.i>
%{
using namespace toadlet;
using namespace toadlet::tadpole;
using namespace toadlet::tadpole::animation;
using namespace toadlet::tadpole::node;
%}

typedef float scalar;

%newobject createColorTrack;
%inline %{
toadlet::tadpole::Track *createColorTrack(toadlet::tadpole::Engine *engine){
	return engine->getBufferManager()->createColorTrack();
}
%}

%inline %{
#include <toadlet/egg/Collection.h>
#include <toadlet/egg/DynamicLibrary.h>
#include <toadlet/peeper/RenderDevice.h>
#include <toadlet/ribbit/AudioDevice.h>

Collection<DynamicLibrary::ptr> libraries;
toadlet::peeper::RenderDevice *new_RenderDevice(char *name){
	DynamicLibrary::ptr library(new DynamicLibrary());
	if(library->load(name,(char*)NULL,(char*)NULL)){
		libraries.add(library);
		RenderDevice *(*creator)()=(RenderDevice*(*)())library->getSymbol("new_RenderDevice");
		if(creator!=NULL){
			return creator();
		}
	}
	return NULL;
}
toadlet::ribbit::AudioDevice *new_AudioDevice(char *name){
	DynamicLibrary::ptr library(new DynamicLibrary());
	if(library->load(name,(char*)NULL,(char*)NULL)){
		libraries.add(library);
		AudioDevice *(*creator)()=(AudioDevice*(*)())library->getSymbol("new_AudioDevice");
		if(creator!=NULL){
			return creator();
		}
	}
	return NULL;
}
%}
