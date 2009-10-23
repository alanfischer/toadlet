#include "HalfLifeBSP.h"
#include <toadlet/peeper/CapabilitySet.h>
#include <toadlet/peeper/TextureStage.h>
#include <toadlet/tadpole/mesh/MeshData.h>
#include <toadlet/egg/pset/XMLPropertySet.h>
#include <toadlet/egg/io/FileOutputStream.h>
#include <toadlet/egg/image/ImageUtilities.h>

#include "HalfLifeBSPMaterial.h"
#include <toadlet/peeper/plugins/glrenderer/GLTexturePeer.h>

#include <stdexcept>

using namespace toadlet;
using namespace toadlet::egg;
using namespace toadlet::egg::io;
using namespace toadlet::egg::pset;
using namespace toadlet::egg::image;
using namespace toadlet::egg::math;
using namespace toadlet::egg::math::Math;
using namespace toadlet::peeper;
using namespace toadlet::tadpole;
using namespace toadlet::tadpole::mesh;
using namespace toadlet::tadpole::material;

namespace hl{

class SimpleParser{
public:
	SimpleParser(const String &text){
		mDelimiters[0]='\t';
		mDelimiters[1]=13;
		mDelimiters[2]=' ';
		mDelimiters[3]=10;
		mDelimiters[4]=0;

		mData=new char[text.length()+2];
		strcpy(mData,text.c_str());
		strcat(mData," ");
		mText=mData;
		mEnd=mData+strlen(mData);
	}

	~SimpleParser(){
		delete mData;
	}

	char *getNextToken(){
		if(mText==NULL){
			return NULL;
		}

		char *next=NULL,*temp=NULL,*token=NULL;
		bool parse=true;

		do{
			next=NULL;
			temp=NULL;

			if(mText>=mEnd){
				parse=false;
				token=NULL;
			}
			else if(*mText=='\"'){
				*mText=0;

				temp=mText+1;
				mText=strchr(temp,'\"');

				*mText=0;
				mText++;

				parse=false;
				token=temp;
			}

			else{
				int i;
				char *start;

				start=temp+1;

				next=0;

				temp=mText;

				while(*temp!=0 && next==0){
					for(i=0;i<(int)strlen(mDelimiters);i++){
						if(mDelimiters[i]==*temp){
							next=temp;
							break;
						}
					}

					temp++;
				}
				
				if(next==0){
					return 0;
				}

				*next=0;
				temp=mText;
				mText=next+1;

				start=mText;
				
				if(start==temp+1){
					parse=true;
				}
				else{
					parse=false;
					token=temp;
				}
			}
		}while(parse==true);

		return token;
	}

protected:
	char mDelimiters[10];

	char *mData;
	char *mText;
	char *mEnd;
};

template<class Type> void readLump(lump_t &lump,InputStream *in,Collection<Type> &collection){
	int length=lump.filelen;
	if(length%sizeof(Type)){
		throw std::runtime_error("Odd lump size");
	}

	int num=length/sizeof(Type);

	int ofs=lump.fileofs;

	in->reset();
	if(ofs>0){
		char *temp=new char[ofs];
		int amt=in->read(temp,ofs);
		delete[] temp;
	}

	collection.resize(num);
	in->read((char*)&collection[0],num*sizeof(Type));
}

HalfLifeBSP::HalfLifeBSP(){
}

HalfLifeBSP::~HalfLifeBSP(){
}

void HalfLifeBSP::load(Engine *engine,const String &level){
	InputStream *in=engine->makeInputStream(level);
	if(in==0){
		throw std::runtime_error("Unable to open BSP file");
	}

	int version=0;
	in->read((char*)&version,sizeof(int));
	in->reset();

	lump_t *lumps=NULL;
	dheader_t header;

	if(version==BSPVERSION){
		in->read((char*)&header,sizeof(header));
		lumps=header.lumps;
	}
	else{
		printf("Header version=%d\n",version);
		throw std::runtime_error("BSP version is incorrect");
	}

	readLump(lumps[LUMP_MODELS],in,models);
	readLump(lumps[LUMP_VERTEXES],in,vertexes);
	readLump(lumps[LUMP_PLANES],in,planes);
	readLump(lumps[LUMP_LEAFS],in,leafs);
	readLump(lumps[LUMP_NODES],in,nodes);
	readLump(lumps[LUMP_TEXINFO],in,texInfos);
	readLump(lumps[LUMP_CLIPNODES],in,clipNodes);
	readLump(lumps[LUMP_FACES],in,faces);
	readLump(lumps[LUMP_MARKSURFACES],in,markSurfaces);
	readLump(lumps[LUMP_SURFEDGES],in,surfEdges);
	readLump(lumps[LUMP_EDGES],in,edges);
	readLump(lumps[LUMP_TEXTURES],in,texData);
	readLump(lumps[LUMP_VISIBILITY],in,visData);
	readLump(lumps[LUMP_LIGHTING],in,lightData);
	readLump(lumps[LUMP_ENTITIES],in,entData);

	#ifdef TOADLET_BIG_ENDIAN
		int i,j;
		for(i=0;i<models.size();++i){
			dmodel_t &model=models[i];
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

		for(i=0;i<vertexes.size();++i){
			dvertex_t &vertex=vertexes[i];
			littleFloat(vertex.point.x);
			littleFloat(vertex.point.y);
			littleFloat(vertex.point.z);
		}

		for(i=0;i<planes.size();++i){
			dplane_t &plane=planes[i];
			littleFloat(plane.dist);
			littleFloat(plane.normal.x);
			littleFloat(plane.normal.y);
			littleFloat(plane.normal.z);
			littleInt32(plane.type);
		}

		for(i=0;i<leafs.size();++i){
			dleaf_t &leaf=leafs[i];
			littleFloat(plane.dist);
			littleFloat(plane.normal.x);
			littleFloat(plane.normal.y);
			littleFloat(plane.normal.z);
			littleInt32(plane.type);
		}

		for(i=0;i<nodes.size();++i){
			dnode_t &node=nodes[i];
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

		for(i=0;i<nodes.size();++i){
			dnode_t &node=nodes[i];
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
	#endif

	delete in;
	in=NULL;

	decompressVIS();

	int vertCount=0;
	int i;
	for(i=0;i<faces.size();i++){
		const dface_t &face=faces[i];
		vertCount+=face.numedges;
	}
	mVertexBuffer=engine->getVertexBufferManager()->load(new VertexBuffer());
	mVertexBuffer->setStatic(true);
	mVertexBuffer->resize((VertexBuffer::VertexType)(VertexBuffer::VT_POSITION|VertexBuffer::VT_NORMAL|VertexBuffer::VT_TEXCOORD1_DIM2|VertexBuffer::VT_TEXCOORD2_DIM2),vertCount);
	mRenderFaces.resize(faces.size());

	Collection<IndexBuffer*> indexBuffers(faces.size());
	Collection<Primitive> primitives(faces.size());

	vertCount=0;
	for(i=0;i<faces.size();i++){
		const dface_t &face=faces[i];

		mRenderFaces[i].indexes.resize(face.numedges);
		int j;
		for(j=0;j<face.numedges;j++){
			int t=surfEdges[face.firstedge+j];

			int vert=0;
			if(t<0){
				vert=edges[-t].v[1];
			}
			else{
				vert=edges[t].v[0];
			}

			mVertexBuffer->position(vertCount+j)=Vector3(vertexes[vert].point[0],vertexes[vert].point[1],vertexes[vert].point[2]);
			mVertexBuffer->normal(vertCount+j)=*(Vector3*)planes[face.planenum].normal;
			if(face.side){
				mVertexBuffer->normal(vertCount+j)*=-1;
			}
			mRenderFaces[i].indexes.index(face.numedges-j-1)=vertCount+j;
		}

		const texinfo_t &texinfo=texInfos[face.texinfo];
		int width=0,height=0;
		dmiptexlump_t *mipTexLump=(dmiptexlump_t*)&texData[0];
		int miptexofs=mipTexLump->dataofs[texinfo.miptex];
		if(miptexofs!=-1){
			miptex_t *mipTex=(miptex_t*)(&texData[miptexofs]);
			width=mipTex->width;
			height=mipTex->height;
		}
		Vector2 mins,maxs;
		calculateSurfaceExtents(&face,mins,maxs);

		Image *lightmap=NULL;
		unsigned int lmwidth=0,lmheight=0;

		if((texinfo.flags&TEX_SPECIAL)==0){
			lightmap=computeLightmap(&face,mins,maxs);
			lightmap->getSize(lmwidth,lmheight);

			unsigned int newlmwidth=lmwidth,newlmheight=lmheight;
			if(isPowerOf2(newlmwidth)==false){
				newlmwidth=nextPowerOf2(newlmwidth);
			}
			if(isPowerOf2(newlmheight)==false){
				newlmheight=nextPowerOf2(newlmheight);
			}
/*
			if(lmwidth!=newlmwidth || lmheight!=newlmheight){
				Image *newLightmap=new Image(lightmap->getDimension(),lightmap->getFormat(),lightmap->getType(),newlmwidth,newlmheight);
				ImageUtilities::scaleImage(lightmap,newLightmap,false);
				delete lightmap;
				lightmap=newLightmap;
			}
*/
			Texture *lmt=new Texture(lightmap);
			lmt->setMipMap(TMM_NO);
			lmt->setClamp(TC_CLAMP_X|TC_CLAMP_Y);
			mRenderFaces[i].lightmap=engine->getTextureManager()->load(lmt);
		}

		if(width>0 && height>0){
			float is=1.0f/(float)width;
			float it=1.0f/(float)height;

			for(j=0;j<face.numedges;j++){
				float s=dot(mVertexBuffer->position(vertCount+j),*(Vector3*)texinfo.vecs[0]) + texinfo.vecs[0][3];
				float t=dot(mVertexBuffer->position(vertCount+j),*(Vector3*)texinfo.vecs[1]) + texinfo.vecs[1][3];
 
				mVertexBuffer->texCoord2d(vertCount+j)=Vector2(s*is,t*it);

				if(lightmap!=NULL){
					// compute lightmap coords
					float mid_poly_s = (mins[0] + maxs[0])/2.0f;
					float mid_poly_t = (mins[1] + maxs[1])/2.0f;
					float mid_tex_s = (float)lmwidth / 2.0f;
					float mid_tex_t = (float)lmheight / 2.0f;
					float ls = mid_tex_s + (s - mid_poly_s) / 16.0f;
					float lt = mid_tex_t + (t - mid_poly_t) / 16.0f;
					ls /= (float)lmwidth;
					lt /= (float)lmheight;

					mVertexBuffer->texCoord2d(vertCount+j,1)=Vector2(ls,lt);
				}
			}
		}

		vertCount+=face.numedges;

		indexBuffers[i]=&mRenderFaces[i].indexes;
		primitives[i]=PRIMITIVE_TRIFAN;
	}

//	MeshData::calculateTangentVectorsForBuffers(mVertexBuffer,faces.size(),&indexBuffers[0],&primitives[0],0.0001f);

	mRendererData.markedFaces.resize((faces.size()+7)/8);

	SimpleParser parser((char*)&entData[0]);

	XMLPropertySet *current=0;
	char *token=0;
	for(token=parser.getNextToken();token!=0;token=parser.getNextToken()){
		if(strcmp(token,"{")==0){
			current=new XMLPropertySet();
		}
		else if(strcmp(token,"}")==0){
			entities.push_back(current);
		}
		else{
			current->setProperty(token,parser.getNextToken());
		}
	}
}

template<class Type> void calcLumpSize(lump_t &lump,Collection<Type> &collection,int &size){
	int amt=sizeof(Type)*collection.size();

	lump.fileofs=size;
	lump.filelen=amt;
	size+=amt;
}

template<class Type> void writeLump(lump_t &lump,OutputStream *out,Collection<Type> &collection){
	out->write((char*)&collection[0],sizeof(Type)*collection.size());
}

void HalfLifeBSP::save(const String &level,int version){
	FileOutputStream *out=new FileOutputStream(level);
	if(out->isOpen()==false){
		delete out;
		throw std::runtime_error("Unable to open BSP file");
	}

	lump_t *lumps=NULL;
	dheader_t header;
	int size=0;

	if(version==BSPVERSION){
		header.version=BSPVERSION;
		lumps=header.lumps;
		size=sizeof(header);
	}

	calcLumpSize(lumps[LUMP_MODELS],models,size);
	calcLumpSize(lumps[LUMP_VERTEXES],vertexes,size);
	calcLumpSize(lumps[LUMP_PLANES],planes,size);
	calcLumpSize(lumps[LUMP_LEAFS],leafs,size);
	calcLumpSize(lumps[LUMP_NODES],nodes,size);
	calcLumpSize(lumps[LUMP_TEXINFO],texInfos,size);
	calcLumpSize(lumps[LUMP_CLIPNODES],clipNodes,size);
	calcLumpSize(lumps[LUMP_FACES],faces,size);
	calcLumpSize(lumps[LUMP_MARKSURFACES],markSurfaces,size);
	calcLumpSize(lumps[LUMP_SURFEDGES],surfEdges,size);
	calcLumpSize(lumps[LUMP_EDGES],edges,size);
	calcLumpSize(lumps[LUMP_TEXTURES],texData,size);
	calcLumpSize(lumps[LUMP_VISIBILITY],visData,size);
	calcLumpSize(lumps[LUMP_LIGHTING],lightData,size);
	calcLumpSize(lumps[LUMP_ENTITIES],entData,size);

	if(version==BSPVERSION){
		out->write((char*)&header,sizeof(header));
	}

	writeLump(lumps[LUMP_MODELS],out,models);
	writeLump(lumps[LUMP_VERTEXES],out,vertexes);
	writeLump(lumps[LUMP_PLANES],out,planes);
	writeLump(lumps[LUMP_LEAFS],out,leafs);
	writeLump(lumps[LUMP_NODES],out,nodes);
	writeLump(lumps[LUMP_TEXINFO],out,texInfos);
	writeLump(lumps[LUMP_CLIPNODES],out,clipNodes);
	writeLump(lumps[LUMP_FACES],out,faces);
	writeLump(lumps[LUMP_MARKSURFACES],out,markSurfaces);
	writeLump(lumps[LUMP_SURFEDGES],out,surfEdges);
	writeLump(lumps[LUMP_EDGES],out,edges);
	writeLump(lumps[LUMP_TEXTURES],out,texData);
	writeLump(lumps[LUMP_VISIBILITY],out,visData);
	writeLump(lumps[LUMP_LIGHTING],out,lightData);
	writeLump(lumps[LUMP_ENTITIES],out,entData);

	delete out;
	out=NULL;
}

void HalfLifeBSP::setMaterials(const Collection<Material::Ptr> &materials){
	if(materials.size()!=((dmiptexlump_t*)&texData[0])->nummiptex){
		throw std::runtime_error("Incorrect number of materials");
	}

	this->materials=materials;
	mRendererData.materialVisibleFaces.resize(materials.size());
}

// TODO: I believe the majority of the VIS decompression could be done at runtime?  I thought I remembered doing that on
// a q2 viewer
void HalfLifeBSP::decompressVIS(){
	int count;
	int i,c,index;
	unsigned char bit;

	if(visData.size()==0){
		return; // No vis data to decompress
	}

	leafVisibility.resize(leafs.size());

	for(i=0;i<leafs.size();i++){
		int v=leafs[i].visofs;

		count=0;

		// first count the number of visible leafs...
		for(c=1;c<models[0].visleafs;v++){
			if(visData[v]==0){
				v++;
				c+=(visData[v]<<3);
			}
			else{
				for (bit = 1; bit; bit<<=1,c++){
					if(visData[v]&bit){
						count++;
					}
				}
			}
		}

		if(count>0){
			// allocate space to store the uncompressed VIS bit set...
			leafVisibility[i].leafs=new int[count];

			if(leafVisibility[i].leafs==NULL){
				throw std::runtime_error("Error allocating memory for leaf visibility!");
			}
		}
		else{
			leafVisibility[i].leafs=0;
		}

		leafVisibility[i].num_leafs=count;
	}

	// now go through the VIS bit set again and store the VIS leafs...
	for(i=0;i<leafs.size();i++){
		int v=leafs[i].visofs;

		index=0;

		for(c=1;c<models[0].visleafs;v++){
			if(visData[v]==0){
				v++;
				c+=(visData[v]<<3);
			}
			else{
				for(bit=1;bit;bit<<=1,c++){
					if(visData[v] & bit){
						if(leafVisibility[i].leafs!=0){
							leafVisibility[i].leafs[index]=c;
						}
						index++;
					}
				}
			}
		}
	}
}

void HalfLifeBSP::calculateSurfaceExtents(const dface_t *s,Vector2 &mins,Vector2 &maxs){
	float val;
	int i,j,e;
	dvertex_t *v;
	texinfo_t *tex;

	mins[0]=mins[1]=999999;
	maxs[0]=maxs[1]=-999999;

	tex=&texInfos[s->texinfo];

	for(i=0;i<s->numedges;i++){
		e=surfEdges[s->firstedge + i];
		if(e>=0){
			v=&vertexes[edges[e].v[0]];
		}
		else{
			v=&vertexes[edges[-e].v[1]];
		}

		for(j=0;j<2;j++){
			val=v->point[0] * tex->vecs[j][0] +
				v->point[1] * tex->vecs[j][1] +
				v->point[2] * tex->vecs[j][2] +
				tex->vecs[j][3];
			if(val<mins[j]){
				mins[j]=val;
			}
			if(val>maxs[j]){
				maxs[j]=val;
			}
		}
	}
}

#define TEXTURE_SIZE 16

Image *HalfLifeBSP::computeLightmap(const dface_t *face,const Vector2 &mins,const Vector2 &maxs){
	if(lightData.size()==0){return new Image();}

	int c;
	int width, height;

	// compute lightmap size
	int size[2];
	for(c=0;c<2;c++){
		float tmin=(float)floor(mins[c]/TEXTURE_SIZE);
		float tmax=(float)ceil(maxs[c]/TEXTURE_SIZE);
		size[c]=(int)(tmax-tmin);
	}

	width=size[0]+1;
	height=size[1]+1;
	int lsz=width*height*3;  // RGB buffer size

	// generate lightmaps texture
	Image *image=new Image();

//	for(c=0;c<1;c++){
		//if(face->styles[c]==-1)break;
		//face->styles[c]=dface->styles[c];

	image->reallocate(Image::DIMENSION_2D,Image::FORMAT_RGB,Image::TYPE_BYTE,width,height);

	memcpy(image->getData(),&lightData[face->lightofs],lsz);

	// Line below is valid for when there are multiple lightmaps on this face, then c would be that index.
	// Need to check into how there can be multiple lightmaps
	//memcpy(image->data,&mLightData[face->lightofs+(lsz*c)],lsz);
//	}

//    face->lightmap = face->lightmaps[0];
	return image;
}

int HalfLifeBSP::findLeaf(const Vector3 &coords) const{
	int i=models[0].headnode[0];

	while(i>=0){
		const dnode_t *n=&nodes[i];
		const dplane_t *p=&planes[n->planenum];

		float d;

		// TODO: Good point!  Keep this in mind?
		if(p->type<=PLANE_Z){  // easier for axial planes
			d=coords[p->type]-p->dist;
		}
		else{
			// f(x,y,z) = Ax+Ay+Az-D
			d=p->normal[0]*coords[0]
				+p->normal[1]*coords[1]
				+p->normal[2]*coords[2]
				-p->dist;
		}

		if(d>=0){  // in front or on the plane
			i=n->children[0];
		}
		else{  // behind the plane
			i=n->children[1];
		}
	}

	return -(i+1);
}

void HalfLifeBSP::processVisibleFaces(Renderer *r,Frustum *f) const{
	int i;
	RendererData &data=(RendererData&)mRendererData;

	// TODO: Instead of this calculation here and then again in the AABox testing, transform frustum to local space
	// TODO: Matrix4x4 shortcut
	Vector4 temp(f->origin,1);
	//temp=mInvTransformation*temp;
	Vector3 cam(temp.x,temp.y,temp.z);

	// clear stuff
	memset(&data.markedFaces[0],0,data.markedFaces.size()*sizeof(unsigned char));
	for(i=0;i<data.materialVisibleFaces.size();++i){
		if(data.materialVisibleFaces[i].size()>0){
			data.materialVisibleFaces[i].clear();
		}
	}

	// find leaf we're in
	int idx=findLeaf(cam);

	// If no visibility information just test all leaves
	if(idx==0 || visData.size()==0){ // TODO: Replace with an mHasVisData
		for(i=0;i<leafs.size();i++){
			addLeafToVisible(leafs[i],data,cam,f);
		}
	}
	else{
		const vis_leaf_t &vis_leaf=leafVisibility[idx];
		// go thru leaf visibility list
		for(i=0;i<vis_leaf.num_leafs;i++){
			addLeafToVisible(leafs[vis_leaf.leafs[i]],data,cam,f);
		}
	}
}

void HalfLifeBSP::renderVisibleFaces(Engine *e,Renderer *r,Frustum *f) const{
	int i,j,k;
	RendererData &data=(RendererData&)mRendererData;

	TextureStage lightmapStage;
	lightmapStage.setTexCoordIndex(1);
	lightmapStage.setBlend(TextureBlend(TBO_MODULATE,TBS_PREVIOUS,TBS_TEXTURE));
/*
	for(i=0;i<data.materialVisibleFaces.size();i++){
		const Collection<int> &visibleFaces=data.materialVisibleFaces[i];

		if(visibleFaces.size()>0){
			MaterialRenderer *materialRenderer=e->getMaterialRenderer(materials[i]->getMaterialIdentifierIndex());

			materialRenderer->startRendering(r);

			int numPasses=materialRenderer->getNumRenderPasses(materials[i]);

			for(j=0;j<numPasses;++j){
				materialRenderer->preRenderPass(materials[i],j,r);

				for(k=0;k<visibleFaces.size();++k){
					int vf=visibleFaces[k];

					lightmapStage.setTexture(mRenderFaces[vf].lightmap);
					r->setTextureStage(1,lightmapStage);

					r->renderPrimitive(PRIMITIVE_TRIFAN,mVertexBuffer,&mRenderFaces[vf].indexes);
				}

				materialRenderer->postRenderPass(materials[i],j,r);
			}

			materialRenderer->stopRendering(r);
		}
	}
*/

	static b=false;if(b==false){b=true;glewInit();}

	for(i=0;i<data.materialVisibleFaces.size();i++){
		const Collection<int> &visibleFaces=data.materialVisibleFaces[i];

		if(visibleFaces.size()>0){
glActiveTexture(GL_TEXTURE0_ARB+0);
glClientActiveTexture(GL_TEXTURE0_ARB+0);
glEnable(GL_TEXTURE_2D);
HalfLifeBSPMaterial *m=(HalfLifeBSPMaterial*)materials[i].get();
Texture *t=m->getDiffuseTexture();
if(t!=NULL){
GLTexturePeer *p=(GLTexturePeer*)t->getTexturePeer();
glBindTexture(GL_TEXTURE_2D,p->textureHandle);
}
else{
glBindTexture(GL_TEXTURE_2D,0);
}
for(k=0;k<visibleFaces.size();++k){
int vf=visibleFaces[k];

glActiveTexture(GL_TEXTURE0_ARB+1);
glClientActiveTexture(GL_TEXTURE0_ARB+1);
glEnable(GL_TEXTURE_2D);
glBindTexture(GL_TEXTURE_2D,((GLTexturePeer*)mRenderFaces[vf].lightmap->getTexturePeer())->textureHandle);

//r->renderPrimitive(PRIMITIVE_TRIFAN,mVertexBuffer,&mRenderFaces[vf].indexes);
glBegin(GL_TRIANGLE_FAN);
for(j=0;j<mRenderFaces[vf].indexes.getNumIndexes();++j){
int index=mRenderFaces[vf].indexes.index(j);
glMultiTexCoord2f(0,mVertexBuffer->texCoord2d(index,0).x,mVertexBuffer->texCoord2d(index,0).y);
glMultiTexCoord2f(1,mVertexBuffer->texCoord2d(index,1).x,mVertexBuffer->texCoord2d(index,1).y);
glVertex3f(mVertexBuffer->position(index).x,mVertexBuffer->position(index).y,mVertexBuffer->position(index).z);
}
glEnd();

}
}
	}


glActiveTexture(GL_TEXTURE0_ARB+1);
glClientActiveTexture(GL_TEXTURE0_ARB+1);
glDisable(GL_TEXTURE_2D);
glActiveTexture(GL_TEXTURE0_ARB+0);
glClientActiveTexture(GL_TEXTURE0_ARB+0);

}

void HalfLifeBSP::addLeafToVisible(const dleaf_t &leaf,RendererData &data,const Vector3 &cam,Frustum *f) const{
	// discard leafs outside frustum
	if(testIntersection(leaf,f)){
		const unsigned short* p=&markSurfaces[leaf.firstmarksurface];

		int x;
		for(x=0;x<leaf.nummarksurfaces;x++){
			// don't render those already rendered
			int face_idx=*p++;

			if(!(data.markedFaces[face_idx>>3] & (1<<(face_idx & 7)))){
				// back face culling
				const dface_t &f=faces[face_idx];

				// The planes should be converted to our type
				float d=dot(cam,*(Vector3*)planes[f.planenum].normal) - planes[f.planenum].dist;

				if(f.side){
					if(d>0) continue;
				}
				else{
					if(d<0) continue;
				}

				// mark face as visible
				data.markedFaces[face_idx>>3] |= (1<<(face_idx & 7));

				const texinfo_t &texinfo=texInfos[f.texinfo];

				if((texinfo.flags&TEX_SPECIAL)==0){
					data.materialVisibleFaces[texinfo.miptex].push_back(face_idx);
				}
/*
				// chose lightmap style
				f->lightmap = f->lightmaps[0];
				if(current_style != 0) {
					for(int c = 0; c < MAXLIGHTMAPS; c++) {
						if(f->styles[c] == current_style) {
							f->lightmap = f->lightmaps[c];
							break;
						}
					}
				}
*/			}
		}
	}
}

bool HalfLifeBSP::testIntersection(const dleaf_t &leaf,Frustum *f) const{
	Vector3 mins,maxs;

	Vector4 temp(leaf.mins[0],leaf.mins[1],leaf.mins[2],1);
	mins=Vector3(temp.x,temp.y,temp.z);
	temp=Vector4(leaf.maxs[0],leaf.maxs[1],leaf.maxs[2],1);
	maxs=Vector3(temp.x,temp.y,temp.z);

	return f->testIntersection(AABox(mins,maxs),false);
}

}
 
