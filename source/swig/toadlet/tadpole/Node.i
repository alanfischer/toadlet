%{
#include <toadlet/tadpole/node/Node.h>
%}

namespace toadlet{
namespace tadpole{

%refobject Node "$this->retain();"
%unrefobject Node "$this->release();"

class Node{
public:
	Node *create(Scene *scene);
	void destroy();

	Node *getParent();

	void setRotate(float axis[3],float angle);

	void setScope(int scope);
	int getScope();

	bool attach(Node *node);
	bool remove(Node *node);
};

}
}
