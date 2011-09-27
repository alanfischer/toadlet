#ifndef __MS_PLUGIN_IMPL_H__
#define __MS_PLUGIN_IMPL_H__

#include "msPlugIn.h"
#include "msLib.h"
#include <toadlet/egg/io/FileStream.h>
#include <toadlet/tadpole/Mesh.h>
#include <toadlet/tadpole/handler/XMSHHandler.h>
#include <toadlet/tadpole/handler/XANMHandler.h>

using namespace toadlet::egg;
using namespace toadlet::egg::io;
using namespace toadlet::tadpole;
using namespace toadlet::tadpole::handler;

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