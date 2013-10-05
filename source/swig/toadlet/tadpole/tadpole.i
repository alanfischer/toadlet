%module tadpole

%module(directors="1") tadpole

%typemap(javaimports) SWIGTYPE %{import us.toadlet.tadpole.*;%}

%include <arrays_java.i>
%include <../toadlet_egg.i>
%include <enumtypeunsafe.swg>
%javaconst(1);

// Shared pragmas
%pragma(java) jniclassimports=%{
import us.toadlet.peeper.RenderDevice;
import us.toadlet.ribbit.AudioDevice;
%}


// Ribbit accessors
%{
#	include <toadlet/ribbit/plugins/jaudiodevice/JAudioDevice.h>
%}

namespace toadlet{
namespace ribbit{
class AudioDevice{virtual interface()=0;}; // This tricks swig into generating the gpcpp parameter which we can pull the jobject from
%typemap(jtype) AudioDevice * "long"
%typemap(jstype) AudioDevice * "us.toadlet.ribbit.AudioDevice"
%typemap(in) AudioDevice * "$1=($input!=0?*(toadlet::ribbit::AudioDevice**)&$input:new toadlet::ribbit::JAudioDevice(jenv,$input_));"
%typemap(javain) AudioDevice * "us.toadlet.ribbit.AudioDevice.getCPtr($javainput)"
%typemap(javaout) AudioDevice * "{long cobj=$jnicall;return cobj!=0?new us.toadlet.ribbit.AudioDevice(cobj,$owner):null;}"
}
}
using namespace toadlet::ribbit;


// Peeper accessors
namespace toadlet{
namespace peeper{
class RenderDevice{virtual interface()=0;}; // This tricks swig into generating the gpcpp parameter which we can pull the jobject from
%typemap(jtype) RenderDevice * "long"
%typemap(jstype) RenderDevice * "us.toadlet.peeper.RenderDevice"
%typemap(in) RenderDevice * "$1=*(toadlet::peeper::RenderDevice**)&$input;"
%typemap(javain) RenderDevice * "us.toadlet.peeper.RenderDevice.getCPtr($javainput)"
%typemap(javaout) RenderDevice * "{long cobj=$jnicall;return cobj!=0?new us.toadlet.peeper.RenderDevice(cobj,$owner):null;}"
}
}
using namespace toadlet::peeper;


namespace toadlet{
namespace peeper{
class RenderState;
%typemap(jtype) RenderState * "long"
%typemap(jstype) RenderState * "us.toadlet.peeper.RenderState"
%typemap(in) RenderState * "$1=*(toadlet::peeper::RenderState**)&$input;"
%typemap(javain) RenderState * "us.toadlet.peeper.RenderState.getCPtr($javainput)"
%typemap(javaout) RenderState * "{long cobj=$jnicall;return cobj!=0?new us.toadlet.peeper.RenderState(cobj,$owner):null;}"
}
}
using namespace toadlet::peeper;


namespace toadlet{
namespace tadpole{
}
}
using namespace toadlet::tadpole;

%include <toadlet/tadpole/Track.i>
%include <toadlet/tadpole/Sequence.i>
%include <toadlet/tadpole/Engine.i>
%include <toadlet/tadpole/Scene.i>
%include <toadlet/tadpole/Camera.i>
%include <toadlet/tadpole/Component.i>
%include <toadlet/tadpole/Visible.i>
%include <toadlet/tadpole/ActionComponent.i>
%include <toadlet/tadpole/AudioComponent.i>
%include <toadlet/tadpole/CameraComponent.i>
%include <toadlet/tadpole/MeshComponent.i>
%include <toadlet/tadpole/LightComponent.i>
%include <toadlet/tadpole/Node.i>
%include <toadlet/tadpole/Material.i>
%include <toadlet/tadpole/action/Action.i>
%include <toadlet/tadpole/action/ActionListener.i>
%include <toadlet/tadpole/action/AnimationAction.i>
%include <toadlet/tadpole/action/Animation.i>
%include <toadlet/tadpole/action/BaseAnimation.i>
%include <toadlet/tadpole/action/MaterialStateAnimation.i>
%{
using namespace toadlet;
using namespace toadlet::tadpole;
using namespace toadlet::tadpole::action;
using namespace toadlet::tadpole::node;
%}

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
	bool result=false;
	TOADLET_TRY
		result=library->load(name,(char*)NULL,(char*)NULL);
	TOADLET_CATCH_ANONYMOUS(){}
	if(result){
		libraries.add(library);
		RenderDevice *(*creator)()=NULL;
		TOADLET_TRY
			creator=(RenderDevice*(*)())library->getSymbol("new_RenderDevice");
		TOADLET_CATCH_ANONYMOUS(){}
		if(creator!=NULL){
			return creator();
		}
	}
	return NULL;
}
toadlet::ribbit::AudioDevice *new_AudioDevice(char *name){
	DynamicLibrary::ptr library(new DynamicLibrary());
	bool result=false;
	TOADLET_TRY
		result=library->load(name,(char*)NULL,(char*)NULL);
	TOADLET_CATCH_ANONYMOUS(){}
	if(result){
		libraries.add(library);
		AudioDevice *(*creator)()=NULL;
		TOADLET_TRY
			creator=(AudioDevice*(*)())library->getSymbol("new_AudioDevice");
		TOADLET_CATCH_ANONYMOUS(){}
		if(creator!=NULL){
			return creator();
		}
	}
	return NULL;
}
%}
