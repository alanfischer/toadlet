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
	HWND hwndProgress;
	Engine::ptr engine;

	class Frame{
	public:
		Frame(){
			translateSet=false;
			rotateSet=false;
			time=-1;
		}

		Vector3 translate;
		bool translateSet;
		Quaternion rotate;
		bool rotateSet;
		float time;
	};

	Skeleton::ptr buildSkeleton(msModel *pModel,const Collection<int> &emptyBones);
	int exportMesh(msModel *pModel,const String &sname);
	int exportAnimation(msModel *pModel,const String &sname);
	void findEmptyBones(msModel *pModel,Collection<int> &emptyBones);
	void progressUpdated(float amount);

public:
	cPlugIn ();
    virtual ~cPlugIn ();

public:
    int             GetType ();
    const char *    GetTitle ();
    int             Execute (msModel* pModel);
};

#endif // __MS_PLUGIN_IMPL_H__