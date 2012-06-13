%{
#include <toadlet/tadpole/node/Node.h>
%}

namespace toadlet{
namespace tadpole{

class Node:public Component{
public:
	Node *create(Scene *scene);
	void destroy();

	Node *getParent();

	void setName(char *name);
//	String getName() const;

	bool getActive() const;

	void setTranslate(float x,float y,float z);
	void setRotate(float axis[3],float angle);

	void setScope(int scope);
	int getScope() const;

	bool attach(Component *node);
	bool remove(Component *node);

	void startAction(char *name);
	void stopAction(char *name);
	bool getActionActive(char *name);
};

}
}
