/********** Copyright header - do not remove **********
 *
 * The Toadlet Engine
 *
 * Copyright 2009, Lightning Toads Productions, LLC
 *
 * Author(s): Alan Fischer, Andrew Fischer
 *
 * This file is part of The Toadlet Engine.
 *
 * The Toadlet Engine is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * The Toadlet Engine is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.

 * You should have received a copy of the GNU Lesser General Public License
 * along with The Toadlet Engine.  If not, see <http://www.gnu.org/licenses/>.
 *
 ********** Copyright header - do not remove **********/

#ifndef POSIXERRORHANDLER_H
#define POSIXERRORHANDLER_H

#include <toadlet/egg/StackTraceListener.h>
#include <signal.h>

namespace toadlet{
namespace egg{

class PosixErrorHandler{
public:
	PosixErrorHandler();
	~PosixErrorHandler();

	virtual void setStackTraceListener(StackTraceListener *listener){mListener=listener;}
	virtual StackTraceListener *getStackTraceListener(){return mListener;}

	virtual bool installHandler();
	virtual bool uninstallHandler();

	virtual void handleFrames(void **frames,int count);
	virtual void errorHandled();

	static PosixErrorHandler *instance;

protected:
	static void signalHandler(int sig,siginfo_t *info,void *context);

	static int mSignals[NSIG];

	StackTraceListener *mListener;

	struct sigaction mAction;
	struct sigaction mOldActions[NSIG];
	
	const static int MAX_STACKFRAMES=128;
	void *mStackFrames[MAX_STACKFRAMES];
};

}
}

#endif

