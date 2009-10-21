 /********** Copyright header - do not remove **********
 *
 * The Toadlet Engine 
 *
 * Copyright (C) 2005, Lightning Toads Productions, LLC
 *
 * Author(s): Alan Fischer
 * 
 * All source code for the Toadlet Engine, including 
 * this file, is the sole property of Lightning Toads 
 * Productions, LLC. It has been developed on our own 
 * time, with our own tools, on our own hardware. None
 * of this code may be observed, used or changed 
 * without our explicit permission. 
 *
 *  - Lightning Toads Productions, LLC
 *
 ********** Copyright header - do not remove **********/

#ifndef TOADLET_FLICK_DSINCLUDES_H
#define TOADLET_FLICK_DSINCLUDES_H

#include <toadlet/Types.h>
#include <toadlet/egg/Logger.h>

// The DirectShow filter libraries are built release only, and have virtual functions for DEBUG
// So horrible crashes if any form of DEBUG is defined, so we work around it
#undef DEBUG
#undef _DEBUG

#if _MSC_VER>=1100
#define AM_NOVTABLE __declspec(novtable)
#else
#define AM_NOVTABLE
#endif

#include <windows.h>
#include <windowsx.h>
#include <olectl.h>
#include <ddraw.h>
#include <mmsystem.h>

#include <sal.h>
#include <cs.h>
#include <csmedia.h>
#include <amvideo.h>
#include <wxdebug.h>
#include <wxutil.h>
#include <mediaobj.h>
#include <reftime.h>
#include <wxdebug.h>
#include <strmif.h>
#include <combase.h>
#include <wxlist.h>
#include <mtype.h>
#include <amfilter.h>
#include <amextra.h>
#include <control.h>
#include <ctlutil.h>
#include <atlbase.h>
#include <uuids.h>


typedef struct _DevmgrDeviceInformation_tag {
    DWORD dwSize;                   // size of this structure
    HANDLE hDevice;                 // device handle from ActivateDevice()
    HANDLE hParentDevice;           // parent device's handle from ActivateDevice()
    WCHAR szLegacyName[6];          // e.g, "COM1:"
    WCHAR szDeviceKey[MAX_PATH];    // key path passed to ActivateDevice
    WCHAR szDeviceName[MAX_PATH];   // in $device namespace
    WCHAR szBusName[MAX_PATH];      // in $bus namespace
} DEVMGR_DEVICE_INFORMATION, *PDEVMGR_DEVICE_INFORMATION;


typedef enum {
    DeviceSearchByLegacyName, 
    DeviceSearchByDeviceName, 
    DeviceSearchByBusName, 
    DeviceSearchByGuid, 
    DeviceSearchByParent 
} DeviceSearchType;

HANDLE FindFirstDevice(DeviceSearchType searchType, LPCVOID pvSearchParam, PDEVMGR_DEVICE_INFORMATION pdi);


 // Message id from WINWarning.H
//
// MessageId: VFW_S_NO_MORE_ITEMS
//
// MessageText:
//
//  The end of the list has been reached.%0
//
#define VFW_S_NO_MORE_ITEMS              ((HRESULT)0x00040103L)


////////////////////////////////////////////////////////////////////////////////
// Capture Events
////////////////////////////////////////////////////////////////////////////////
#define EC_CAP_FILE_COMPLETED			0x101
#define EC_CAP_FILE_WRITE_ERROR                 0x104
#define EC_CAP_SAMPLE_PROCESSED                 0x102
#define EC_BUFFER_FULL                          0x103


// Flag value to specify the source DIB section has the same rotation angle
// as the destination.
#define BI_SRCPREROTATE    0x8000


#endif
