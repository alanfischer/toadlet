/********** Copyright header - do not remove **********
 *
 * The Toadlet Engine
 *
 * Copyright (C) 2006, Lightning Toads Productions, LLC
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

#ifndef TOADLET_EGG_NET_BCSERVERENTRY_H
#define TOADLET_EGG_NET_BCSERVERENTRY_H

#include <toadlet/egg/net/BluetoothServerEntry.h>
#include <toadlet/egg/net/plugins/broadcom/BCIncludes.h>

namespace toadlet{
namespace egg{
namespace net{

class BCServerEntry:public BluetoothServerEntry{
public:
	BCServerEntry(const String &name,const GUID &serviceGUID,const String &serviceName,BD_ADDR address){
		this->name=name;
		this->serviceGUID=serviceGUID;
		this->serviceName=serviceName;
		memcpy(this->address,address,sizeof(BD_ADDR));
	}

	BCServerEntry(){}

	const String &getName(){return name;}

	String name;
	GUID serviceGUID;
	String serviceName;
	BD_ADDR address;
};

}
}
}

#endif
