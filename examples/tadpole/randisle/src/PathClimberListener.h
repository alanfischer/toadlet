#ifndef PATHCLIMBERLISTENER_H
#define PATHCLIMBERLISTENER_H

class PathClimber;
class Path;

class PathClimberListener{
public:
	virtual void pathMounted(PathClimber *climber)=0;
	virtual void pathDismounted(PathClimber *climber)=0;
	virtual int atJunction(PathClimber *climber,Path *current,Path *neighbor)=0;
};

#endif
