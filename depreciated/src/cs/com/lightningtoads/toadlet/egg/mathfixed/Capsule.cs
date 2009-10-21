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
    public sealed class Capsule
    {
        public Capsule()
        {
            origin = new Vector3();
            direction = new Vector3();
        }

        public Capsule(Capsule capsule)
        {
            origin = new Vector3(capsule.origin);
            direction = new Vector3(capsule.direction);
            radius = capsule.radius;
        }

        public Capsule(Vector3 origin1, Vector3 direction1, int radius1)
        {
            origin = new Vector3(origin1);
            direction = new Vector3(direction1);
            radius = radius1;
        }

        public void set(Capsule capsule)
        {
            origin.x = capsule.origin.x;
            origin.y = capsule.origin.y;
            origin.z = capsule.origin.z;

            direction.x = capsule.direction.x;
            direction.y = capsule.direction.y;
            direction.z = capsule.direction.z;

            radius = capsule.radius;
        }

        public void set(Vector3 origin1, Vector3 direction1, int radius1)
        {
            origin.x = origin1.x;
            origin.y = origin1.y;
            origin.z = origin1.z;

            direction.x = direction1.x;
            direction.y = direction1.y;
            direction.z = direction1.z;

            radius = radius1;
        }

        public void reset()
        {
            origin.x = 0;
            origin.y = 0;
            origin.z = 0;

            direction.x = 0;
            direction.y = 0;
            direction.z = 0;

            radius = 0;
        }

        public void setOrigin(Vector3 origin1)
        {
            origin.x = origin1.x;
            origin.y = origin1.y;
            origin.z = origin1.z;
        }

        public void setOrigin(int x, int y, int z)
        {
            origin.x = x;
            origin.y = y;
            origin.z = z;
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

        public void setDirection(int x, int y, int z)
        {
            direction.x = x;
            direction.y = y;
            direction.z = z;
        }

        public Vector3 getDirection()
        {
            return direction;
        }

        public void setRadius(int radius1)
        {
            radius = radius1;
        }

        public int getRadius()
        {
            return radius;
        }

        public void setLength(int length)
        {
            if (length >= (radius << 1))
            {
                if (direction.x == 0 && direction.y == 0 && direction.z == 0)
                {
                    direction.z = (length - (radius << 1));
                }
                else
                {
                    int s = Math.div(length - (radius << 1), Math.length(direction));
                    Math.mul(direction, s);
                }
            }
            else
            {
                direction.x = 0;
                direction.y = 0;
                direction.z = 0;
            }
        }

        public int getLength()
        {
            return Math.length(direction) + (radius << 1);
        }

        public void rotate(Matrix3x3 rotation)
        {
            Math.mul(direction, rotation);
        }

        public Vector3 origin;
        public Vector3 direction;
        public int radius;
    }
}