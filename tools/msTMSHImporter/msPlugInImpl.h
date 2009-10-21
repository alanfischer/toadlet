#ifndef __MS_PLUGIN_IMPL_H__
#define __MS_PLUGIN_IMPL_H__



#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif



#include "resource.h"
#include "msPlugIn.h"
#include "msLib.h"
#include <toadlet/tadpole/Mesh.h>

using namespace toadlet::egg;
using namespace toadlet::tadpole;

/////////////////////////////////////////////////////////////////////////////
// CMsPlugInApp
//
//

class CMsPlugInApp : public CWinApp
{
public:
	CMsPlugInApp();

	//{{AFX_VIRTUAL(CMsPlugInApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CMsPlugInApp)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};



/////////////////////////////////////////////////////////////////////////////
// cPlugIn
//
//

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
	MeshSkeleton::ptr buildSkeleton(msModel *pModel);
	int importAnimation(msModel *pModel,const String &name,int flags);

public:
	cPlugIn ();
    virtual ~cPlugIn ();

public:
    int             GetType ();
    const char *    GetTitle ();
    int             Execute (msModel* pModel);
};



/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // __MS_PLUGIN_IMPL_H__