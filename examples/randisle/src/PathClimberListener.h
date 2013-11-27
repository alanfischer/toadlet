#ifndef PATHCLIMBERLISTENER_H
#define PATHCLIMBERLISTENER_H

class PathClimber;
class PathVertex;

class PathClimberListener{
public:
	virtual void pathMounted(PathClimber *climber)=0;
	virtual void pathDismounted(PathClimber *climber)=0;
	virtual int atJunction(PathClimber *climber,PathVertex *current,PathVertex *neighbor)=0;
};

#endif
