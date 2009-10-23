#ifndef CSG_MAPHANDLER_H
#define CSG_MAPHANDLER_H

#include <toadlet/egg/Collection.h>
#include <toadlet/egg/String.h>
#include <toadlet/egg/io/InputStream.h>
#include <toadlet/egg/io/OutputStream.h>
#include "Map.h"

namespace csg{

class MAPHandler{
public:
	MAPHandler(){}
	~MAPHandler(){}

	Map *load(toadlet::egg::io::InputStream *in,const toadlet::egg::String &name);
	void save(Map *map,toadlet::egg::io::OutputStream *out,const toadlet::egg::String &name,bool invertYTexCoord);
};

}

#endif
