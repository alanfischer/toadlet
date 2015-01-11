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

#ifndef TOADLET_TADPOLE_FONTMANAGER_H
#define TOADLET_TADPOLE_FONTMANAGER_H

#include <toadlet/tadpole/ResourceManager.h>
#include <toadlet/tadpole/Font.h>
#include <toadlet/tadpole/FontData.h>

namespace toadlet{
namespace tadpole{

class Engine;

class TOADLET_API FontManager:public ResourceManager,public ResourceRequest{
public:
	TOADLET_IOBJECT(FontManager);

	FontManager(Engine *engine);

	void findDefaultFont();

	Font::ptr getDefaultFont();

	Font::ptr getFont(const String &name,float pointSize){return static_pointer_cast<Font>(get(name+String(":")+pointSize));}

	bool findFont(const String &name,float pointSize,ResourceRequest *request){return find(name,request,new FontData(pointSize));}
	
	Resource::ptr manage(Resource *resource,const String &name=(char*)NULL);

	bool find(const String &name,ResourceRequest *request,ResourceData *data=NULL);

	ResourceData *createResourceData(){return new FontData();}

	void resourceReady(Resource *resource);
	void resourceException(const Exception &ex){}
	void resourceProgress(float progress){}

protected:
	String mDefaultCharacterSet;
	Font::ptr mDefaultFont;
};

}
}

#endif
