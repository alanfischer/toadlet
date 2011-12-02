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

#ifndef TOADLET_TADPOLE_STUDIO_STUDIOMODEL_H
#define TOADLET_TADPOLE_STUDIO_STUDIOMODEL_H

#include <toadlet/egg/BaseResource.h>
#include <toadlet/tadpole/studio/StudioTypes.h>
#include <toadlet/tadpole/material/Material.h>

namespace toadlet{
namespace tadpole{
namespace studio{

class TOADLET_API StudioModel:public BaseResource{
public:
	TOADLET_BASERESOURCE_PASSTHROUGH(BaseResource);
	TOADLET_SHARED_POINTERS(StudioModel);

	StudioModel():
		data(NULL),
		header(NULL)
	{}
	
	void destroy(){
		if(data!=NULL){
			delete[] data;
			data=NULL;
		}
		header=NULL;

		int i;
		for(i=0;i<textures.size();++i){
			textures[i]->release();
		}
		textures.clear();

		for(i=0;i<materials.size();++i){
			materials[i]->destroy();
		}
		materials.clear();
	}

	studiobodyparts *bodyparts(int i){return &((studiobodyparts*)(data+header->bodypartindex))[i];}
	studiomodel *model(studiobodyparts *bodyparts,int i){return &((studiomodel*)(data+bodyparts->modelindex))[i];}
	studiomesh *mesh(studiomodel *model,int i){return &((studiomesh*)(data+model->meshindex))[i];}
	studiotexture *texture(int i){return &((studiotexture*)(data+header->textureindex))[i];}
	short skin(int i){return ((short*)(data+header->skinindex))[i];}
	studiobone *bone(int i){return &((studiobone*)(data+header->boneindex))[i];}
	studioseqdesc *seqdesc(int i){return &((studioseqdesc*)(data+header->seqindex))[i];}
	studioseqgroup *seqgroup(int i){return &((studioseqgroup*)(data+header->seqgroupindex))[i];}
	studioanim *anim(studioseqdesc *seqdesc){
		studioseqgroup *sseqgroup=seqgroup(seqdesc->seqgroup);
		return ((studioanim*)(data+sseqgroup->data+seqdesc->animindex));
	}
	studioanimvalue *animvalue(studioanim *anim,int i){return ((studioanimvalue*)((tbyte*)anim+anim->offset[i]));}
	studiobbox *bbox(int i){return &((studiobbox*)(data+header->hitboxindex))[i];}
	studiobonecontroller *bonecontroller(int i){return &((studiobonecontroller*)(data+header->bonecontrollerindex))[i];}
	studioattachment *attachment(int i){return &((studioattachment*)(data+header->attachmentindex))[i];}

	struct meshdata{
		int vertexStart;
		Collection<IndexData::ptr> indexDatas;
	};

	meshdata *findmeshdata(int bodypartIndex,int modelIndex,int meshIndex){
		int i,j,k,index=0;
		for(i=0;i<=header->numbodyparts;++i){
			studiobodyparts *sbodyparts=bodyparts(i);
			for(j=0;j<sbodyparts->nummodels;++j){
				studiomodel *smodel=model(sbodyparts,j);
				for(k=0;k<smodel->nummesh;++k){
					if(i==bodypartIndex && j==modelIndex && k==meshIndex){
						return &meshdatas[index];
					}
					index++;
				}
			}
		}
		return NULL;
	}

	tbyte *data;
	studiohdr *header;
	Collection<Texture::ptr> textures;
	Collection<Material::ptr> materials;
	Collection<meshdata> meshdatas;
	int vertexCount;
};

}
}
}

#endif
