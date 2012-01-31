#ifndef PATHSYSTEM_H
#define PATHSYSTEM_H

#include <toadlet/tadpole.h>

class PathSystem{
public:
	class Path{
	public:
		virtual scalar getLength()=0;
		virtual void getPoint(Vector3 &point,scalar time)=0;
		virtual void getOrientation(Vector3 &tangent,Vector3 &normal,Vector3 &scale,scalar time)=0;

		/// @todo: See if this could be replaced with a getNextNeighbor(scalar time,int direction)
		virtual int getNumNeighbors()=0;
		virtual Path *getNeighbor(int i)=0;
		virtual scalar getNeighborTime(int i)=0;
		virtual scalar getNeighborTime(Path *path)=0;
	};

	virtual Path *getClosestPath(Vector3 &closestPoint,const Vector3 &point)=0;
};

#endif
