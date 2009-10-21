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

//using com.lightningtoads.toadlet.egg.mathfixed.Math;

namespace com.lightningtoads.toadlet.egg.mathfixed
{
    public sealed class Segment
    {
        public Segment()
        {
            origin = new Vector3();
            direction = new Vector3();
        }

        public void set(Segment segment)
        {
            origin.x = segment.origin.x;
            origin.y = segment.origin.y;
            origin.z = segment.origin.z;

            direction.x = segment.direction.x;
            direction.y = segment.direction.y;
            direction.z = segment.direction.z;
        }

        public void setStartEnd(Vector3 start, Vector3 end)
        {
            origin.x = start.x;
            origin.y = start.y;
            origin.z = start.z;

            direction.x = end.x - start.x;
            direction.y = end.y - start.y;
            direction.z = end.z - start.z;
        }

        public void setStartDir(Vector3 start, Vector3 dir)
        {
            origin.x = start.x;
            origin.y = start.y;
            origin.z = start.z;

            direction.x = dir.x;
            direction.y = dir.y;
            direction.z = dir.z;
        }

        public void reset()
        {
            origin.x = 0;
            origin.y = 0;
            origin.z = 0;

            direction.x = 0;
            direction.y = 0;
            direction.z = 0;
        }

        public void setOrigin(Vector3 origin1)
        {
            origin.x = origin1.x;
            origin.y = origin1.y;
            origin.z = origin1.z;
        }

        public Vector3 getOrigin()
        {
            return origin;
        }

        public void setDirection(Vector3 direction1)
        {
            direction.x = direction1.x;
            direction.y = direction1.y;
            direction.z = direction1.z;
        }

        public Vector3 getDirection()
        {
            return direction;
        }

        public void getEndPoint(Vector3 endPoint)
        {
            endPoint.x = origin.x + direction.x;
            endPoint.y = origin.y + direction.y;
            endPoint.z = origin.z + direction.z;
        }

        public Vector3 origin;
        public Vector3 direction;
    }
}