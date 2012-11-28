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
	Engine(JNIEnv *env,void *context);

	void destroy();

	void installHandlers();
	
	bool isBackable() const;
	bool hasShader(int type) const;
	bool hasFixed(int type) const;
	void setHasBackableShader(bool has);
	void setHasBackableFixed(bool has);
	void setHasMaximumShader(bool has);
	void setHasMaximumFixed(bool has);
	
	bool setRenderDevice(RenderDevice *renderDevice);

	bool setAudioDevice(AudioDevice * audioDevice);
};

}
}
