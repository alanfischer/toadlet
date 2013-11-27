#ifndef PATH_H
#define PATH_H

#include <toadlet/toadlet.h>

class PathVertex;

class PathEdge:public Interface{
public:
	TOADLET_INTERFACE(PathEdge);

	virtual PathVertex *getVertex(bool end) const=0;

	virtual scalar getTime(bool end) const=0;
};

class PathVertex:public Interface{
public:
	TOADLET_INTERFACE(PathVertex);

	typedef PointerIteratorRange<PathEdge>::iterator iterator;
	virtual PointerIteratorRange<PathEdge> getEdges() const=0;

	virtual scalar getLength() const=0;
	virtual void getPoint(Vector3 &point,scalar time) const=0;
	virtual void getOrientation(Vector3 &tangent,Vector3 &normal,Vector3 &scale,scalar time) const=0;
};

#endif
