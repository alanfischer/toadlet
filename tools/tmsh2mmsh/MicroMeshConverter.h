#include <toadlet/egg/math/Math.h>
#include <toadlet/egg/io/OutputStream.h>
#include <toadlet/tadpole/mesh/Mesh.h>

extern "C"{
	#include <stdio.h>
	#include "../shared/actc/tc.h"
}

class MicroMeshConverter{
public:
	enum{
		MESH_BLOCK=		1,
		SKELETON_BLOCK=	2,
		ANIMATION_BLOCK=3,
	};

	enum{
		VT_POSITION=	1<<0,
		VT_NORMAL=		1<<1,
		VT_COLOR=		1<<2,
		VT_BONE=		1<<3,
		VT_TEXCOORD1=	1<<8,
	};

	MicroMeshConverter();
	~MicroMeshConverter();

	bool convertMesh(toadlet::tadpole::mesh::Mesh *mesh,toadlet::egg::io::OutputStream *outStream,float meshScale,int forceBytes,bool invertYTexCoord);

protected:
	ACTCData *mTC;
};
