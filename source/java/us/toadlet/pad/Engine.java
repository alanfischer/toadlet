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

package us.toadlet.pad;

import android.content.*;
import us.toadlet.peeper.*;
import us.toadlet.ribbit.*;

public class Engine{
	public Engine(Context ctx){
		makeEngine(ctx);
	}
	
	public native void destroy();

	public native void installHandlers();

	public native void setRenderDevice(RenderDevice device);
	
	public native void setAudioDevice(AudioDevice device);

	public native void hack_addNodes();
	public native void hack_render();
	public native void hack_update(int dt);
	
	private native void makeEngine(Context ctx);
	
	protected int mNativeHandle;
}
