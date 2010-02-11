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

package org.toadlet.tadpole;

#include <org/toadlet/tadpole/Types.h>

import org.toadlet.egg.Resource;
import java.util.Vector;

public class MeshSkeleton implements Resource{
	public class Bone{
		public Bone(){}

		public int index=-1;
		public int parentIndex=-1;

		public Vector3 translate=new Vector3();
		public Quaternion rotate=new Quaternion();
		public Vector3 scale=new Vector3(Math.ONE,Math.ONE,Math.ONE); // TODO: Implement bone scaling

		public Vector3 worldToBoneTranslate=new Vector3();
		public Quaternion worldToBoneRotate=new Quaternion();

		public String name;
	}

	public MeshSkeleton(){}

	public void compile(){}

	public String name;

	public Bone[] bones;
	public Vector<MeshSkeletonSequence> sequences;
}
