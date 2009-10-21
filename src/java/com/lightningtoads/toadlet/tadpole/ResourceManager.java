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

package com.lightningtoads.toadlet.tadpole;

#include <com/lightningtoads/toadlet/tadpole/Types.h>

import com.lightningtoads.toadlet.egg.Categories;
import com.lightningtoads.toadlet.egg.Error;
import com.lightningtoads.toadlet.egg.Logger;
import com.lightningtoads.toadlet.egg.Resource;
import com.lightningtoads.toadlet.egg.io.InputStreamFactory;
import java.io.InputStream;
import java.lang.ref.WeakReference;
import java.util.HashMap;
import java.util.Map;

public class ResourceManager{
	public class ResourceCache{
		public ResourceCache(Resource resource,String name){
			this.resource=new WeakReference<Resource>(resource);
			this.name=name;
			count=1;
		}

		WeakReference<Resource> resource;
		Resource cachedResource;
		String name;
		int count;
	};

	public ResourceManager(InputStreamFactory inputStreamFactory){
		mInputStreamFactory=inputStreamFactory;
	}

	void setInputStreamFactory(InputStreamFactory inputStreamFactory){
		mInputStreamFactory=inputStreamFactory;
	}

// TODO: Revisit the ResourceManager/PointerQueue implementation to make it compatible with Java & C#
#if 1
	public Resource load(String name){
		return load(false,name,name,null,null);
	}

	public Resource load(String name,String file){
		return load(false,name,file,null,null);
	}

	public Resource load(String name,ResourceHandlerData data){
		return load(false,name,name,null,data);
	}

	public Resource load(String name,String file,ResourceHandlerData data){
		return load(false,name,file,null,data);
	}

	public Resource load(String name,Resource resource){
		return load(false,name,null,resource,null);
	}

	public Resource load(Resource resource){
		return load(false,null,null,resource,null);
	}
#endif
	public Resource cache(String name){
		return load(true,name,name,null,null);
	}

	public Resource cache(String name,String file){
		return load(true,name,file,null,null);
	}

	public Resource cache(String name,ResourceHandlerData data){
		return load(true,name,name,null,data);
	}

	public Resource cache(String name,String file,ResourceHandlerData data){
		return load(true,name,file,null,data);
	}

	public Resource cache(String name,Resource resource){
		return load(true,name,null,resource,null);
	}

	public Resource cache(Resource resource){
		return load(true,null,null,resource,null);
	}

	public boolean uncache(String name){
		ResourceCache it=mResourceNameMap.get(name);
		if(it!=null){
			uncache(it);
			return true;
		}
		return false;
	}

	public boolean uncache(Resource resource){
		ResourceCache it=mResourcePtrMap.get(resource);
		if(it!=null){
			uncache(it);
			return true;
		}
		return false;
	}

	public Resource get(String name){
		ResourceCache it=mResourceNameMap.get(name);
		if(it!=null){
			return it.resource.get();
		}
		else{
			Logger.log(Categories.TOADLET_TADPOLE,Logger.Level.DEBUG,
				"Resource "+name+" not found");
			return null;
		}
	}

	public void resourceLoaded(Resource resource){}

	public void resourceUnloaded(Resource resource){}

	public Map<Resource,ResourceCache> getResourcePtrMap(){return mResourcePtrMap;}

	// TODO: This class in Java & C++ should be replaced with something cleaner, perhaps using regex
	public static String cleanFilename(String name){return name;}

	public void addHandler(ResourceHandler handler,String extension){
		ResourceHandler it=mExtensionHandlerMap.get(extension);
		if(it!=null){
			Logger.log(Categories.TOADLET_TADPOLE,Logger.Level.DEBUG,
				"Removing handler for extension "+extension);
		}

		Logger.log(Categories.TOADLET_TADPOLE,Logger.Level.DEBUG,
			"Adding handler for extension "+extension);
		mExtensionHandlerMap.put(extension,handler);
	}

	public void removeHandler(String extension){
		ResourceHandler it=mExtensionHandlerMap.remove(extension);
		if(it!=null){
			Logger.log(Categories.TOADLET_TADPOLE,Logger.Level.DEBUG,
				"Removing handler for extension "+extension);
		}
		else{
			Logger.log(Categories.TOADLET_TADPOLE,Logger.Level.DEBUG,
				"Handler not found for extension "+extension);
		}
	}

	public ResourceHandler getHandler(String extension){
		return mExtensionHandlerMap.get(extension);
	}

	public void setDefaultHandler(ResourceHandler handler){
		mDefaultHandler=handler;
	}
	
	public ResourceHandler getDefaultHandler(){
		return mDefaultHandler;
	}

	public void setDefaultExtension(String extension){
		mDefaultExtension=extension;
	}

	public Resource load(boolean cache,String name,String file,Resource resource,ResourceHandlerData handlerData){
		if(Logger.getInstance().getMasterCategoryReportingLevel(Categories.TOADLET_TADPOLE)>=Logger.Level.EXCESSIVE){
			String description=cache?"Caching":"Loading";
			description=description+" resource ";
			if(file!=null){
				description=description+file+" ";
			}
			if(name!=null){
				description=description+"as "+name;
			}
			else{
				description=description+"anonymously";
			}

			Logger.log(Categories.TOADLET_TADPOLE,Logger.Level.EXCESSIVE,
				description);
		}

		ResourceCache resCache=null;
		if(name!=null){
			resCache=mResourceNameMap.get(name);
		}

		if(resCache!=null){
			if(resource!=null && resCache.resource.get()!=resource){
				Logger.log(Categories.TOADLET_TADPOLE,Logger.Level.WARNING,
					"Replacing previous resource of same name, previous resource is becoming anonymous");

				resCache.name=null;
				mResourceNameMap.remove(name);
				resCache=null;
			}
			else if(cache){
				resCache.count++;
			}
		}

		if(resCache==null){
			if(resource==null){
				if(file!=null){
					try{
						resource=loadFromFile(file,handlerData);
					}catch(Exception ex){
						resource=null;
					}
					if(resource==null){
						return resource;
					}
				}
				else{
					return resource;
				}
			}
			resCache=new ResourceCache(resource,name);
// TODO
//			resCache.resource.setPointerQueue(this);
			if(cache){
				resCache.cachedResource=resource;
			}
			if(name!=null){
				mResourceNameMap.put(name,resCache);
			}
			mResourcePtrMap.put(resource,resCache);

			resourceLoaded(resCache.resource.get());
		}

		return resCache.resource.get();
	}

	public Resource loadFromFile(String name,ResourceHandlerData handlerData){
		String filename=cleanFilename(name);

		String extension="";
		int i=filename.lastIndexOf('.');
		if(i!=-1){
			extension=filename.substring(i+1,filename.length()).toLowerCase();
		}
		else if(mDefaultExtension!=null){
			extension=mDefaultExtension;
			filename+="."+extension;
		}
		if(extension!=null){
			ResourceHandler it=mExtensionHandlerMap.get(extension);
			if(it!=null){
				InputStream in=mInputStreamFactory.makeInputStream(filename);
				if(in!=null){
					return it.load(in,handlerData);
				}
				else{
					Error.unknown(Categories.TOADLET_TADPOLE,
						"File "+filename+" not found");
					return null;
				}
			}
			else if(mDefaultHandler!=null){
				InputStream in=mInputStreamFactory.makeInputStream(filename);
				if(in!=null){
					return mDefaultHandler.load(in,handlerData);
				}
				else{
					Error.unknown(Categories.TOADLET_TADPOLE,
						"File "+filename+" not found");
					return null;
				}
			}
			else{
				Error.unknown(Categories.TOADLET_TADPOLE,
					// We require the extra \'s here because the preprocessor strips them out
					"Handler for extension \\""+extension+"\\" not found");
				return null;
			}
		}
		else{
			Error.unknown(Categories.TOADLET_TADPOLE,
				"Extension not found on file");
			return null;
		}
	}

	void uncache(ResourceCache resCache){
		if(resCache!=null){
			resCache.count--;
			if(resCache.count<=0){
// TODO
resourceUnloaded(resCache.cachedResource);
				resCache.cachedResource=null;
			}
		}
	}

	HashMap<Resource,ResourceCache> mResourcePtrMap=new HashMap<Resource,ResourceCache>();
	HashMap<String,ResourceCache> mResourceNameMap=new HashMap<String,ResourceCache>();

	InputStreamFactory mInputStreamFactory=null;
	HashMap<String,ResourceHandler> mExtensionHandlerMap=new HashMap<String,ResourceHandler>();
	ResourceHandler mDefaultHandler=null;
	String mDefaultExtension=null;
}
