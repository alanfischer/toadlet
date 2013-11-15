#ifndef PATH_H
#define PATH_H

#include <toadlet/toadlet.h>

class Path:public Interface{
public:
	virtual scalar getLength() const=0;
	virtual void getPoint(Vector3 &point,scalar time) const=0;
	virtual void getOrientation(Vector3 &tangent,Vector3 &normal,Vector3 &scale,scalar time) const=0;

	typedef Pair<Path::ptr,scalar> Neighbor;
	typedef IteratorRange<Neighbor> NeighborRange;
	virtual NeighborRange getNeighbors() const=0;
};

#endif
