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

#ifndef TOADLET_TADPOLE_ENTITY_LIGHTENTITY_H
#define TOADLET_TADPOLE_ENTITY_LIGHTENTITY_H

#include <toadlet/tadpole/entity/Entity.h>
#include <toadlet/peeper/Light.h>

namespace toadlet{
namespace tadpole{
namespace entity{

class Scene;

/// @todo: Add proper world position & direction updating of the Light, same with AudioEntity
class TOADLET_API LightEntity:public Entity{
public:
	TOADLET_ENTITY(LightEntity,Entity);

	LightEntity();
	virtual Entity *create(Engine *engine);
	virtual void destroy();

	virtual bool isLight() const{return true;}

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

protected:
	peeper::Light::ptr mLight;
};

}
}
}

#endif

