#ifndef HL_HALFLIFEMDLHANDLER_H
#define HL_HALFLIFEMDLHANDLER_H

#include <toadlet/tadpole/resource/handler/MeshDataHandler.h>

namespace hl{

class HalfLifeMDLHandler:public toadlet::tadpole::resource::handler::MeshDataHandler{
public:
	HalfLifeMDLHandler();
	virtual ~HalfLifeMDLHandler();

	toadlet::tadpole::mesh::MeshData *load(toadlet::egg::io::InputStream *in,const toadlet::egg::String &name);
	void save(toadlet::tadpole::mesh::MeshData *resource,toadlet::egg::io::OutputStream *out,const toadlet::egg::String &name);
	bool handlesType(const toadlet::egg::String &ext);

	void setTextureManager(toadlet::tadpole::resource::TextureManager *textureManager);
	void setMaterialManager(toadlet::tadpole::resource::MaterialManager *materialManager);
	void setSkeletonDataManager(toadlet::tadpole::resource::SkeletonDataManager *skeletonDataManager);
	void setAnimationDataManager(toadlet::tadpole::resource::AnimationDataManager *animationDataManager);
	void setVertexBufferManager(toadlet::tadpole::resource::VertexBufferManager *vertexBufferManager);
	void setIndexBufferManager(toadlet::tadpole::resource::IndexBufferManager *indexBufferManager);

protected:
	toadlet::tadpole::resource::TextureManager *mTextureManager;
	toadlet::tadpole::resource::MaterialManager *mMaterialManager;
	toadlet::tadpole::resource::SkeletonDataManager *mSkeletonDataManager;
	toadlet::tadpole::resource::AnimationDataManager *mAnimationDataManager;
	toadlet::tadpole::resource::VertexBufferManager *mVertexBufferManager;
	toadlet::tadpole::resource::IndexBufferManager *mIndexBufferManager;
};

}

#endif

