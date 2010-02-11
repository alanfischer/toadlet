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

package org.toadlet.egg.mathfixed;

#include <org/toadlet/Types.h>

public final class AABox{
	public AABox(){
		mins=new Vector3();
		maxs=new Vector3();
	}

	public AABox(AABox box){
		mins=new Vector3(box.mins);
		maxs=new Vector3(box.maxs);
	}

	public AABox(Vector3 mins1,Vector3 maxs1){
		mins=new Vector3(mins1);
		maxs=new Vector3(maxs1);
	}

	public AABox(fixed minx,fixed miny,fixed minz,fixed maxx,fixed maxy,fixed maxz){
		mins=new Vector3(minx,miny,minz);
		maxs=new Vector3(maxx,maxy,maxz);
	}

	public AABox(fixed radius){
		mins=new Vector3(-radius,-radius,-radius);
		maxs=new Vector3(radius,radius,radius);
	}

	public AABox set(AABox box){
		mins.x=box.mins.x;
		mins.y=box.mins.y;
		mins.z=box.mins.z;

		maxs.x=box.maxs.x;
		maxs.y=box.maxs.y;
		maxs.z=box.maxs.z;

		return this;
	}

	public AABox set(Vector3 mins1,Vector3 maxs1){
		mins.x=mins1.x;
		mins.y=mins1.y;
		mins.z=mins1.z;

		maxs.x=maxs1.x;
		maxs.y=maxs1.y;
		maxs.z=maxs1.z;

		return this;
	}

	public AABox set(fixed minx,fixed miny,fixed minz,fixed maxx,fixed maxy,fixed maxz){
		mins.x=minx;
		mins.y=miny;
		mins.z=minz;

		maxs.x=maxx;
		maxs.y=maxy;
		maxs.z=maxz;

		return this;
	}

	public AABox set(fixed radius){
		mins.x=-radius;
		mins.y=-radius;
		mins.z=-radius;

		maxs.x=radius;
		maxs.y=radius;
		maxs.z=radius;

		return this;
	}

	public AABox reset(){
		mins.x=0;
		mins.y=0;
		mins.z=0;

		maxs.x=0;
		maxs.y=0;
		maxs.z=0;

		return this;
	}

	public void setMins(Vector3 mins1){
		mins.x=mins1.x;
		mins.y=mins1.y;
		mins.z=mins1.z;
	}

	public void setMins(fixed x,fixed y,fixed z){
		mins.x=x;
		mins.y=y;
		mins.z=z;
	}

	public Vector3 getMins(){
		return mins;
	}

	public void setMaxs(Vector3 maxs1){
		maxs.x=maxs1.x;
		maxs.y=maxs1.y;
		maxs.z=maxs1.z;
	}

	public void setMaxs(fixed x,fixed y,fixed z){
		maxs.x=x;
		maxs.y=y;
		maxs.z=z;
	}

	public Vector3 getMaxs(){
		return maxs;
	}

	public void getVertexes(Vector3[] vertexes){
		vertexes[0].x=mins.x;vertexes[0].y=mins.y;vertexes[0].z=mins.z;
		vertexes[1].x=maxs.x;vertexes[1].y=mins.y;vertexes[1].z=mins.z;
		vertexes[2].x=mins.x;vertexes[2].y=maxs.y;vertexes[2].z=mins.z;
		vertexes[3].x=maxs.x;vertexes[3].y=maxs.y;vertexes[3].z=mins.z;
		vertexes[4].x=mins.x;vertexes[4].y=mins.y;vertexes[4].z=maxs.z;
		vertexes[5].x=maxs.x;vertexes[5].y=mins.y;vertexes[5].z=maxs.z;
		vertexes[6].x=mins.x;vertexes[6].y=maxs.y;vertexes[6].z=maxs.z;
		vertexes[7].x=maxs.x;vertexes[7].y=maxs.y;vertexes[7].z=maxs.z;
	}

	public void findPVertex(Vector3 r,Vector3 normal){
		r.set(mins);
		if(normal.x>=0)
			r.x=maxs.x;
		if(normal.y>=0)
			r.y=maxs.y;
		if(normal.z>=0)
			r.z=maxs.z;
	}

	public void findNVertex(Vector3 r,Vector3 normal){
		r.set(maxs);
		if(normal.x>=0)
			r.x=mins.x;
		if(normal.y>=0)
			r.y=mins.y;
		if(normal.z>=0)
			r.z=mins.z;
	}

	public void mergeWith(AABox box){
		if(box.mins.x<mins.x)mins.x=box.mins.x;
		if(box.mins.y<mins.y)mins.y=box.mins.y;
		if(box.mins.z<mins.z)mins.z=box.mins.z;
		if(box.maxs.x>maxs.x)maxs.x=box.maxs.x;
		if(box.maxs.y>maxs.y)maxs.y=box.maxs.y;
		if(box.maxs.z>maxs.z)maxs.z=box.maxs.z;
	}

	public void mergeWith(Vector3 vec){
		if(vec.x<mins.x)mins.x=vec.x;
		if(vec.y<mins.y)mins.y=vec.y;
		if(vec.z<mins.z)mins.z=vec.z;
		if(vec.x>maxs.x)maxs.x=vec.x;
		if(vec.y>maxs.y)maxs.y=vec.y;
		if(vec.z>maxs.z)maxs.z=vec.z;
	}

	public boolean equals(Object object){
		if(this==object){
			return true;
		}
		if((object==null) || (object.getClass()!=getClass())){
			return false;
		}
		AABox box=(AABox)object;
		return mins.equals(box.mins) && maxs.equals(box.maxs);
	}

	public int hashCode(){
		return mins.hashCode() + maxs.hashCode();
	}

	public Vector3 mins;
	public Vector3 maxs;
}
