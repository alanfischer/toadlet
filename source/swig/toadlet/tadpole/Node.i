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

	void setRotate(float axis[3],float angle);

	void setScope(int scope);
	int getScope();

	bool attach(Component *node);
	bool remove(Component *node);
};

}
}
