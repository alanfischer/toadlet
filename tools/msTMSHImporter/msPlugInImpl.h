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