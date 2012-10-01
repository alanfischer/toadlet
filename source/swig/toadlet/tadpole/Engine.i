%{
#include <toadlet/tadpole/Engine.h>
%}

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
	Engine(JNIEnv *env,void *context,int options);

	static const int Option_BIT_FIXEDBACKABLE=	1<<0;
	static const int Option_BIT_SHADERBACKABLE=	1<<1;
	static const int Option_BIT_NOFIXED=		1<<2;
	static const int Option_BIT_NOSHADER=		1<<3;

	void destroy();

	void installHandlers();
	
	bool setRenderDevice(RenderDevice *renderDevice);

	bool setAudioDevice(AudioDevice * audioDevice);
};

}
}
