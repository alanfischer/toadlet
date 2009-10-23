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

package com.lightningtoads.toadlet.egg.math;

#include <com/lightningtoads/toadlet/Types.h>

public final class ConvexSolid{
	public ConvexSolid(){}

	public ConvexSolid(ConvexSolid cs){
		int i;
		for(i=0;i<cs.numPlanes;++i){
			addPlane(cs.planes[i]);
		}
	}

	public ConvexSolid set(ConvexSolid cs){
		int i;
		for(i=0;i<cs.numPlanes;++i){
			if(i>=numPlanes){
				addPlane(cs.planes[i]);
			}
			else{
				planes[i].set(cs.planes[i]);
			}
		}
		return this;
	}

	public void addPlane(Plane plane){
                if(planes==null || planes.length<numPlanes+1){
                        Plane[] planes=new Plane[numPlanes+1];
                        if(planes!=null){
                                System.arraycopy(planes,0,planes,0,planes.length);
                        }
                        planes=planes;
                }
                planes[numPlanes++]=plane;
        }

        public void removePlane(Plane plane){
                int i;
                for(i=0;i<numPlanes;++i){
                        if(planes[i]==plane)break;
                }
                if(i!=numPlanes){
                        System.arraycopy(planes,i+1,planes,i,numPlanes-(i+1));
                        planes[--numPlanes]=null;
                }
        }

	public Plane[] planes;
	public int numPlanes=0;
}
