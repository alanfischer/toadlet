#ifndef CSG_BRUSH_H
#define CSG_BRUSH_H

#include <toadlet/egg/Collection.h>
#include <toadlet/egg/String.h>
#include "BrushFace.h"

namespace csg{

class Brush{
public:
	enum CompileResult{
		CR_SUCCESS=0,
		CR_WARNINGS=100,
		CR_WARNING_DUPLICATE_PLANE,
		CR_ERRORS=200,
		CR_ERROR_NO_THICKNESS,
		CR_ERROR_INVALID_NORMAL,
		CR_ERROR_INVALID_POLYGON,
		CR_ERROR_INFINITE_POLYGON,
		CR_ERROR_INSUFFICIENT_FACES,
	};

	Brush();
	~Brush();

	void addFace(BrushFace *face);
	int getNumFaces();
	BrushFace *getFace(int i);
	void deleteFace(int i);

	void setName(const toadlet::egg::String &name){mName=name;}
	const toadlet::egg::String &getName() const{return mName;}

	CompileResult compile();

protected:
	toadlet::egg::Collection<BrushFace*> mFaces;
	toadlet::egg::String mName;
};

}

#endif
