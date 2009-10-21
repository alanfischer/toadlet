%{
#include <toadlet/tadpole/sg/NodeEntity.h>
%}

namespace toadlet{
namespace tadpole{
namespace sg{

class NodeEntity:public Entity{
public:
	NodeEntity(Engine *engine);
	virtual ~NodeEntity();
	virtual void destroy();

	virtual void attach(Entity *entity);
	virtual bool remove(Entity *entity);

	inline int getNumChildren() const;
	inline Entity *getChild(int i);
};

}
}
}
