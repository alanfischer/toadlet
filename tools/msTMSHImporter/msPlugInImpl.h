#ifndef __MS_PLUGIN_IMPL_H__
#define __MS_PLUGIN_IMPL_H__

#include "msPlugIn.h"
#include "msLib.h"
#include <toadlet/egg/io/FileStream.h>
#include <toadlet/tadpole/Mesh.h>
#include <toadlet/tadpole/plugins/XMSHStreamer.h>
#include <toadlet/tadpole/plugins/XANMStreamer.h>

using namespace toadlet::egg;
using namespace toadlet::egg::io;
using namespace toadlet::tadpole;

struct msModel;
class cPlugIn : public cMsPlugIn
{
    char szTitle[64];
	Engine::ptr engine;

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