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

import java.util.HashMap;
import java.util.Vector;

public class ObjectManager{
	public ObjectManager(){}

	public Object alloc(Class type){
		Vector<Object> available=mAllocated.get(type);
		if(available!=null && available.size()>0){
			Object object=available.get(0);
			available.remove(0);
			return object;
		}
		else if(mFactory!=null){
			return mFactory.createObjectOfType(type);
		}
		else{
			Object object=null;
			try{
				object=type.newInstance();
			}
			catch(Exception ex){}
			return object;
		}
	}

	public void free(Object object){
		Vector<Object> available=mAllocated.get(object.getClass());
		if(available==null){
			available=new Vector<Object>();
			mAllocated.put(object.getClass(),available);
		}
		available.add(object);
	}

	public void setFactory(ObjectFactory factory){
		mFactory=factory;
	}

	protected HashMap<Class,Vector<Object>> mAllocated=new HashMap<Class,Vector<Object>>();
	protected ObjectFactory mFactory;
}
