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
	FontManager(egg::io::Archive *archive);

	Font::ptr getFont(const egg::String &name,float pointSize){return egg::shared_static_cast<Font>(get(name+egg::String(":")+pointSize));}
	Font::ptr findFont(const egg::String &name,float pointSize){return egg::shared_static_cast<Font>(find(name,FontData::ptr(new FontData(pointSize))));}
	Font::ptr getDefaultFont();
	
	egg::Resource::ptr manage(const egg::Resource::ptr &resource,const egg::String &name=(char*)NULL);
	void unmanage(egg::Resource *resource);

	egg::Resource::ptr find(const egg::String &name,ResourceHandlerData::ptr handlerData=ResourceHandlerData::ptr());

protected:
	egg::String mDefaultCharacterSet;
	Font::ptr mDefaultFont;
};

}
}

#endif
