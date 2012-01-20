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

#ifndef TOADLET_PAD_APPLET_H
#define TOADLET_PAD_APPLET_H

#include <toadlet/pad/Types.h>

namespace toadlet{
namespace pad{

class Applet{
public:
	virtual ~Applet(){}

	virtual void create()=0;
	virtual void destroy()=0;
	virtual void update(int dt)=0;
	virtual void render()=0;

	/// @todo: Replace these with events, so I can just have 1 event method to override instead of many
	virtual void resized(int width,int height)=0;
	virtual void focusGained()=0;
	virtual void focusLost()=0;

	virtual void keyPressed(int key)=0;
	virtual void keyReleased(int key)=0;

	virtual void mousePressed(int x,int y,int button)=0;
	virtual void mouseMoved(int x,int y)=0;
	virtual void mouseReleased(int x,int y,int button)=0;
	virtual void mouseScrolled(int x,int y,int scroll)=0;
};

}
}

#endif
