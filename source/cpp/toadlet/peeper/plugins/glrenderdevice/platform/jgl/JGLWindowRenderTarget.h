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

#ifndef TOADLET_PEEPER_JGLWINDOWRENDERTARGET_H
#define TOADLET_PEEPER_JGLWINDOWRENDERTARGET_H

#include "../../GLRenderTarget.h"
#include <jni.h>

namespace toadlet{
namespace peeper{

class TOADLET_API JGLWindowRenderTarget:public GLRenderTarget{
public:
	JGLWindowRenderTarget(JNIEnv *env,jobject obj);
	virtual ~JGLWindowRenderTarget();
	void destroy();

	virtual RenderTarget *getRootRenderTarget(){return (GLRenderTarget*)this;}

	bool swap();
	void reset();
	bool activate();
	bool deactivate();
	bool activateAdditionalContext();
	void deactivateAdditionalContext();

	bool isPrimary() const;
	bool isValid() const;
	int getWidth() const;
	int getHeight() const;

protected:
	JNIEnv *getEnv() const;

	JavaVM *vm;
	jobject obj;
	jmethodID swapID,resetID,activateID,deactivateID,activateAdditionalContextID,deactivateAdditionalContextID;
	jmethodID destroyID,isPrimaryID,isValidID,getWidthID,getHeightID;
};

}
}

#endif
