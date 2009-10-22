#include "TreeTraversalClasses.h"
#include <toadlet/egg/String.h>
#include <toadlet/tadpole/material/StandardMaterial.h>
#include "../csg/Brush.h"
#include "../MAXUtilities/MAXUtilities.h"

#pragma comment(lib,"Poly.lib")
#pragma comment(lib,"MNMath.lib")

#include "DECOMP.H"

using namespace toadlet::egg;
using namespace toadlet::egg::math;
using namespace toadlet::egg::math::Math;
using namespace csg;

// Simple debugging function
static void assertionFailed(bool b, char *err){
	if(!b){
		throw std::runtime_error(err);
	}
}

Matrix3 getTotalTransform(INode *node,TimeValue time){
	if(node->GetParentNode()!=NULL){
		return getTotalTransform(node->GetParentNode(),time)*node->GetNodeTM(time);
	}
	else{
		return node->GetNodeTM(time);
	}
}

int GetGeometry::callback(INode *node){
	Object *obj=node->EvalWorldState(mDumpTime).obj;

	if(obj->SuperClassID()==LIGHT_CLASS_ID){
		// Get max light info
		ObjectState os = node->EvalWorldState(mDumpTime);
		if(!os.obj){
			return TREE_CONTINUE;
		}
		GenLight* maxLight = (GenLight*)os.obj;
		struct LightState ls;
		Interval valid = FOREVER;
		maxLight->EvalLightState(mDumpTime, valid, &ls);

		float red = ls.color.r*ls.intens;
		float green = ls.color.g*ls.intens;
		float blue = ls.color.b*ls.intens;

		// Get light position and orientation
		Matrix3 nm = node->GetNodeTM(mDumpTime);
		AffineParts ap;
		decomp_affine(nm, &ap);
		Point3 pos = ap.t;
		Quat rot = ap.q;
		float x=0,y=0,z=0;
		rot.GetEuler(&x,&y,&z);

		// Create a toadlet light
		toadlet::peeper::Light light;
		light.setPosition(Vector3(pos[0],pos[1],pos[2]));
		light.setDiffuse(Vector4(red, green, blue, 1.0f));
		light.setSpecular(Vector4(red, green, blue, 1.0f));

		if(ls.type==OMNI_LIGHT){
			// No direction
			light.setType(toadlet::peeper::Light::LT_POSITION);
		}
		else if(ls.type==DIR_LIGHT){
			light.setType(toadlet::peeper::Light::LT_DIRECTION);
			// TODO: Calculate a direction vector from the Euler's XYZ
		}
		else{
			// Just ignore it
		}
		light.setLinearAttenuation(1.0f);

		mExport->addLight(light);

		return TREE_CONTINUE;
	}
	else{
		int i;

		String nodeName(node->GetName());
		const Matrix3 nm=node->GetNodeTM(mDumpTime);
		Object *obj=node->EvalWorldState(mDumpTime).obj;

		BOOL negScale=(DotProd(CrossProd(nm.GetRow(0),nm.GetRow(1)),nm.GetRow(2))<0.0)?1:0;
		int vi0=0,vi1=1,vi2=2;
		if(negScale){
			vi0=2;
			vi1=1;
			vi2=0;
		}

		Mtl *nodeMtl=node->GetMtl();

		if(obj->CanConvertToType(polyObjectClassID)){
			PolyObject *polyObj=(PolyObject*)obj->ConvertToType(mDumpTime,polyObjectClassID);

			Brush *brush=new Brush();

			MNMesh *mnmesh=&polyObj->GetMesh();
			int map=1;

			for(i=0;i<mnmesh->FNum();++i){
				MNFace *mnface=mnmesh->F(i);
				MNMapFace *mnmapface=mnmesh->MF(map,i);
				if(mnface->deg>=3){
					Mtl *mtl=nodeMtl;

					if(nodeMtl!=NULL){
						if(nodeMtl->IsMultiMtl()){
							// We have a multi material, so find the currently active one
							MtlID materialId=mnface->material;
							if(materialId>=mtl->NumSubMtls()){
								materialId=0;
							}
							mtl=nodeMtl->GetSubMtl(materialId);

							if(mtl->IsMultiMtl()){
								// Found a sub-material
								mtl=mtl->GetSubMtl(materialId);
							}
						}
					}

					unsigned int textureWidth=1,textureHeight=1;
					int materialIndex=mExport->getMaterialIndexFromMtl(mtl);
					toadlet::tadpole::material::Material::Ptr material=mExport->mBrushCollection.materials[materialIndex];
					toadlet::tadpole::material::StandardMaterial::Ptr smat=shared_dynamic_cast<toadlet::tadpole::material::StandardMaterial>(material);
					if(smat!=NULL){
						toadlet::peeper::Texture *tex=smat->getMap(toadlet::tadpole::material::StandardMaterial::MAP_DIFFUSE);
						if(tex!=NULL){
							tex->getSize(textureWidth,textureHeight);
							if(textureWidth==0){
								textureWidth=1;
							}
							if(textureHeight==0){
								textureHeight=1;
							}
						}
					}

					BrushFace *face=new BrushFace(
						mExport->snapToGrid(makeVector3FromMaxPoint3(nm.PointTransform(mnmesh->P(mnface->vtx[vi2])))),
						makeVector2FromMaxPoint3(mnmesh->MV(map,mnmapface->tv[vi2])),
						mExport->snapToGrid(makeVector3FromMaxPoint3(nm.PointTransform(mnmesh->P(mnface->vtx[vi1])))),
						makeVector2FromMaxPoint3(mnmesh->MV(map,mnmapface->tv[vi1])),
						mExport->snapToGrid(makeVector3FromMaxPoint3(nm.PointTransform(mnmesh->P(mnface->vtx[vi0])))),
						makeVector2FromMaxPoint3(mnmesh->MV(map,mnmapface->tv[vi0])),
						textureWidth,textureHeight,materialIndex
					);

					brush->addFace(face);
				}
			}

			Brush::CompileResult result=brush->compile();
			if(result==Brush::CR_SUCCESS || result<Brush::CR_ERRORS){
				mExport->addBrush(brush);
			}
			else{
				mExport->reportInvalidObject(node->GetName(),result);
				delete brush;
			}
		}
		else{
			mExport->reportNonBrushObject(node->GetName());
		}
	}

	return TREE_CONTINUE;
}
