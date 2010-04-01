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

#ifndef TOADLET_TADPOLE_NODE_LIGHTNODE_H
#define TOADLET_TADPOLE_NODE_LIGHTNODE_H

#include <toadlet/peeper/Light.h>
#include <toadlet/tadpole/node/Node.h>

namespace toadlet{
namespace tadpole{
namespace node{

class Scene;

/// @todo: Add proper world position & direction updating of the Light, same with AudioNode
class TOADLET_API LightNode:public Node{
public:
	TOADLET_NODE(LightNode,Node);

	LightNode();
	virtual Node *create(Scene *scene);
	virtual void destroy();

	inline void setSpecularColor(const peeper::Color &specularColor){mLight->setSpecularColor(specularColor);}
	inline const peeper::Color &getSpecularColor() const{return mLight->getSpecularColor();}

	inline void setDiffuseColor(const peeper::Color &diffuseColor){mLight->setDiffuseColor(diffuseColor);}
	inline const peeper::Color &getDiffuseColor() const{return mLight->getDiffuseColor();}

	inline void setLinearAttenuation(scalar attenuation){mLight->setLinearAttenuation(attenuation);}
	inline scalar getLinearAttenuation() const{return mLight->getLinearAttenuation();}

	inline void setLightType(peeper::Light::Type type){mLight->setType(type);}
	inline peeper::Light::Type getLightType() const{return mLight->getType();}

	inline void setDirection(const Vector3 &direction){mLight->setDirection(direction);}
	inline const Vector3 &getDirection() const{return mLight->getDirection();}

	inline void setSpotCutoff(scalar cutoff){mLight->setSpotCutoff(cutoff);}
	inline scalar getSpotCutoff() const{return mLight->getSpotCutoff();}

	inline void setRadius(scalar radius){mLight->setRadius(radius);}
	inline scalar getRadius() const{return mLight->getRadius();}

	inline peeper::Light *internal_getLight(){return mLight;}

	void renderUpdate(RenderQueue *queue,CameraNode *camera);

protected:
	peeper::Light::ptr mLight;
};

}
}
}

#endif

