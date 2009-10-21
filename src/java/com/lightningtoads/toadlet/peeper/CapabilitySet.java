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

public class CapabilitySet{
	public boolean resetOnResize=false;
	public boolean hardwareTextures=false;
	public boolean hardwareVertexBuffers=false;
	public boolean hardwareIndexBuffers=false;
	public int maxLights=0;
	public int maxTextureStages=0;
	public int maxTextureSize=0;
	public boolean pointSprites=false;
	public boolean vertexShaders=false;
	public int maxVertexShaderLocalParameters=0;
	public boolean fragmentShaders=false;
	public int maxFragmentShaderLocalParameters=0;
	public boolean renderToTexture=false;
	public boolean renderToDepthTexture=false;
	public boolean renderToTextureNonPowerOf2Restricted=false;
	public boolean textureDot3=false;
	public boolean textureNonPowerOf2Restricted=false;
	public boolean textureNonPowerOf2=false;
	public int idealFormatBit=VertexElement.Format.BIT_FLOAT_32;
}
