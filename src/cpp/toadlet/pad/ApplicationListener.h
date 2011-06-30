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

#ifndef TOADLET_PAD_APPLICATIONLISTENER_H
#define TOADLET_PAD_APPLICATIONLISTENER_H

namespace toadlet{
namespace peeper{class RenderDevice;}
namespace pad{

class ApplicationListener{
public:
	virtual ~ApplicationListener(){}

	virtual void resized(int width,int height)=0;
	virtual void focusGained()=0;
	virtual void focusLost()=0;
	virtual void keyPressed(int key)=0;
	virtual void keyReleased(int key)=0;
	virtual void mousePressed(int x,int y,int button)=0;
	virtual void mouseMoved(int x,int y)=0;
	virtual void mouseReleased(int x,int y,int button)=0;
	virtual void mouseScrolled(int x,int y,int scroll)=0;
	virtual void joyPressed(int button)=0;
	virtual void joyMoved(scalar x,scalar y,scalar z,scalar r,scalar u,scalar v)=0;
	virtual void joyReleased(int button)=0;
	virtual void update(int dt)=0;
	virtual void render(peeper::RenderDevice *renderDevice)=0;
};

}
}

#endif
