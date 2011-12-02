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
import org.toadlet.peeper.IndexData;
import org.toadlet.peeper.VertexData;
import java.util.Vector;

public class Mesh implements Resource{
	public static class SubMesh{
		public SubMesh(){}

		public IndexData indexData;

		public Material material;
		public String materialName;

		public String name;
	};

	public static class VertexBoneAssignment{
		public VertexBoneAssignment(short b,scalar w){
			bone=b;
			weight=w;
		}

		public VertexBoneAssignment(){}

		public short bone=0;
		public scalar weight=Math.ONE;
	};

	public Mesh(){
	}

	public void compile(){}

	public String name;

	public scalar boundingRadius=-Math.ONE; // Before world scale
	public scalar worldScale=Math.ONE;

	public SubMesh[] subMeshes;
	public VertexData staticVertexData;

	public MeshSkeleton skeleton;
	public VertexBoneAssignment[][] vertexBoneAssignments;
}
