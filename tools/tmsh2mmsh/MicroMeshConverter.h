#include <toadlet/toadlet.h>
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

	bool convertMesh(Mesh *mesh,Stream *outStream,float meshScale,int forceBytes,bool invertYTexCoord);

protected:
	ACTCData *mTC;
};
