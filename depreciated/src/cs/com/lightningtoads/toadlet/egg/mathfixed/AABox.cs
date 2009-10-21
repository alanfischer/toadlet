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

/***********  c# Conversion from Java ************
 * 
 * package changed to namespace
 * Change class type from final to sealed
 *
 * ***********************************************/

// need to use something other than "fixed" since it's a c# keyword
// unfortunately, this is only vision to this file so needs to be defined
// for each file using it
using tfixed = System.Int32;

namespace com.lightningtoads.toadlet.egg.mathfixed
{
    public sealed class AABox
    {
        public AABox()
        {
            mins = new Vector3();
            maxs = new Vector3();
        }

        public AABox(AABox box)
        {
            mins = new Vector3(box.mins);
            maxs = new Vector3(box.maxs);
        }

        public AABox(Vector3 mins1, Vector3 maxs1)
        {
            mins = new Vector3(mins1);
            maxs = new Vector3(maxs1);
        }

        public AABox(int x1, int y1, int z1, int x2, int y2, int z2)
        {
            mins = new Vector3(x1, y1, z1);
            maxs = new Vector3(x2, y2, z2);
        }

        public AABox(int radius)
        {
            mins = new Vector3(-radius, -radius, -radius);
            maxs = new Vector3(radius, radius, radius);
        }

        public void set(AABox box)
        {
            mins.x = box.mins.x;
            mins.y = box.mins.y;
            mins.z = box.mins.z;

            maxs.x = box.maxs.x;
            maxs.y = box.maxs.y;
            maxs.z = box.maxs.z;
        }

        public void set(Vector3 mins1, Vector3 maxs1)
        {
            mins.x = mins1.x;
            mins.y = mins1.y;
            mins.z = mins1.z;

            maxs.x = maxs1.x;
            maxs.y = maxs1.y;
            maxs.z = maxs1.z;
        }

        public void set(int minx, int miny, int minz, int maxx, int maxy, int maxz)
        {
            mins.x = minx;
            mins.y = miny;
            mins.z = minz;

            maxs.x = maxx;
            maxs.y = maxy;
            maxs.z = maxz;
        }

        public void set(int radius)
        {
            mins.x = -radius;
            mins.y = -radius;
            mins.z = -radius;

            maxs.x = radius;
            maxs.y = radius;
            maxs.z = radius;
        }

        public void reset()
        {
            mins.x = 0;
            mins.y = 0;
            mins.z = 0;

            maxs.x = 0;
            maxs.y = 0;
            maxs.z = 0;
        }

        public void setMins(Vector3 mins1)
        {
            mins.x = mins1.x;
            mins.y = mins1.y;
            mins.z = mins1.z;
        }

        public Vector3 getMins()
        {
            return mins;
        }

        public void setMaxs(Vector3 maxs1)
        {
            maxs.x = maxs1.x;
            maxs.y = maxs1.y;
            maxs.z = maxs1.z;
        }

        public Vector3 getMaxs()
        {
            return maxs;
        }

        public void mergeWith(AABox box)
        {
            if (box.mins.x < mins.x) mins.x = box.mins.x;
            if (box.mins.y < mins.y) mins.y = box.mins.y;
            if (box.mins.z < mins.z) mins.z = box.mins.z;
            if (box.maxs.x > maxs.x) maxs.x = box.maxs.x;
            if (box.maxs.y > maxs.y) maxs.y = box.maxs.y;
            if (box.maxs.z > maxs.z) maxs.z = box.maxs.z;
        }

        public void mergeWith(Vector3 vec)
        {
            if (vec.x < mins.x) mins.x = vec.x;
            if (vec.y < mins.y) mins.y = vec.y;
            if (vec.z < mins.z) mins.z = vec.z;
            if (vec.x > maxs.x) maxs.x = vec.x;
            if (vec.y > maxs.y) maxs.y = vec.y;
            if (vec.z > maxs.z) maxs.z = vec.z;
        }

        public Vector3 mins;
        public Vector3 maxs;
    }
}