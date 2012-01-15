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

package us.toadlet.flick;

public interface InputDevice{
	static final int InputType_JOY=0;
	static final int InputType_MOTION=1;
	static final int InputType_ANGULAR=2;
	static final int InputType_LIGHT=3;
	static final int InputType_PROXIMITY=4;
	static final int InputType_MAGNETIC=5;
	static final int InputType_MAX=6;

	public boolean create();
	public void destroy();

	public int getType();
	public boolean start();
	public void update(int dt);
	public void stop();

	public void setListener(InputDeviceListener listener);
	public void setSampleTime(int dt);
	public void setAlpha(float alpha);
}
