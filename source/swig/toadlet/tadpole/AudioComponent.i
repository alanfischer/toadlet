%{
#include <toadlet/tadpole/AudioComponent.h>
%}

namespace toadlet{
namespace tadpole{

class AudioComponent:public Component{
public:
	AudioComponent(Engine *engine);
	void destroy();

	void setName(String name);
	String getName() const;

	bool setAudioBuffer(char *name);
	
	void play();
};

}
}
