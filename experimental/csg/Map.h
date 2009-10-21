#ifndef CSG_MAP_H
#define CSG_MAP_H

#include "BrushCollection.h"

namespace csg{

class MapEntity{
public:
	toadlet::egg::Collection<std::pair<toadlet::egg::String,toadlet::egg::String> > keyValues;
	BrushCollection brushCollection;
};

class Map{
public:
	toadlet::egg::Collection<MapEntity> mapEntities;
};

}

#endif
