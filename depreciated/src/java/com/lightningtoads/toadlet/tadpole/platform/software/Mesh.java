/********** Copyright header - do not remove **********
 *
 * The Toadlet Engine
 *
 * Copyright (C) 2006, Lightning Toads Productions, LLC
 *
 * Author(s): Alan Fischer
 *
 * All source code for the Toadlet Engine, including
 * this file, is the sole property of Lightning Toads
 * Productions, LLC. It has been developed on our own
 * time, with our own tools, on our own hardware. None
 * of this code may be observed, used or changed
 * without our explicit permission.
 *
 *  - Lightning Toads Productions, LLC
 *
 ********** Copyright header - do not remove **********/

package com.lightningtoads.toadlet.tadpole;

import com.lightningtoads.toadlet.egg.mathfixed.*;
import com.lightningtoads.toadlet.egg.mathfixed.Math;

public final class Mesh{
	public final static class SubMesh{
		// FaceCullings
		public final static byte FC_NONE=0;
		public final static byte FC_CCW=1;
		public final static byte FC_CW=2;
		public final static byte FC_FRONT=1;
		public final static byte FC_BACK=2;

		public SubMesh(){}

		public byte faceCulling=FC_NONE;
		public boolean hasTexture=false;
		public int textureIndex=0;
		public boolean lightingEnabled=true;
		public int color=0xFFFFFFFF;
		public boolean sort=true;
	}

	public final static class Bone{
		public Bone(){}

		public byte index=-1;
		public byte parent=-1;
		public byte[] children;

		public Vector3 translate=new Vector3();
		public Quaternion rotate=new Quaternion();

		public Vector3 worldToBoneTranslate=new Vector3();
		public Quaternion worldToBoneRotate=new Quaternion();
	}

	public Mesh(){}

	public int boundingRadius; // Before world scale
	public int worldScale;

	public SubMesh[] subMeshes;

	public short[] triIndexData;
	public byte[] triSubMesh;

	public VertexData vertexData;

	public byte[] boneControllingVertexes;
	public Bone[] bones=new Bone[0];
	public Animation[] animations=new Animation[0];
}
