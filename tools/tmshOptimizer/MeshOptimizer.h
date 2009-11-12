#ifndef MESHOPTIMIZER_H
#define MESHOPTIMIZER_H

#include <toadlet/egg/math/Math.h>
#include <toadlet/tadpole/Engine.h>

class MeshOptimizer{
public:
	MeshOptimizer();
	~MeshOptimizer();

	bool optimizeMesh(toadlet::tadpole::mesh::Mesh *mesh,toadlet::tadpole::Engine *engine);

protected:
	float mEpsilon;
};

#endif
