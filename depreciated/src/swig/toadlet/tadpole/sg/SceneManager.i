%{
#include <toadlet/tadpole/sg/SceneManager.h>
%}

namespace toadlet{
namespace tadpole{
namespace sg{

class SceneManager:public NodeEntity{
public:
	SceneManager(Engine *engine);
	virtual ~SceneManager();
	virtual void destroy();

	NodeEntity *getBackgroundNode() const;

	virtual void setFixedDT(int dt);
	inline int getFixedDT() const;
	inline int getAccumulatedDT() const;
	inline int getTime() const;
	inline int getImmediateTime() const;

	virtual void update(int dt);

	virtual void render(toadlet::peeper::Renderer *renderer,toadlet::tadpole::sg::CameraEntity *cameraEntity,bool beginEndSceneAndContextUpdate=true);
};

}
}
}
