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

#include <toadlet/tadpole/handler/XMLParticleHandler.h>

namespace toadlet{
namespace tadpole{
namespace handler{

bool XMLParticleHandler::loadParticleFile(egg::Collection<Vector3> &points,InputStream::ptr in){
	if(in==NULL){
		Error::unknown(Categories::TOADLET_TADPOLE,
			"error opening particle file");
		return false;
	}

	/// @todo: Replace the following when mxml implements custom load/save callbacks
	char buffer[1025];
	int amount=0;
	String string;
	while((amount=in->read(buffer,1024))>0){
		buffer[amount]=0;
		string+=buffer;
	}

	mxml_node_t *root=mxmlLoadString(NULL,string,MXML_OPAQUE_CALLBACK);

	mxml_node_t *lineNode=root->child;
	while((lineNode=lineNode->next)!=NULL){
		if(strcmp(mxmlGetElementName(lineNode),"line")!=0){
			continue;
		}

		mxml_node_t *pointNode=lineNode->child;
		while((pointNode=pointNode->next)!=NULL){
			if(strcmp(mxmlGetElementName(pointNode),"point")!=0){
				continue;
			}

			String data=mxmlGetCDATA(pointNode->child);

			Vector3 point;
			int space1=data.find((char)10);
			int space2=data.find((char)10,space1+1);
			if(space1>0 && space2>0){
				float x=data.substr(0,space1).toFloat();
				float y=data.substr(space1+1,space2-(space1+1)).toFloat();
				float z=data.substr(space2+1,data.length()).toFloat();
				#if defined(TOADLET_FIXED_POINT)
					point.x=Math::fromFloat(x);
					point.y=Math::fromFloat(y);
					point.z=Math::fromFloat(z);
				#else
					point.x=x;
					point.y=y;
					point.z=z;
				#endif
			}

			points.add(point);
		}
	}

	return true;
}

}
}
}
