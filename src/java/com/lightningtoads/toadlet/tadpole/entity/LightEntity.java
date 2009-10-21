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

package com.lightningtoads.toadlet.tadpole.entity;

#include <com/lightningtoads/toadlet/tadpole/Types.h>

import com.lightningtoads.toadlet.peeper.*;
import com.lightningtoads.toadlet.tadpole.*;

public class LightEntity extends Entity{
	public LightEntity(){super();}

	public Entity create(Engine engine){
		super.create(engine);

		mLight=new Light();
		
		return this;
	}

	public void destroy(){
		mLight=null;

		super.destroy();
	}

	public boolean isLight(){return true;}

	public void setSpecularColor(Color specularColor){mLight.setSpecularColor(specularColor);}
	public Color getSpecularColor(){return mLight.getSpecularColor();}

	public void setDiffuseColor(Color diffuseColor){mLight.setDiffuseColor(diffuseColor);}
	public Color getDiffuseColor(){return mLight.getDiffuseColor();}

	public void setLinearAttenuation(scalar attenuation){mLight.setLinearAttenuation(attenuation);}
	public scalar getLinearAttenuation(){return mLight.getLinearAttenuation();}

	public void setLightType(Light.Type type){mLight.setType(type);}
	public Light.Type getLightType(){return mLight.getType();}

	public void setDirection(Vector3 direction){mLight.setDirection(direction);}
	public Vector3 getDirection(){return mLight.getDirection();}

	public void setSpotCutoff(scalar cutoff){mLight.setSpotCutoff(cutoff);}
	public scalar getSpotCutoff(){return mLight.getSpotCutoff();}

	public void setRadius(scalar radius){mLight.setRadius(radius);}
	public scalar getRadius(){return mLight.getRadius();}

	public Light internal_getLight(){return mLight;}

	Light mLight;
}
