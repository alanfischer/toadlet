#ifndef PATH_H
#define PATH_H

#include <toadlet/tadpole.h>

using namespace toadlet;

class Path{
public:
	virtual scalar getLength() const=0;
	virtual void getPoint(Vector3 &point,scalar time) const=0;
	virtual void getOrientation(Vector3 &tangent,Vector3 &normal,Vector3 &scale,scalar time) const=0;

	virtual int getNumNeighbors() const=0;
	virtual Path *getNeighbor(int i) const=0;
	virtual scalar getNeighborTime(int i) const=0;
	virtual scalar getNeighborTime(Path *path) const=0;
};

#endif
