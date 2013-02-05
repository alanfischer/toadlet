%{
#include <toadlet/tadpole/CameraComponent.h>
%}

namespace toadlet{
namespace tadpole{

class CameraComponent:public Component{
public:
	CameraComponent(Camera *camera);

	void destroy();

	virtual void setName(String name);
	virtual String getName() const;
	
	virtual bool getActive() const;

	Camera *getCamera();
	
	void setLookAt(float eye[3],float point[3],float up[3]);
	void setLookDir(float eye[3],float point[3],float up[3]);
};

}
}
