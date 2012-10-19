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

package org.toadlet.tadpole.handler;

#include <org/toadlet/tadpole/Types.h>

import org.toadlet.egg.*;
import org.toadlet.peeper.*;
import org.toadlet.tadpole.*;
import java.io.*;

public class TextureHandler implements ResourceHandler{
	public Resource load(InputStream in,ResourceHandlerData handlerData){
		android.graphics.Bitmap bitmap=android.graphics.BitmapFactory.decodeStream(in);
		return new Texture(bitmap);
	}
}
