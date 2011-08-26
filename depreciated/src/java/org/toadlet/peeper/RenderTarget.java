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

package org.toadlet.peeper;

#include <org/toadlet/peeper/Types.h>

public interface RenderTarget{
	public interface Peer{
		public Texture.Peer castToTexturePeer();

		public void destroy();

		public boolean isValid();
		
		public int getWidth();
		public int getHeight();
	}

	/// @todo:add a destroy ()to RenderTarget so we can destroy() the Peer

	public Texture castToTexture();

	public int getWidth();
	public int getHeight();

	public void internal_setRenderTargetPeer(RenderTarget.Peer renderTargetPeer);
	public RenderTarget.Peer internal_getRenderTargetPeer();
}
