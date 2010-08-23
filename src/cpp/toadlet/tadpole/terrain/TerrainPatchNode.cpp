/********** Copyright header - do not remove **********
 *
 * The Toadlet Engine 
 *
 * Copyright (C) 2005, Lightning Toads Productions, LLC
 *
 * Author(s): Alan Fischer
 * 
 * All source code for the Toadlet Engine, including 
 * this file, is the sole property of Lightning Toads 
 * Productions, LLC. It has been developed on our own 
 * time, with our own tools, on our own hardware. None
 * of this code may be observed, used or changed 
 * without our explicit permission. 
 *
 *  - Lightning Toads Productions, LLC
 *
 ********** Copyright header - do not remove **********/

#include <toadlet/tadpole/terrain/TerrainPatchNode.h>

using namespace toadlet::egg;
using namespace toadlet::peeper;
using namespace toadlet::tadpole;

namespace toadlet{
namespace tadpole{
namespace terrain{

TerrainPatch::TerrainPatch(Engine *engine){
	mEngine=engine;

	mMinTolerance=0;
	mMaxTolerance=1;

	mS1=0.5;
	mS2=1;

	mSize=0;

	mEngine->addRendererListener(this);
}

TerrainPatch::~TerrainPatch(){
	mEngine->removeRendererListener(this);
}

void TerrainPatch::load(TerrainData *data){
	if(data->width!=data->height){
		throw std::runtime_error("width!=height");
	}

	mSize=0;
	mSizeN=0;

	for(mSize=1;mSize<=data->width;mSize<<=1)mSizeN++;

	mSizeN--;
	mSize>>=1;
	mSize++;

	if(mSize!=data->width){
		throw std::runtime_error("width & height not a power of 2 + 1");
	}

	int squareSize=square(mSize);

	// Load height data
	mTerrainVertexTypes.resize(squareSize);
	mTerrainVertexes.resize(squareSize);

	mVertexBuffer=mEngine->getVertexBufferManager()->load(new VertexBuffer());
	mVertexBuffer->resize(VertexBuffer::VertexType(VertexBuffer::VT_POSITION|VertexBuffer::VT_NORMAL|VertexBuffer::VT_TEXCOORD1_DIM2),squareSize);

	mIndexBuffer=mEngine->getIndexBufferManager()->load(new IndexBuffer());
	mIndexBuffer->setStatic(false);

	int i,j;
	for(i=0;i<mSize;++i){
		for(j=0;j<mSize;++j){
			int index=j*mSize+i;

			vertex(i,j)->index=index;

			float height=data->heightData[index];

			position(i,j).z=height;
			position(i,j).x=i;
			position(i,j).y=j;

			texCoord(i,j).x=(float)i/(float)(mSize)+.5/(float)mSize;
			texCoord(i,j).y=(float)j/(float)(mSize)+.5/(float)mSize;

			// TODO: Set type
			// terrainVertexType
		}
	}

	// Build blocks
	int numBlocks=1;
	for(i=0;i<mSizeN;i++){
		numBlocks*=4;
	}
	numBlocks=(int)((numBlocks-1)/3) + 1;

	mBlocks.resize(numBlocks);

	mBlockQueueSize=numBlocks;
	mBlockQueue.resize(mBlockQueueSize);

	mNumBlocksInQueue=0;
	mBlockQueueStart=0;
	mBlockQueueEnd=0;

	for(i=0;i<mSize;i++){
		for(j=0;j<mSize;j++){
			if(i>0 && j>0){
				cross(normal(i,j),position(i-1,j)-position(i,j),position(i,j-1)-position(i,j));
				normalize(normal(i,j));
			}
		}
	}

	for(i=0;i<mSize;i++){
		normal(i,0)=normal(i,1);
	}

	for(j=0;j<mSize;j++){
		normal(0,j)=normal(1,j);
	}

	int numInitBlocks=1;
	for(i=0;i<mSizeN-1;i++){
		numInitBlocks*=2;
	}
	initBlocks(&mBlocks[0],0,0,0,numInitBlocks,true);

	addBlockToBack(0);
}

void TerrainPatch::updateBlocks(const Vector3 &position,const Vector3 &direction){
	mPosition=position;
	mDirection=direction;

	mTolerance=lerp(mMaxTolerance,mMinTolerance,0.5f);

	resetBlocks();
	simplifyBlocks();
	simplifyVertexes();
}

void TerrainPatch::updateIndexBuffers(Renderer *renderer,Frustum *frustum){
	IndexBuffer *indexes=mRendererData.find(renderer)->second.indexBuffer;
	indexes->resize(square(mSize-1)*6);

	int indexCount=gatherBlocks(frustum,indexes);
	indexes->resize(indexCount);
}

void TerrainPatch::render(Renderer *renderer) const{
#if 1
	{
		// TODO: Introduce this to multitexturing :D

//		renderer->setTextureState(TextureState(mTerrainTexture));
		renderer->renderPrimitive(PRIMITIVE_TRIS,mVertexBuffer,indexes);

//		renderer->setTextureMatrix(
//		renderer->setTextureState(TextureState(mDetailTexture));
//		renderer->setBlendFunction(BC_ALPHA);
//		renderer->setColor(Vector4(1,1,1,0.3f));

//		renderer->renderPrimitive(PRIMITIVE_TRIS,mVertexBuffer,indexes);

//		renderer->setTextureState(Math::IDENTITY_MATRIX4X4);
	}
#else
	{
		mVertexBuffer->mColors.clear();

		renderer->setRenderState(TextureRenderState(terrainTexture));
		renderer->renderTriangleList(mVertexBuffer,mIndexBuffer);

		renderer->pushRenderStates();
		renderer->setRenderState(BlendRenderState(BC_ALPHA));

		renderer->pushMatrix(RendererI::MATRIX_TEXTURE);
		renderer->multMatrix(RendererI::MATRIX_TEXTURE,Matrix4x4::makeMatrixFromScale(getWorldScale()));

		mVertexBuffer->mColors.resize(size*size,Vector4(1,1,1,1));

		for(j=0;j<numDetailTextures;j++){
			mDetailIndexBuffer->mIndexes.clear();

			for(i=0;i<mIndexBuffer->mIndexes.size();i+=3){
				float t1=.5f;
				float t2=.5f;
				float t3=.5f;

				if(terrainVertexTypes[mIndexBuffer->mIndexes[i+0]]!=j)t1=0;
				if(terrainVertexTypes[mIndexBuffer->mIndexes[i+1]]!=j)t2=0;
				if(terrainVertexTypes[mIndexBuffer->mIndexes[i+2]]!=j)t3=0;

				if(t1>0 || t2>0 || t3>0){
					mDetailIndexBuffer->mIndexes.push_back(mIndexBuffer->mIndexes[i+0]);
					mDetailIndexBuffer->mIndexes.push_back(mIndexBuffer->mIndexes[i+1]);
					mDetailIndexBuffer->mIndexes.push_back(mIndexBuffer->mIndexes[i+2]);

					mVertexBuffer->mColors[mIndexBuffer->mIndexes[i+0]]=Vector4(1,1,1,t1);
					mVertexBuffer->mColors[mIndexBuffer->mIndexes[i+1]]=Vector4(1,1,1,t2);
					mVertexBuffer->mColors[mIndexBuffer->mIndexes[i+2]]=Vector4(1,1,1,t3);
				}
			}

			renderer->setRenderState(TextureRenderState(detailTextures[j]));
			renderer->renderTriangleList(mVertexBuffer,mDetailIndexBuffer);
		}

		renderer->popMatrix(RendererI::MATRIX_TEXTURE);
		renderer->popRenderStates();
	}
#endif
}

void TerrainPatch::stitchToRight(TerrainPatch *terrain){
	TerrainPatch *tLeft=this;
	TerrainPatch *tRight=terrain;
	int row;

	if(tLeft->getSizeN()!=tRight->getSizeN()){
		throw std::runtime_error("left->sizeN!=right->sizeN");
	}

	for(row=1;row<mSize-1;row++){
		tRight->vertex(0,row)->dependent0=tLeft->vertex(mSize-1,row);
		tLeft->vertex(mSize-1,row)->dependent1=tRight->vertex(0,row);
	}

	for(row=0;row<mSize;row++){
		tRight->normal(0,row)=tLeft->normal(mSize-1,row);
	}
}

void TerrainPatch::unstitchFromRight(TerrainPatch *terrain){
	TerrainPatch *tLeft=this;
	TerrainPatch *tRight=terrain;
	int row;

	if(tLeft->getSizeN()!=tRight->getSizeN()){
		throw std::runtime_error("left->sizeN!=right->sizeN");
	}

	for(row=1;row<mSize-1;row++){
		tRight->vertex(0,row)->dependent0=0;
		tLeft->vertex(mSize-1,row)->dependent1=0;
	}
}

void TerrainPatch::stitchToBottom(TerrainPatch *terrain){
	TerrainPatch *tTop=this;
	TerrainPatch *tBottom=terrain;
	int col;

	if(tTop->getSizeN()!=tBottom->getSizeN()){
		throw std::runtime_error("top->sizeN!=bottom->sizeN");
	}

	for(col=1;col<mSize-1;col++){
		tBottom->vertex(col,mSize-1)->dependent0=tTop->vertex(col,0);
		tTop->vertex(col,0)->dependent1=tBottom->vertex(col,mSize-1);
	}

	for(col=0;col<mSize;col++){
		tBottom->normal(col,mSize-1)=tTop->normal(col,0);
	}
}

void TerrainPatch::unstitchFromBottom(TerrainPatch *terrain){
	TerrainPatch *tTop=this;
	TerrainPatch *tBottom=terrain;
	int col;

	if(tTop->getSizeN()!=tBottom->getSizeN()){
		throw std::runtime_error("top->sizeN!=bottom->sizeN");
	}

	for(col=1;col<mSize-1;col++){
		tTop->vertex(col,mSize-1)->dependent0=NULL;
		tBottom->vertex(col,0)->dependent1=NULL;
	}
}

void TerrainPatch::initBlocks(Block *block,int q,int x,int y,int s,bool e){
	int i=0;
	int j=0;
	float delta[5];
	
	block->xOrigin=x;
	block->yOrigin=y;
	block->stride=s;
	block->even=e;
	block->delta0=0;
	block->delta1=MAX_FLOAT;
	block->processed=false;

	delta[0]=(float)(position(x,y).z+position(x+2*s,y).z)/2-position(x+s,y).z;
	delta[0]*=delta[0];
	delta[1]=(float)(position(x+2*s,y).z+position(x+2*s,y+2*s).z)/2-position(x+2*s,y+s).z;
	delta[1]*=delta[1];
	delta[2]=(float)(position(x,y+2*s).z+position(x+2*s,y+2*s).z)/2-position(x+s,y+2*s).z;
	delta[2]*=delta[2];
	delta[3]=(float)(position(x,y).z+position(x,y+2*s).z)/2-position(x,y+s).z;
	delta[3]*=delta[3];
	if(block->even){
		delta[4]=(float)(position(x,y+2*s).z+position(x+2*s,y).z)/2-position(x+s,y+s).z;
	}
	else{
		delta[4]=(float)(position(x,y).z+position(x+2*s,y+2*s).z)/2-position(x+s,y+s).z;
	}
	delta[4]*=delta[4];

	block->deltaMax=delta[0];
	for(i=1;i<5;i++){
		if(delta[i]>block->deltaMax){
			block->deltaMax=delta[i];
		}
	}

	block->mins.x=position(x,y).x;
	block->maxs.x=position(x+s*2,y+s*2).x;
	block->mins.y=position(x,y).y;
	block->maxs.y=position(x+s*2,y+s*2).y;

	// Dependents set correctly! :)
	vertex(x+s,y)->dependent0=vertex(x+s,y+s);
	vertex(x,y+s)->dependent1=vertex(x+s,y+s);
	vertex(x+2*s,y+s)->dependent0=vertex(x+s,y+s);
	vertex(x+s,y+2*s)->dependent1=vertex(x+s,y+s);

	if(block->even==true){
		vertex(x+s,y+s)->dependent0=vertex(x,y+2*s);
		vertex(x+s,y+s)->dependent1=vertex(x+2*s,y);
	}
	else{
		vertex(x+s,y+s)->dependent0=vertex(x,y);
		vertex(x+s,y+s)->dependent1=vertex(x+2*s,y+2*s);
	}

	// Initialization set correctly! :)
	if(s>1){
		block->children[0]=&mBlocks[4*q+1];
		block->children[1]=&mBlocks[4*q+2];
		block->children[2]=&mBlocks[4*q+3];
		block->children[3]=&mBlocks[4*q+4];
		initBlocks(block->children[0],4*q+1,x,y,(int)s/2,true);
		initBlocks(block->children[1],4*q+2,x+s,y,(int)s/2,false);
		initBlocks(block->children[2],4*q+3,x,y+s,(int)s/2,false);
		initBlocks(block->children[3],4*q+4,x+s,y+s,(int)s/2,true);

		block->mins.z=block->children[0]->mins.z;
		block->maxs.z=block->children[0]->maxs.z;

		for(i=1;i<4;i++){
			if(block->mins.z>block->children[i]->mins.z)
				block->mins.z=block->children[i]->mins.z;
			if(block->maxs.z<block->children[i]->maxs.z)
				block->maxs.z=block->children[i]->maxs.z;

			if(block->deltaMax<block->children[i]->deltaMax)
				block->deltaMax=block->children[i]->deltaMax;
		}
	}
	else{
		block->mins.z=position(x,y).z;
		block->maxs.z=position(x,y).z;

		for(i=x;i<=x+2;i++){
			for(j=y;j<=y+2;j++){
				if(position(i,j).z<block->mins.z)
					block->mins.z=position(i,j).z;
				if(position(i,j).z>block->maxs.z)
					block->maxs.z=position(i,j).z;
			}
		}
	}
}

void TerrainPatch::resetBlocks(){
	int i;
	int x,y,s;

	mNumUnprocessedBlocks=mNumBlocksInQueue;

	for(i=0;i<mNumBlocksInQueue;i++){
		Block *b=getBlockNumber(i);

		x=b->xOrigin;
		y=b->yOrigin;
		s=b->stride;

		b->processed=false;

		disableVertex(vertex(x+s,y));
		disableVertex(vertex(x,y+s));
		disableVertex(vertex(x+s,y+s));
		disableVertex(vertex(x+2*s,y+s));
		disableVertex(vertex(x+s,y+s*2));

		if(b->even==true){
			disableVertex(vertex(x,y+s*2));
			disableVertex(vertex(x+2*s,y));
		}
		else{
			disableVertex(vertex(x,y));
			disableVertex(vertex(x+2*s,y+s*2));
		}
	}
}

void TerrainPatch::enableVertex(Vertex *v){
	if(v->enabled==false){
		v->enabled=true;
		if(v->dependent0){
			enableVertex(v->dependent0);
		}
		if(v->dependent1){
			enableVertex(v->dependent1);
		}
	}
}

void TerrainPatch::disableVertex(Vertex *v){
	if(v->enabled==true){
		v->enabled=false;
		if(v->dependent0){
			disableVertex(v->dependent0);
		}
		if(v->dependent1){
			disableVertex(v->dependent1);
		}
	}
}

void TerrainPatch::simplifyBlocks(){
	int blockNum;
	int i;

	while(mNumUnprocessedBlocks>0){
		blockNum=popBlockFromFront();

		if(mBlocks[blockNum].processed==false){
			mNumUnprocessedBlocks--;

			//Is block a first child?
			if(blockNum>0 && ((blockNum-1)%4)==0){

				//Does block have a sibling?
				if(mNumBlocksInQueue>2 && (blockNum-1)>>2==(getBlockNumberInQueue(2)-1)>>2){
					bool replaceParent=true;
					
					for(i=0;i<4;i++){
						if(blockShouldSubdivide(&mBlocks[blockNum+i])){
							replaceParent=false;
						}
					}

					if(replaceParent){
						for(i=1;i<4;i++){
							popBlockFromFront();
							
							if(mBlocks[blockNum+i].processed==false){
								mNumUnprocessedBlocks--;
							}
						}

						mBlocks[(blockNum-1)/4].processed=false;

						addBlockToBack((blockNum-1)/4);
						mNumUnprocessedBlocks++;

						continue;
					}
				}
			}

			// Is block interrior node?
			if(mBlocks[blockNum].stride>1){
				// TODO: Put back in optional blockIntersectsFrustum test here if on single pipe machine
				bool shouldSubdivide=false;
					
				for(i=0;i<4;i++){
					if(blockShouldSubdivide(mBlocks[blockNum].children[i])){
						shouldSubdivide=true;
					}
				}

				if(shouldSubdivide){
					for(i=0;i<4;i++){
						mBlocks[blockNum].children[i]->processed=false;
						addBlockToBack(4*blockNum+i+1);
					}
					continue;
				}
			}

			mBlocks[blockNum].processed=true;
		}

		addBlockToBack(blockNum);
	}
}

bool TerrainPatch::blockShouldSubdivide(Block *block){
#if 1
	Vector3 bo=(block->mins+block->maxs)/2.0f;
	float size=(block->maxs.z-block->mins.z);
	float distx=fabs(bo.x-(mPosition.x/*-mTranslate.x*/));
	float disty=fabs(bo.y-(mPosition.y/*-mTranslate.y*/));
	float dist=maxVal(distx,disty);

	dist*=square(mS1);
	size*=square(mS2);

	if(dist*mS1-size*mS2<=0){
		return true;
	}
	else{
		return false;
	}
#else
	computeDelta(block,f->origin,f->forward,tolerance);

	if(block->deltaMax>block->delta0){
		return true;
	}

	return false;
#endif
}

#if 0
void TerrainPatch::computeDelta(Block *block,Vector3 origin,Vector3 forward,float tolerance){
    float fHMin;

	origin=origin-mTranslate.x;

    if ( origin.z < block->mins.z)
    {
        fHMin = origin.z - block->mins.z;
        fHMin *= fHMin;
    }
    else if ( origin.z > block->maxs.z )
    {
        fHMin = origin.z - block->maxs.z;
        fHMin *= fHMin;
    }
    else
    {
        fHMin = 0.0;
    }

    // compute hmax = max{|eye.z - bmin.z|,|eye.z - bmax.z|}
    float fHMax = origin.z - block->mins.z;
    fHMax *= fHMax;
    float fTmp = origin.z - block->maxs.z;
    fTmp *= fTmp;
    if ( fTmp > fHMax )
        fHMax = fTmp;

    // compute rmin and rmax
    float fDxMin = origin.x - block->mins.x;
    float fDxMax = origin.x - block->maxs.x;
    float fDyMin = origin.y - block->mins.y;
    float fDyMax = origin.y - block->maxs.y;
    fDxMin *= fDxMin;
    fDxMax *= fDxMax;
    fDyMin *= fDyMin;
    fDyMax *= fDyMax;
    float fRMin = 0.0, fRMax = 0.0;

    if ( origin.x < block->mins.x )
    {
        fRMin += fDxMin;
        fRMax += fDxMax;
    }
    else if ( origin.x <= block->maxs.x )
    {
        fRMax += ( fDxMax >= fDxMin ? fDxMax : fDxMin );
    }
    else
    {
        fRMin += fDxMax;
        fRMax += fDxMin;
    }

    if ( origin.y < block->mins.y )
    {
        fRMin += fDyMin;
        fRMax += fDyMax;
    }
    else if ( origin.y <= block->maxs.y )
    {
        fRMax += ( fDyMax >= fDyMin ? fDyMax : fDyMin );
    }
    else
    {
        fRMin += fDyMax;
        fRMax += fDyMin;
    }

    // compute fmin
    float fDenom = fRMin + fHMax;
    float fFMin =
        (fDenom > 0.0 ? fRMin/(fDenom*fDenom) : vrInfinity);
    fDenom = fRMax + fHMax;
    fTmp = (fDenom > 0.0 ? fRMax/(fDenom*fDenom) : vrInfinity);
    if ( fTmp < fFMin )
        fFMin = fTmp;
    
    // compute fmax
    float fFMax;
    if ( fRMin >= fHMin )
    {
        fDenom = fRMin + fHMin;
        fFMax =
            (fDenom > 0.0 ? fRMin/(fDenom*fDenom) : vrInfinity);
    }
    else if ( fRMax <= fHMin )
    {
        fDenom = fRMax + fHMin;
        fFMax =
            (fDenom > 0.0 ? fRMax/(fDenom*fDenom) : vrInfinity);
    }
    else
    {
        fFMax = (fHMin > 0.0 ? 0.25/fHMin : vrInfinity);
    }

    block->delta0=tolerance/fFMax;
    block->delta1=(fFMin > 0.0 ? tolerance/fFMin : vrInfinity);
}
#endif

void TerrainPatch::simplifyVertexes(){
	int i;
	Block *b;
	for(i=0;i<mNumBlocksInQueue;i++){
		b=getBlockNumber(i);

		enableVertex(vertex(b->xOrigin+b->stride,b->yOrigin));
		enableVertex(vertex(b->xOrigin,b->yOrigin+b->stride));
		enableVertex(vertex(b->xOrigin+b->stride,b->yOrigin+b->stride));
		enableVertex(vertex(b->xOrigin+b->stride*2,b->yOrigin+b->stride));
		enableVertex(vertex(b->xOrigin+b->stride,b->yOrigin+b->stride*2));

		if(b->even==true){
			enableVertex(vertex(b->xOrigin,b->yOrigin+b->stride*2));
			enableVertex(vertex(b->xOrigin+b->stride*2,b->yOrigin));
		}
		else{
			enableVertex(vertex(b->xOrigin,b->yOrigin));
			enableVertex(vertex(b->xOrigin+b->stride*2,b->yOrigin+b->stride*2));
		}
	}
}

bool TerrainPatch::blockIntersectsFrustum(const Block *block,Frustum *f) const{
	AABox box(block->mins/*+mTranslate*/,block->maxs/*+mTranslate*/);
	return f->testIntersection(box,false);
}

int TerrainPatch::gatherBlocks(Frustum *f,IndexBuffer *indexes) const{
	int indexCount=0;
	int i;
	const Block *b;

	for(i=0;i<mNumBlocksInQueue;i++){
		b=getBlockNumber(i);

		if(blockIntersectsFrustum(b,f)){
			int x0=b->xOrigin;
			int y0=b->yOrigin;
			int x1=b->xOrigin+b->stride*2;
			int y1=b->yOrigin+b->stride*2;

			if(b->even==true){
				indexCount=gatherTriangle(indexes,indexCount,x0,y0,x0,y1,x1,y0);
				indexCount=gatherTriangle(indexes,indexCount,x1,y1,x1,y0,x0,y1);
			}
			else{
				indexCount=gatherTriangle(indexes,indexCount,x1,y0,x0,y0,x1,y1);
				indexCount=gatherTriangle(indexes,indexCount,x0,y1,x1,y1,x0,y0);
			}
		}
	}

	return indexCount;
}

int TerrainPatch::gatherTriangle(IndexBuffer *indexes,int indexCount,int x0,int y0,int x1,int y1,int x2,int y2) const{
	if(Math::abs(x0-x1)>1 || Math::abs(x2-x1)>1 || Math::abs(y0-y1)>1 || Math::abs(y2-y1)>1){
		int mx,my;

		mx=(x1+x2)/2;
		my=(y1+y2)/2;

		if(vertex(mx,my)->enabled){
			indexCount=gatherTriangle(indexes,indexCount,mx,my,x0,y0,x1,y1);
			indexCount=gatherTriangle(indexes,indexCount,mx,my,x2,y2,x0,y0);
			return indexCount;
		}
	}

	indexes->index(indexCount++)=x0+y0*mSize;
	indexes->index(indexCount++)=x2+y2*mSize;
	indexes->index(indexCount++)=x1+y1*mSize;

	return indexCount;
}

}
}
}
