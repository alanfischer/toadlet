#ifndef HL_HALFLIFEBSP_H
#define HL_HALFLIFEBSP_H

#include <toadlet/egg/Collection.h>
#include <toadlet/egg/pset/PropertySet.h>
#include <toadlet/tadpole/Engine.h>
#include "HalfLifeBSPDefines.h"

namespace hl{

class HalfLifeBSP{
public:
	HalfLifeBSP();
	~HalfLifeBSP();

	void load(toadlet::tadpole::Engine *engine,const toadlet::egg::String &level);
	void save(const toadlet::egg::String &level,int bspVersion);

	void processVisibleFaces(toadlet::peeper::Renderer *r,toadlet::peeper::Frustum *f) const;
	void renderVisibleFaces(toadlet::tadpole::Engine *e,toadlet::peeper::Renderer *r,toadlet::peeper::Frustum *f) const;

	void setMaterials(const toadlet::egg::Collection<toadlet::tadpole::material::Material::Ptr> &materials);

	toadlet::egg::Collection<dmodel_t> models;
	toadlet::egg::Collection<dvertex_t> vertexes;
	toadlet::egg::Collection<dplane_t> planes;
	toadlet::egg::Collection<dleaf_t> leafs;
	toadlet::egg::Collection<dnode_t> nodes;
	toadlet::egg::Collection<dface_t> faces;
	toadlet::egg::Collection<dedge_t> edges;
	toadlet::egg::Collection<dclipnode_t> clipNodes;
	toadlet::egg::Collection<unsigned char> texData;
	toadlet::egg::Collection<unsigned char> visData;
	toadlet::egg::Collection<unsigned char> lightData;
	toadlet::egg::Collection<unsigned char> entData;
	toadlet::egg::Collection<texinfo_t> texInfos;
	toadlet::egg::Collection<unsigned short> markSurfaces;
	toadlet::egg::Collection<int> surfEdges;
	toadlet::egg::Collection<toadlet::egg::pset::PropertySet*> entities;
	toadlet::egg::Collection<vis_leaf_t> leafVisibility;
	toadlet::egg::Collection<toadlet::tadpole::material::Material::Ptr> materials;

//protected:
	class RendererData{
	public:
		toadlet::egg::Collection<unsigned char> markedFaces;
		toadlet::egg::Collection<toadlet::egg::Collection<int> > materialVisibleFaces;
	};

	class RenderFace{
	public:
		toadlet::peeper::IndexBuffer indexes;
		toadlet::peeper::Texture::Ptr lightmap;
	};

	void decompressVIS();
	int findLeaf(const toadlet::egg::math::Vector3 &coords) const;
	bool testIntersection(const dleaf_t &leaf,toadlet::peeper::Frustum *f) const;
	void addLeafToVisible(const dleaf_t &leaf,RendererData &data,const toadlet::egg::math::Vector3 &cam,toadlet::peeper::Frustum *f) const;
	void loadTextures();
	void calculateSurfaceExtents(const dface_t *s,toadlet::egg::math::Vector2 &mins,toadlet::egg::math::Vector2 &maxs);
	toadlet::egg::image::Image *computeLightmap(const dface_t *face,const toadlet::egg::math::Vector2 &mins,const toadlet::egg::math::Vector2 &maxs);

	RendererData mRendererData;
	toadlet::egg::Collection<RenderFace> mRenderFaces;
	toadlet::peeper::VertexBuffer::Ptr mVertexBuffer;
};

}

#endif
