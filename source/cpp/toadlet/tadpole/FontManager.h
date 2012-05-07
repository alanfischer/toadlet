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

class TOADLET_API FontManager:public ResourceManager{
public:
	TOADLET_OBJECT(FontManager);

	FontManager(Engine *engine);

	Font::ptr getFont(const String &name,float pointSize){return shared_static_cast<Font>(get(name+String(":")+pointSize));}
	Font::ptr findFont(const String &name,float pointSize){return shared_static_cast<Font>(find(name,FontData::ptr(new FontData(pointSize))));}
	Font::ptr getDefaultFont();
	
	Resource::ptr manage(const Resource::ptr &resource,const String &name=(char*)NULL);
	void unmanage(Resource *resource);

	Resource::ptr find(const String &name,ResourceData *data=NULL);

protected:
	String mDefaultCharacterSet;
	Font::ptr mDefaultFont;
};

}
}

#endif
