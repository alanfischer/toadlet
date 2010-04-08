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

#ifndef TOADLET_TADPOLE_NODE_CAMERAALIGNEDNODE_H
#define TOADLET_TADPOLE_NODE_CAMERAALIGNEDNODE_H

#include <toadlet/tadpole/node/Node.h>

namespace toadlet{
namespace tadpole{
namespace node{

// This class is basically a work around to have some nodes always face the camera, and be able to toggle their perspectiveness.
//  Ideally this would all be handled in a vertex shader, since it's almost more of a material property than anythign else, but
//	since changing the perspectiveness affects the bounding volume, we have a separate class for this.
// This class needs to be modified to be able to handle multiple rendering threads at once, with multiple cameras.

class TOADLET_API CameraAlignedNode:public Node{
public:
	TOADLET_NODE(CameraAlignedNode,Node);
	
	CameraAlignedNode();

	virtual void setCameraAligned(bool aligned){mCameraAligned=aligned;}
	inline bool getCameraAligned() const{return mCameraAligned;}

	virtual void setPerspective(bool perspective){mPerspective=perspective;}
	inline bool getPerspective() const{return mPerspective;}

	virtual void frameUpdate(int dt,int scope);
	virtual void queueRenderables(CameraNode *camera,RenderQueue *queue);

protected:
	bool mCameraAligned;
	bool mPerspective;
};

}
}
}

#endif
