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
	public ConvexSolid(){};

	public ConvexSolid(ConvexSolid cs){
		planes=cs.planes;
	}

	public ConvexSolid set(ConvexSolid cs){
		planes=cs.planes;
	}

	public void addPlane(Plane plane){
                if(mPlanes==null || mPlanes.length<mNumPlanes+1){
                        Plane[] planes=new Plane[mNumPlanes+1];
                        if(mPlanes!=null){
                                System.arraycopy(mPlanes,0,planes,0,mPlanes.length);
                        }
                        mPlanes=planes;
                }
                mPlanes[mNumPlanes++]=plane;
        }

        public void removePlane(Plane plane){
                mActive=true;
                mDeactivateCount=0;

                int i;
                for(i=0;i<mNumPlanes;++i){
                        if(mPlanes[i]==plane)break;
                }
                if(i!=mNumPlanes){
                        System.arraycopy(mPlanes,i+1,mPlanes,i,mNumPlanes-(i+1));
                        mPlanes[--mNumPlanes]=null;
                }
        }

	public Plane[] planes;
	public int numPlanes=0;
}
