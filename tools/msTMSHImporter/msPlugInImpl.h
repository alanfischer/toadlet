#ifndef __MS_PLUGIN_IMPL_H__
#define __MS_PLUGIN_IMPL_H__

#include "msPlugIn.h"
#include "msLib.h"
#include <toadlet/tadpole/mesh/Mesh.h>

using namespace toadlet::egg;
using namespace toadlet::tadpole;
using namespace toadlet::tadpole::mesh;

struct msModel;
class cPlugIn : public cMsPlugIn
{
    char szTitle[64];
	Matrix3x3 milkshapeToToadlet;
	Matrix3x3 toadletToMilkshape;

	void convertVector3ToMSVec3(const Vector3 &tvec,msVec3 msvec,bool rotate);
	void convertQuaternionToMSVec3(const Quaternion &quat,msVec3 msvec,bool rotate);
	void convertMSVec3ToVector3(const msVec3 &msvec,Vector3 &tvec,bool rotate);
	void convertMSVec3ToQuaternion(const msVec3 &msvec,Quaternion &quat,bool rotate);
	int importMesh(msModel *pModel,const String &name,int flags);
	Skeleton::ptr buildSkeleton(msModel *pModel);
	int importAnimation(msModel *pModel,const String &name,int flags);

public:
	cPlugIn ();
    virtual ~cPlugIn ();

public:
    int             GetType ();
    const char *    GetTitle ();
    int             Execute (msModel* pModel);
};

#endif // __MS_PLUGIN_IMPL_H__