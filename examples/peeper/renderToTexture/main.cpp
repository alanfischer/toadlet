#include <toadlet/egg/System.h>
#include <toadlet/pad/Application.h>
#include <toadlet/peeper/SurfaceRenderTarget.h>

using namespace toadlet;
using namespace toadlet::egg;
using namespace toadlet::egg::image;
using namespace toadlet::peeper;
using namespace toadlet::pad;

class RenderToTexture:public Application{
public:
	RenderToTexture():Application(){}

	void create(){
		int options[]={1,0,2,0,0};
		setRendererOptions(options,3);

		Application::create();

		triVertexBuffer=VertexBuffer::ptr(new VertexBuffer(Buffer::UsageType_STATIC,Buffer::AccessType_WRITE_ONLY,mEngine->getVertexFormats().POSITION_COLOR,3));
		{
			VertexBufferAccessor a(triVertexBuffer,Buffer::LockType_WRITE_ONLY);
			a.set3(0,0, 0,Math::ONE,0);				a.setABGR(0,1, 0x000000FF);
			a.set3(1,0, -Math::ONE,-Math::ONE,0);	a.setABGR(1,1, 0x0000FF00);
			a.set3(2,0, Math::ONE,-Math::ONE,0);	a.setABGR(2,1, 0x00FF0000);
		}
		triVertexData=VertexData::ptr(new VertexData(triVertexBuffer));
		triIndexData=IndexData::ptr(new IndexData(IndexData::Primitive_TRIFAN,NULL,0,3));

		quadVertexBuffer=VertexBuffer::ptr(new VertexBuffer(Buffer::UsageType_STATIC,Buffer::AccessType_WRITE_ONLY,mEngine->getVertexFormats().POSITION_TEX_COORD,4));
		{
			VertexBufferAccessor a(quadVertexBuffer,Buffer::LockType_WRITE_ONLY);
			a.set3(0,0, -Math::ONE,Math::ONE,0);	a.set2(0,1, 0,Math::ONE);
			a.set3(1,0, -Math::ONE,-Math::ONE,0);	a.set2(1,1, 0,0);
			a.set3(2,0, Math::ONE,Math::ONE,0);		a.set2(2,1, Math::ONE,Math::ONE);
			a.set3(3,0, Math::ONE,-Math::ONE,0);	a.set2(3,1, Math::ONE,0);
		}
		quadVertexData=VertexData::ptr(new VertexData(quadVertexBuffer));
		quadIndexData=IndexData::ptr(new IndexData(IndexData::Primitive_TRISTRIP,NULL,0,4));

		renderTexture=Texture::ptr(getRenderer()->createTexture());
		renderTexture->create(Texture::UsageFlags_RENDERTARGET,Texture::Dimension_D2,Texture::Format_RGB_8,256,256,0,0);

		surfaceRenderTarget=SurfaceRenderTarget::ptr(getRenderer()->createSurfaceRenderTarget());
		surfaceRenderTarget->create();
		surfaceRenderTarget->attach(renderTexture->getMipSuface(0),SurfaceRenderTarget::Attachment_COLOR_0);

		quadTextureStage=TextureStage::ptr(new TextureStage(renderTexture));

		distance=0;
	}

	void render(Renderer *renderer){
		renderer->setRenderTarget(surfaceRenderTarget);
		int i,j;
		for(i=0;i<16;++i){
			for(j=0;j<16;++j){
				int w=renderTexture->getWidth()/16;
				int h=renderTexture->getHeight()/16;
				renderer->setViewport(Viewport(w*i,h*j,w,h));
				Color color;
				if((i+j)%2==(System::mtime()/100)%2){
					color=Colors::BLUE;
				}
				else{
					color=Colors::RED;
				}
				renderer->clear(Renderer::ClearFlag_COLOR,color);
			}
		}

		renderer->setViewport(Viewport(0,0,renderTexture->getWidth(),renderTexture->getHeight()));

		renderer->clear(Renderer::ClearFlag_DEPTH,Colors::BLACK);

		renderer->beginScene();

		Math::setMatrix4x4FromPerspectiveY(projectionMatrix,Math::degToRad(45),(float)mWidth/(float)mHeight,.1,100);

		renderer->setProjectionMatrix(projectionMatrix);

		renderer->setViewMatrix(viewMatrix);

		Math::setMatrix4x4FromTranslate(modelMatrix,Vector3(0,0,-distance));
		renderer->setModelMatrix(modelMatrix);

		renderer->setTextureStage(0,NULL);
		renderer->renderPrimitive(triVertexData,triIndexData);

		renderer->endScene();

		renderer->swap();

		renderer->setRenderTarget(this);
		renderer->setViewport(Viewport(0,0,mWidth,mHeight));
		renderer->clear(Renderer::ClearFlag_COLOR|Renderer::ClearFlag_DEPTH,Colors::GREEN);
		renderer->beginScene();

		renderer->setProjectionMatrix(projectionMatrix);

		renderer->setViewMatrix(viewMatrix);

		Math::setMatrix4x4FromTranslate(modelMatrix,Vector3(0,0,-distance));
		renderer->setModelMatrix(modelMatrix);

		renderer->setTextureStage(0,quadTextureStage);
		renderer->renderPrimitive(quadVertexData,quadIndexData);

		renderer->endScene();

		renderer->swap();
	}

	void update(int dt){
		distance=Math::fromInt(6)+Math::sin(Math::fromMilli(System::mtime()))*2;
	}

	VertexBuffer::ptr triVertexBuffer;
	VertexData::ptr triVertexData;
	IndexData::ptr triIndexData;

	VertexBuffer::ptr quadVertexBuffer;
	VertexData::ptr quadVertexData;
	IndexData::ptr quadIndexData;

	Texture::ptr renderTexture;
	SurfaceRenderTarget::ptr surfaceRenderTarget;
	TextureStage::ptr quadTextureStage;
	scalar distance;
	Matrix4x4 projectionMatrix;
	Matrix4x4 viewMatrix;
	Matrix4x4 modelMatrix;
};

int main(int argc,char **argv){
	RenderToTexture test;
	test.setFullscreen(false);
	test.create();
	test.start(true);
	test.destroy();
}
