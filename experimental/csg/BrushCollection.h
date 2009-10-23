#ifndef CSG_BRUSHCOLLECTION_H
#define CSG_BRUSHCOLLECTION_H

#include "Brush.h"
#include <toadlet/tadpole/material/Material.h>

namespace csg{

class BrushCollection{
public:
	toadlet::egg::Collection<Brush*> brushes;
	toadlet::egg::Collection<toadlet::tadpole::material::Material::Ptr> materials;
};

}

#endif
