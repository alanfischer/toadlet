%{
#include <toadlet/tadpole/node/MeshNode.h>
%}

namespace toadlet{
namespace tadpole{

class MeshNode:public Node{
public:
	Node *create(Scene *scene);
	void destroy();

	void setMesh(char *name);
};

}
}
