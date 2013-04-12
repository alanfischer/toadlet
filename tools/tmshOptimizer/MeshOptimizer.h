#ifndef MESHOPTIMIZER_H
#define MESHOPTIMIZER_H

#include <toadlet/tadpole.h>

class MeshOptimizer{
public:
	MeshOptimizer();
	~MeshOptimizer();

	bool optimizeMesh(Mesh *mesh,Engine *engine);

protected:
	float mEpsilon;
};

#endif
