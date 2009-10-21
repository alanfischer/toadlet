#include "MAPHandler.h"
#include <toadlet/egg/math/MathTextSTDStream.h>
#include <toadlet/tadpole/material/StandardMaterial.h>

using namespace toadlet::egg;
using namespace toadlet::egg::io;
using namespace toadlet::egg::math;
using namespace toadlet::egg::math::Math;
using namespace toadlet::peeper;
using namespace toadlet::tadpole::material;

namespace csg{

Map *MAPHandler::load(InputStream *in,const String &name){
	return NULL;
}

void MAPHandler::save(Map *map,OutputStream *out,const String &name,bool invertYTexCoord){
	int i,j,k;

	StringStream ss;

	for(i=0;i<map->mapEntities.size();++i){
		MapEntity *entity=&map->mapEntities[i];
		BrushCollection *brushCollection=&entity->brushCollection;

		ss << "{" << std::endl;

		if(i==0){
			bool hasClassname=false;
			bool hasMapversion=false;

			for(j=0;entity->keyValues.size();++j){
				if(entity->keyValues[j].first=="classname"){
					hasClassname=true;
				}
				if(entity->keyValues[j].first=="mapversion"){
					hasMapversion=true;
				}
			}

			if(hasClassname==false){
				ss << "\t\"classname\" \"worldspawn\"" << std::endl;
			}
			if(hasMapversion==false){
				ss << "\t\"mapversion\" \"220\"" << std::endl;
			}
		}

		for(j=0;j<entity->keyValues.size();++j){
			ss << "\t\"" << entity->keyValues[j].first << "\" \"" << entity->keyValues[j].second << "\"" << std::endl;
		}

		for(j=0;j<brushCollection->brushes.size();++j){
			Brush *brush=brushCollection->brushes.at(j);

			ss << "\t{" << std::endl;
			for(k=0;k<brush->getNumFaces();++k){
				BrushFace *face=brush->getFace(k);
				int materialIndex=face->getMaterialIndex();
				String materialName="AAATRIGGER";
				unsigned int textureWidth=0,textureHeight=0;

				if(materialIndex>=0 && materialIndex<brushCollection->materials.size()){
					Material *material=brushCollection->materials[materialIndex];
					StandardMaterial *smat=dynamic_cast<StandardMaterial*>(material);
					if(smat!=NULL){
						int d=smat->getMapName(StandardMaterial::MAP_DIFFUSE).rfind(".");
						if(d==String::npos){
							materialName=smat->getMapName(StandardMaterial::MAP_DIFFUSE);
						}
						else{
							materialName=smat->getMapName(StandardMaterial::MAP_DIFFUSE).substr(0,d);
						}
						std::transform(materialName.begin(),materialName.end(),materialName.begin(),toupper);

						if(smat->getMap(StandardMaterial::MAP_DIFFUSE)!=NULL){
							smat->getMap(StandardMaterial::MAP_DIFFUSE)->getSize(textureWidth,textureHeight);
						}
					}
				}

				const Vector3 point1=face->getPolygonPoint(0);
				const Vector3 point2=face->getPolygonPoint(1);
				const Vector3 point3=face->getPolygonPoint(2);

				ss << "\t\t";
				ss << "( " << point1 << " ) ";
				ss << "( " << point2 << " ) ";
				ss << "( " << point3 << " ) ";
				ss << materialName << " ";

				Vector3 uaxis=face->getUAxis();
				Vector3 vaxis=face->getVAxis();
				Vector2 uvoffset=face->getUVOffset();

				if(invertYTexCoord){
					vaxis*=-1;
					uvoffset.y=textureHeight-uvoffset.y;
				}

				ss << "[ " << uaxis.x << " " << uaxis.y << " " << uaxis.z << " " << uvoffset.x << " ] ";
				ss << "[ " << vaxis.x << " " << vaxis.y << " " << vaxis.z << " " << uvoffset.y << " ] ";
				ss << "0 1 1" << std::endl;
			}
			ss << "\t}" << std::endl;
		}

		ss << "}" << std::endl;
	}

	out->write(ss.str().c_str(),ss.str().length());
}

}
