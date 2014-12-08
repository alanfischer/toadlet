#ifndef PATH_H
#define PATH_H

#include <toadlet/egg.h>

namespace randisle{

class PathVertex;

class PathEdge:public toadlet::egg::Interface{
public:
	TOADLET_INTERFACE(PathEdge);

	virtual PathVertex *getVertex(bool end) const=0;

	virtual toadlet::egg::math::real getTime(bool end) const=0;
};

class PathVertex:public toadlet::egg::Interface{
public:
	TOADLET_INTERFACE(PathVertex);

	typedef toadlet::egg::IteratorRange<PathEdge::ptr>::iterator iterator;
	virtual toadlet::egg::IteratorRange<PathEdge::ptr> getEdges() const=0;

	virtual toadlet::egg::math::real getLength() const=0;
	virtual void getPoint(toadlet::egg::math::Vector3 &point,toadlet::egg::math::real time) const=0;
	virtual void getOrientation(toadlet::egg::math::Vector3 &tangent,toadlet::egg::math::Vector3 &normal,toadlet::egg::math::Vector3 &scale,toadlet::egg::math::real time) const=0;
};

}

#endif
