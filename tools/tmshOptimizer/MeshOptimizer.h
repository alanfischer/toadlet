#ifndef MESHOPTIMIZER_H
#define MESHOPTIMIZER_H

#include <toadlet/toadlet.h>

using namespace toadlet;

class MeshOptimizer{
public:
	MeshOptimizer();
	~MeshOptimizer();

	bool optimizeMesh(Mesh *mesh,Engine *engine);

protected:
	float mEpsilon;
};

#endif
