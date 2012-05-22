%{
#include <toadlet/tadpole/node/LightNode.h>
%}

namespace toadlet{
namespace tadpole{

class LightNode:public Node{
public:
	Node *create(Scene *scene);
	void destroy();

	void setDirection(float direction[3]);
};

}
}
