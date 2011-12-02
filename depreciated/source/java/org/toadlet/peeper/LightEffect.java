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

package org.toadlet.peeper;

#include <org/toadlet/peeper/Types.h>

public final class LightEffect{
	public LightEffect(){}

	public LightEffect(LightEffect lightEffect){
		set(lightEffect);
	}

	public LightEffect(Color color){
		set(color);
	}

	public LightEffect(boolean trackColor){
		this.trackColor=trackColor;
	}
	
	public LightEffect set(LightEffect lightEffect){
		ambient.set(lightEffect.ambient);
		diffuse.set(lightEffect.diffuse);
		specular.set(lightEffect.specular);
		shininess=lightEffect.shininess;
		emissive.set(lightEffect.emissive);
		trackColor=lightEffect.trackColor;
		return this;
	}

	public LightEffect set(Color color){
		ambient.set(color);
		diffuse.set(color);
		specular.set(color);
		return this;
	}
	
	public boolean equals(Object object){
		if(this==object){
			return true;
		}
		if((object==null) || (object.getClass()!=getClass())){
			return false;
		}
		LightEffect lightEffect=(LightEffect)object;
		return ambient.equals(lightEffect.ambient) && diffuse.equals(lightEffect.diffuse) && specular.equals(lightEffect.specular) && shininess==lightEffect.shininess && emissive.equals(lightEffect.emissive) && trackColor==lightEffect.trackColor;
	}

	public Color ambient=new Color(Colors.WHITE);
	public Color diffuse=new Color(Colors.WHITE);
	public Color specular=new Color(Colors.WHITE);
	public scalar shininess=Math.ONE;
	public Color emissive=new Color(Colors.BLACK);
	public boolean trackColor=false;
}
