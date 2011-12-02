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

package org.toadlet.egg;

import java.util.*;

public final class Profile{
	public final int STACK_MAX_DEPTH=20;
	
	public static Profile getInstance(){
		if(mTheProfile==null){
			mTheProfile=new Profile();
		}
		return mTheProfile;
	}

	public void beginSection(String name){
		Timing timing=mTimings.get(name);
		if(timing==null){
			timing=new Timing(name);
			mTimings.put(name,timing);
		}

		timing.depth++;
		if(timing.depth==1){
			timing.current=System.nanoTime();
		}
		else if(timing.depth>STACK_MAX_DEPTH){
			Error.unknown(Categories.TOADLET_EGG,
				"timing stack depth > STACK_MAX_DEPTH");
			return;
		}

		mTimingStack.add(timing);
	}

	public void endSection(String name){
		long time=System.nanoTime();

		if(mTimingStack.size()==0){
			Error.unknown(Categories.TOADLET_EGG,
				"empty timing stack");
		}

		Timing timing=mTimingStack.remove(mTimingStack.size()-1);

		if(timing.name.equals(name)==false){
			Error.unknown(Categories.TOADLET_EGG,
				"mismatched begin/endSection");
			return;
		}

		timing.depth--;
		if(timing.depth==0){
			timing.total+=(time-timing.current);
		}
		else if(timing.depth<0){
			Error.unknown(Categories.TOADLET_EGG,
				"timing stack depth < 0");
			return;
		}
	}

	public void addTimings(){
		if(mTimingStack.size()!=0){
			Error.unknown(Categories.TOADLET_EGG,
				"non empty timing stack");
		}

		mTimingHistory.add(mTimings);
		mTimings=new HashMap<String,Timing>();
	}

	public void clearTimings(){
		mTimings.clear();
		mTimingStack.clear();
		mTimingHistory.clear();
	}

	public int getTimingAverage(String name){
		long time=0,count=0;
		int i;
		for(i=mTimingHistory.size()-1;i>=0;--i){
			HashMap<String,Timing> item=mTimingHistory.get(i);
			Timing timing=item.get(name);
			if(timing!=null){
				time+=timing.total;
				count++;
			}
		}

		if(count==0){
			return -1;
		}
		else{
			return (int)((time/count)/1000); // 1000 to convert to microseconds
		}
	}

	protected class Timing{
		public Timing(String name){
			this.name=name;
		}

		public String name;
		public long total=0;
		public long current=0;
		public int depth=0;
	};

	protected Profile(){}

	protected static Profile mTheProfile=null;

	protected HashMap<String,Timing> mTimings=new HashMap<String,Timing>();
	protected Vector<Timing> mTimingStack=new Vector<Timing>();
	protected Vector<HashMap<String,Timing>> mTimingHistory=new Vector<HashMap<String,Timing>>();
}
