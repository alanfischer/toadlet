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

#include <toadlet/egg/Error.h>
#include <toadlet/tadpole/bsp/BSP30Handler.h>
#include <toadlet/tadpole/bsp/BSPMap.h>
#include <stdlib.h>

using namespace toadlet::egg;
using namespace toadlet::egg::io;

namespace toadlet{
namespace tadpole{
namespace bsp{

BSP30Handler::BSP30Handler(Engine *engine):ResourceHandler(),
	mEngine(NULL),
	mEpsilon(0)
{
	mEngine=engine;
	mEpsilon=0.001f;
	memset(&header,0,sizeof(header));
}

BSP30Handler::~BSP30Handler(){}

Resource::ptr BSP30Handler::load(InputStream::ptr in,const ResourceHandlerData *handlerData){
	DataInputStream::ptr din(new DataInputStream(in));

	header.version=din->readLittleInt32();
	din->reset();

	if(header.version!=BSPVERSION){
		Error::unknown(String("incorrect bsp version:")+header.version);
		return NULL;
	}

	Logger::debug(Categories::TOADLET_TADPOLE,"Reading map");

	in->read((char*)&header,sizeof(header));

	bmodel *models;		int nmodels;
	bvertex *vertexes;	int nvertexes;
	bplane *planes;		int nplanes;
	bleaf *leafs;		int nleafs;
	bnode *nodes;		int nnodes;
	btexinfo *texinfos;	int ntexinfos;
	bclipnode *clipnodes;int nclipnodes;
	bface *faces;		int nfaces;
	bmarksurface *marksurfaces;int nmarksurfaces;
	bsurfedge *surfedges;int nsurfedges;
	bedge *edges;		int nedges;
	void *visibility;	int nvisibility;
	void *textures;		int ntextures;
	void *lighting;		int nlighting;
	void *entities;		int nentities;

	readLump(in,LUMP_MODELS,(void**)&models,sizeof(bmodel),&nmodels);
	readLump(in,LUMP_VERTEXES,(void**)&vertexes,sizeof(bvertex),&nvertexes);
	readLump(in,LUMP_PLANES,(void**)&planes,sizeof(bplane),&nplanes);
	readLump(in,LUMP_LEAFS,(void**)&leafs,sizeof(bleaf),&nleafs);
	readLump(in,LUMP_NODES,(void**)&nodes,sizeof(bnode),&nnodes);
	readLump(in,LUMP_TEXINFO,(void**)&texinfos,sizeof(btexinfo),&ntexinfos);
	readLump(in,LUMP_CLIPNODES,(void**)&clipnodes,sizeof(bclipnode),&nclipnodes);
	readLump(in,LUMP_FACES,(void**)&faces,sizeof(bface),&nfaces);
	readLump(in,LUMP_MARKSURFACES,(void**)&marksurfaces,sizeof(bmarksurface),&nmarksurfaces);
	readLump(in,LUMP_SURFEDGES,(void**)&surfedges,sizeof(bsurfedge),&nsurfedges);
	readLump(in,LUMP_EDGES,(void**)&edges,sizeof(bedge),&nedges);
	readLump(in,LUMP_VISIBILITY,(void**)&visibility,1,&nvisibility);
	readLump(in,LUMP_TEXTURES,(void**)&textures,1,&ntextures);
	readLump(in,LUMP_LIGHTING,(void**)&lighting,1,&nlighting);
	readLump(in,LUMP_ENTITIES,(void**)&entities,1,&nentities);

	#ifdef TOADLET_BIG_ENDIAN
		int i,j;
		for(i=0;i<nmodels;++i){
			bmodel &model=models[i];
			littleFloat(model.mins[0]);
			littleFloat(model.mins[1]);
			littleFloat(model.mins[2]);

			littleFloat(model.maxs[0]);
			littleFloat(model.maxs[1]);
			littleFloat(model.maxs[2]);

			littleFloat(model.origin[0]);
			littleFloat(model.origin[1]);
			littleFloat(model.origin[2]);

			for(j=0;j<MAX_MAP_HULLS;++j){
				littleInt32(model.headnode[j]);
			}

			littleInt32(model.visleafs);
			littleInt32(model.firstface);
			littleInt32(model.numfaces);
		}

		for(i=0;i<nvertexes;++i){
			bvertex &vertex=vertexes[i];
			littleFloat(vertex.point.x);
			littleFloat(vertex.point.y);
			littleFloat(vertex.point.z);
		}

		for(i=0;i<nplanes;++i){
			bplane &plane=planes[i];
			littleFloat(plane.dist);
			littleFloat(plane.normal.x);
			littleFloat(plane.normal.y);
			littleFloat(plane.normal.z);
			littleInt32(plane.type);
		}

		for(i=0;i<nleafs;++i){
			bleaf &leaf=leafs[i];
			littleInt32(leaf.contents);
			littleInt32(leaf.visofs)
			littleInt16(leaf.mins[0]);
			littleInt16(leaf.mins[1]);
			littleInt16(leaf.mins[2]);
			littleInt16(leaf.maxs[0]);
			littleInt16(leaf.maxs[1]);
			littleInt16(leaf.maxs[2]);
			littleUInt32(leaf.firstmarksurface);
			littleUInt32(leaf.nummarksurfaces);
		}

		for(i=0;i<nnodes;++i){
			bnode &node=nodes[i];
			littleInt32(node.planenum);
			littleInt16(node.children[0]);
			littleInt16(node.children[1]);
			littleInt16(node.mins[0]);
			littleInt16(node.mins[1]);
			littleInt16(node.mins[2]);
			littleInt16(node.maxs[0]);
			littleInt16(node.maxs[1]);
			littleInt16(node.maxs[2]);
			littleInt16(node.firstface);
			littleInt16(node.numfaces);
		}

		#error Need to convert rest of this
	#endif

	int i;
	BSPMap::ptr map(new BSPMap());

	map->vertexes.resize(nvertexes);
	memcpy(&map->vertexes[0],vertexes,nvertexes*sizeof(bvertex));

	map->planes.resize(nplanes);
	for(i=0;i<nplanes;++i){
		Plane &p=map->planes[i];
		bplane &pn=planes[i];

		p.normal.x=pn.normal[0];
		p.normal.y=pn.normal[1];
		p.normal.z=pn.normal[2];
		p.distance=pn.dist;
	}

	map->nodes.resize(nnodes);
	for(i=0;i<nnodes;++i){
		Node &n=map->nodes[i];
		bnode &bn=nodes[i];

		n.plane=bn.planenum;
		n.children[0]=bn.children[0];
		n.children[1]=bn.children[1];
		n.bound.mins[0]=bn.mins[0];
		n.bound.mins[1]=bn.mins[1];
		n.bound.mins[2]=bn.mins[2];
		n.bound.maxs[0]=bn.maxs[0];
		n.bound.maxs[1]=bn.maxs[1];
		n.bound.maxs[2]=bn.maxs[2];
	}

	map->leaves.resize(nleafs);
	for(i=0;i<nleafs;++i){
		Leaf &l=map->leaves[i];
		bleaf &bl=leafs[i];

		l.contents=bl.contents;
		l.visibilityStart=bl.visofs;
		l.bound.mins[0]=bl.mins[0];
		l.bound.mins[1]=bl.mins[1];
		l.bound.mins[2]=bl.mins[2];
		l.bound.maxs[0]=bl.maxs[0];
		l.bound.maxs[1]=bl.maxs[1];
		l.bound.maxs[2]=bl.maxs[2];
		l.marksurfaceStart=bl.firstmarksurface;
		l.marksurfaceCount=bl.nummarksurfaces;
	}

	map->faces.resize(nfaces);
	for(i=0;i<nfaces;++i){
		Face &f=map->faces[i];
		bface &bf=faces[i];

		f.plane=bf.planenum;
		f.side=bf.side>0;
		f.edgeStart=bf.firstedge;
		f.edgeCount=bf.numedges;
		f.texinfo=bf.texinfo;
	}

	map->trees.resize(nmodels);
	for(i=0;i<nmodels;++i){
		Tree &t=map->trees[i];
		bmodel &bm=models[i];

		t.bound.mins[0]=bm.mins[0];
		t.bound.mins[1]=bm.mins[1];
		t.bound.mins[2]=bm.mins[2];
		t.bound.mins[0]=bm.mins[0];
		t.bound.mins[1]=bm.mins[1];
		t.bound.mins[2]=bm.mins[2];
		t.nodeStart=bm.headnode[0];
		t.origin[0]=bm.origin[0];
		t.origin[1]=bm.origin[1];
		t.origin[2]=bm.origin[2];
		t.visleafs=bm.visleafs;
	}

	Logger::debug(Categories::TOADLET_TADPOLE,"Building brushes");

	Collection<Plane> workingPlanes,workingBrushPlanes;
	buildBrushes(map,map->trees[0].nodeStart,workingPlanes,workingBrushPlanes);

	// Hacky ones
	map->surfedges.resize(nsurfedges);
	memcpy(&map->surfedges[0],surfedges,nsurfedges*sizeof(int));

	map->edges.resize(nedges);
	memcpy(&map->edges[0],edges,nedges*sizeof(bedge));

	map->visibility.resize(nvisibility);
	memcpy(&map->visibility[0],visibility,nvisibility);

	map->marksurfaces.resize(nmarksurfaces);
	memcpy(&map->marksurfaces[0],marksurfaces,nmarksurfaces*sizeof(bmarksurface));

	map->texinfos.resize(ntexinfos);
	memcpy(&map->texinfos[0],texinfos,ntexinfos*sizeof(btexinfo));

	Logger::debug(Categories::TOADLET_TADPOLE,"Reading map finished ");

	return map;
}

void BSP30Handler::readLump(InputStream *in,int lump,void **data,int size,int *count){
	int length=header.lumps[lump].filelen;
	int ofs=header.lumps[lump].fileofs;

	in->seek(ofs);
	*data=malloc(length);
	in->read((char*)*data,length);

	if(count!=NULL){
		*count=length/size;
	}
}

void BSP30Handler::buildBrushes(BSPMap *map,int node,Collection<Plane> &planes,Collection<Plane> &brushPlanes){
	Plane plane=map->planes[map->nodes[node].plane];
	planes.add(plane);

	// Do back brush first, since the plane is facing the right direction
	if(map->nodes[node].children[1]<0){
		int leafIndex=-map->nodes[node].children[1]-1;

		findBrushPlanes(brushPlanes,planes);
		Brush brush;
		brush.contents=map->leaves[leafIndex].contents;
		brush.planeStart=map->planes.size();
		brush.planeCount=brushPlanes.size();

//		map->leaves[leafIndex].brushCount=1;
//		map->leaves[leafIndex].brushStart=map->brushes.size();
map->leaves[leafIndex].brushes.add(brush);
		map->planes.addAll(brushPlanes);
		map->brushes.add(brush);
	}
	else{
		buildBrushes(map,map->nodes[node].children[1],planes,brushPlanes);
	}

	Math::neg(planes[planes.size()-1]);

	// Now do front brush
	if(map->nodes[node].children[0]<0){
		int leafIndex=-map->nodes[node].children[0]-1;

		findBrushPlanes(brushPlanes,planes);
		Brush brush;
		brush.contents=map->leaves[leafIndex].contents;
		brush.planeStart=map->planes.size();
		brush.planeCount=brushPlanes.size();

//		map->leaves[leafIndex].brushCount=1;
//		map->leaves[leafIndex].brushStart=map->brushes.size();
map->leaves[leafIndex].brushes.add(brush);
		map->planes.addAll(brushPlanes);
		map->brushes.add(brush);
	}
	else{
		buildBrushes(map,map->nodes[node].children[0],planes,brushPlanes);
	}

	planes.removeAt(planes.size()-1);
}

void BSP30Handler::findBrushPlanes(Collection<Plane> &brushPlanes,const Collection<Plane> &planes){
	int i,j,k,l;
	float epsilon=mEpsilon;
	toadlet::egg::Collection<Vector3> points;
	Vector3 r;
	int numPlanes=planes.size();
	brushPlanes.clear();

	for(i=0;i<numPlanes-2;++i){
		for(j=i+1;j<numPlanes-1;++j){
			for(k=j+1;k<numPlanes;++k){
				if(Math::getIntersectionOfThreePlanes(r,planes[i],planes[j],planes[k],epsilon)){
					bool valid=true;
					for(l=0;l<numPlanes;++l){
						if(l!=i && l!=j && l!=k){
							const Plane &plane=planes[l];
							if((Math::dot(plane.normal,r)-plane.distance)>epsilon){
								valid=false;
								break;
							}
						}
					}

					if(valid){
						points.add(r);
					}
				}	
			}
		}
	}

	for(i=0;i<planes.size();++i){
		for(j=0;j<points.size();++j){
			if(Math::length(planes[i],points[j])<epsilon)break;
		}
		if(j!=points.size()){
			brushPlanes.add(planes[i]);
		}
	}


if(points.size()>0){
	AABox box(points[0],points[0]);
	for(j=1;j<points.size();++j){
		if(points[j].x<box.mins.x) box.mins.x=points[j].x;
		if(points[j].y<box.mins.y) box.mins.y=points[j].y;
		if(points[j].z<box.mins.z) box.mins.z=points[j].z;

		if(points[j].x>box.maxs.x) box.maxs.x=points[j].x;
		if(points[j].y>box.maxs.y) box.maxs.y=points[j].y;
		if(points[j].z>box.maxs.z) box.maxs.z=points[j].z;
	}

Logger::alert(String("box:")+box.mins.x+","+box.mins.y+","+box.mins.z+" : "+box.maxs.x+","+box.maxs.y+","+box.maxs.z);

	// Add beveling planes, super crude for now
//	planes.add(Plane(Math::X_UNIT_VECTOR3,box.mins.x));
//	planes.add(Plane(Math::Y_UNIT_VECTOR3,box.mins.y));
//	planes.add(Plane(Math::Z_UNIT_VECTOR3,box.mins.z));
//	planes.add(Plane(Math::NEG_X_UNIT_VECTOR3,-box.maxs.x));
//	planes.add(Plane(Math::NEG_Y_UNIT_VECTOR3,-box.maxs.y));
//	planes.add(Plane(Math::NEG_Z_UNIT_VECTOR3,-box.maxs.z));
}
else{
	Logger::alert("no points");
}

}

}
}
}

