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

package com.lightningtoads.toadlet.peeper;

#include <com/lightningtoads/toadlet/peeper/Types.h>

public final class Light{
	public enum Type{
		POSITION,
		DIRECTION,
		SPOT,
	}

	public Light(){}

	public void setSpecularColor(Color specularColor){mSpecularColor.set(specularColor);}
	public Color getSpecularColor(){return mSpecularColor;}

	public void setDiffuseColor(Color diffuseColor){mDiffuseColor.set(diffuseColor);}
	public Color getDiffuseColor(){return mDiffuseColor;}

	public void setType(Type type){mType=type;}
	public Type getType(){return mType;}

	public void setLinearAttenuation(scalar attenuation){mLinearAttenuation=attenuation;}
	public scalar getLinearAttenuation(){return mLinearAttenuation;}

	public void setSpotCutoff(scalar cutoff){mSpotCutoff=cutoff;}
	public scalar getSpotCutoff(){return mSpotCutoff;}

	public void setRadius(scalar radius){mRadius=radius;}
	public scalar getRadius(){return mRadius;}

	public void setDirection(Vector3 direction){mDirection.set(direction);}
	public Vector3 getDirection(){return mDirection;}

	public void setPosition(Vector3 position){mPosition.set(position);}
	public Vector3 getPosition(){return mPosition;}

	protected Color mSpecularColor=new Color();
	protected Color mDiffuseColor=new Color();
	protected Type mType=Type.DIRECTION;
	protected scalar mLinearAttenuation=0;
	protected scalar mSpotCutoff=0;
	protected scalar mRadius=0;
	protected Vector3 mPosition=new Vector3();
	protected Vector3 mDirection=new Vector3(0,0,-Math.ONE);
}
