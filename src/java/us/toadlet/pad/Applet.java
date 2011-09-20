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

public interface Applet{
	public void create();
	public void destroy();
	public void update(int dt);
	public void render(RenderDevice renderDevice);

	public void resized(int width,int height);
	public void focusGained();
	public void focusLost();
	public void keyPressed(int key);
	public void keyReleased(int key);
	public void mousePressed(int x,int y,int button);
	public void mouseMoved(int x,int y);
	public void mouseReleased(int x,int y,int button);
	public void mouseScrolled(int x,int y,int scroll);
}