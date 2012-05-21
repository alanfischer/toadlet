%{
#	include <toadlet/tadpole/Engine.h>
%}

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

%typemap(in, numinputs=0) JNIEnv *"$1=jenv;"

%typemap(jni) void* "jobject"
%typemap(jtype) void* "java.lang.Object"
%typemap(jstype) void* "java.lang.Object"
%typemap(javain) void* "$javainput"

namespace toadlet{
namespace tadpole{

%refobject Engine "$this->retain();"
%unrefobject Engine "$this->release();"

class Engine{
public:
	Engine(JNIEnv *env,void *context);

	void destroy();

	void installHandlers();
	
	bool setRenderDevice(RenderDevice *renderDevice);

	bool setAudioDevice(AudioDevice * audioDevice);

	toadlet::tadpole::Node *createNode(char *fullName,toadlet::tadpole::Scene *scene);
};

}
}
