#include <toadlet/egg/System.h>
#include <toadlet/egg/Limits.h>
#include <toadlet/egg/Logger.h>
#include <toadlet/peeper/Renderer.h>
#include <toadlet/peeper/RenderWindow.h>
#include <toadlet/peeper/Colors.h>
#include <toadlet/peeper/VertexData.h>
#include <toadlet/peeper/IndexData.h>
#include <toadlet/peeper/Viewport.h>
#include <toadlet/peeper/RenderTexture.h>
#include <toadlet/peeper/TextureStage.h>
#include <toadlet/peeper/CapabilitySet.h>
#if defined(TOADLET_PLATFORM_WINCE)
#	include <windows.h>
#endif

#define USE_GLRENDERER
//#define USE_D3DRENDERER
#define TOADLET_USE_STATIC
#define USE_LINK_PEEPER_PLUGIN

#include "../../ChooseLibs.h"
#include "../../Display.h"
#include "../../UniformMain.cpp"

using namespace toadlet::egg;
using namespace toadlet::egg::image;
using namespace toadlet::peeper;

class RenderToTexture:public RenderWindowListener{
public:
	RenderToTexture(){
		bool result;
		renderWindow=NULL;
		renderer=NULL;

		app::Display screenSize;
		width=screenSize.getWidth();
		height=screenSize.getHeight();
		if(width>640){
			width=640;
		}
		if(height>480){
			height=480;
		}

		renderWindow=new_RenderWindow();
		result=renderWindow->startup("toadlet::peeper",0,0,width,height,false,Visual());
		if(result==false){
			TOADLET_LOG(NULL,Logger::LEVEL_ERROR,
				"Error starting render window");
			delete renderWindow;
			renderWindow=NULL;
			return;
		}

		renderWindow->setRenderWindowListener(this);

		renderer=new_Renderer();

		// Force the use of PBuffers here by turning off FBOs
		Renderer::Options options[]={
			2,0,
			0
		};
		result=renderer->startup(renderWindow,options);
		if(result==false){
			TOADLET_LOG(NULL,Logger::LEVEL_ERROR,
				"Error starting renderer");
			delete renderer;
			renderer=NULL;
			return;
		}

		renderTexture=RenderTexture::Ptr(new RenderTexture(renderer,Image::FORMAT_RGB_5_6_5,128,128));

		if(renderTexture->internal_getTexturePeer()==NULL){
			TOADLET_LOG(NULL,Logger::LEVEL_ERROR,
				"Render to texture support not available");
			return;
		}

#		if defined(TOADLET_PEEPER_FIXED_POINT)
			int typeBit=VertexElement::FORMAT_BIT_FIXED_32;
#		else
			int typeBit=VertexElement::FORMAT_BIT_FLOAT_32;
#		endif

		{
			VertexFormat vertexFormat(2);
			vertexFormat.addVertexElement(VertexElement(VertexElement::TYPE_POSITION,VertexElement::Format(typeBit|VertexElement::FORMAT_BIT_COUNT_3)));
			vertexFormat.addVertexElement(VertexElement(VertexElement::TYPE_COLOR,VertexElement::Format(VertexElement::FORMAT_COLOR_RGBA)));
			triVertexBuffer=VertexBuffer::Ptr(new VertexBuffer(renderer,Buffer::UT_STATIC,Buffer::AT_WRITE_ONLY,vertexFormat,3));
			{
				VertexBufferAccessor a(triVertexBuffer,Buffer::LT_WRITE_ONLY);
				a.color4b(0)=0x000000FF;
				a.color4b(1)=0x0000FF00;
				a.color4b(2)=0x00FF0000;
#				if defined(TOADLET_PEEPER_FIXED_POINT)
					a.position3x(0)=toadlet::egg::mathfixed::Vector3(0,Math::ONE,0);
					a.position3x(1)=toadlet::egg::mathfixed::Vector3(-Math::ONE,-Math::ONE,0);
					a.position3x(2)=toadlet::egg::mathfixed::Vector3(Math::ONE,-Math::ONE,0);
#				else
					a.position3f(0)=toadlet::egg::math::Vector3(0,1,0);
					a.position3f(1)=toadlet::egg::math::Vector3(-1,-1,0);
					a.position3f(2)=toadlet::egg::math::Vector3(1,-1,0);
#				endif
			}
			triVertexData=VertexData::Ptr(new VertexData(triVertexBuffer));

			triIndexBuffer=IndexBuffer::Ptr(new IndexBuffer(renderer,Buffer::UT_STATIC,Buffer::AT_WRITE_ONLY,IndexBuffer::IDT_UINT_16,3));
			{
				IndexBufferAccessor a(triIndexBuffer,Buffer::LT_WRITE_ONLY);
				a.index16(0)=0;
				a.index16(1)=1;
				a.index16(2)=2;
			}
			triIndexData=IndexData::Ptr(new IndexData(IndexData::PRIMITIVE_TRIFAN,triIndexBuffer,0,3));
		}

		{
			VertexFormat vertexFormat(2);
			vertexFormat.addVertexElement(VertexElement(VertexElement::TYPE_POSITION,VertexElement::Format(typeBit|VertexElement::FORMAT_BIT_COUNT_3)));
			vertexFormat.addVertexElement(VertexElement(VertexElement::TYPE_TEX_COORD,VertexElement::Format(typeBit|VertexElement::FORMAT_BIT_COUNT_2)));
			quadVertexBuffer=VertexBuffer::Ptr(new VertexBuffer(renderer,Buffer::UT_STATIC,Buffer::AT_WRITE_ONLY,vertexFormat,4));
			{
				VertexBufferAccessor a(quadVertexBuffer,Buffer::LT_WRITE_ONLY);
#				if defined(TOADLET_PEEPER_FIXED_POINT)
					a.position3x(0)=toadlet::egg::mathfixed::Vector3(-Math::ONE,Math::ONE,0);
					a.texCoord2x(0)=toadlet::egg::mathfixed::Vector2(0,Math::ONE);
					a.position3x(1)=toadlet::egg::mathfixed::Vector3(-Math::ONE,-Math::ONE,0);
					a.texCoord2x(1)=toadlet::egg::mathfixed::Vector2(0,0);
					a.position3x(2)=toadlet::egg::mathfixed::Vector3(Math::ONE,-Math::ONE,0);
					a.texCoord2x(2)=toadlet::egg::mathfixed::Vector2(Math::ONE,0);
					a.position3x(3)=toadlet::egg::mathfixed::Vector3(Math::ONE,Math::ONE,0);
					a.texCoord2x(3)=toadlet::egg::mathfixed::Vector2(Math::ONE,Math::ONE);
#				else
					a.position3f(0)=toadlet::egg::math::Vector3(-1,1,0);
					a.texCoord2f(0)=toadlet::egg::math::Vector2(0,1);
					a.position3f(1)=toadlet::egg::math::Vector3(-1,-1,0);
					a.texCoord2f(1)=toadlet::egg::math::Vector2(0,0);
					a.position3f(2)=toadlet::egg::math::Vector3(1,-1,0);
					a.texCoord2f(2)=toadlet::egg::math::Vector2(1,0);
					a.position3f(3)=toadlet::egg::math::Vector3(1,1,0);
					a.texCoord2f(3)=toadlet::egg::math::Vector2(1,1);
#				endif
			}
			quadVertexData=VertexData::Ptr(new VertexData(quadVertexBuffer));

			quadIndexBuffer=IndexBuffer::Ptr(new IndexBuffer(renderer,Buffer::UT_STATIC,Buffer::AT_WRITE_ONLY,IndexBuffer::IDT_UINT_16,4));
			{
				IndexBufferAccessor a(quadIndexBuffer,Buffer::LT_WRITE_ONLY);
				a.index16(0)=0;
				a.index16(1)=1;
				a.index16(2)=2;
				a.index16(3)=3;
			}
			quadIndexData=IndexData::Ptr(new IndexData(IndexData::PRIMITIVE_TRIFAN,quadIndexBuffer,0,4));
		}

		quadTextureStage=TextureStage(renderTexture);

		if(width>=height){
#			if defined(TOADLET_PEEPER_FIXED_POINT)
				Math::setMatrix4x4FromPerspectiveY(projectionMatrix,Math::degToRad(Math::intToFixed(45)),Math::div(Math::intToFixed(width),Math::intToFixed(height)),Math::milliToFixed(100),Math::intToFixed(100));
#			else
				Math::setMatrix4x4FromPerspectiveY(projectionMatrix,Math::degToRad(45),(float)width/(float)height,.1,100);
#			endif
		}
		else{
#			if defined(TOADLET_PEEPER_FIXED_POINT)
				Math::setMatrix4x4FromPerspectiveX(projectionMatrix,Math::degToRad(Math::intToFixed(45)),Math::div(Math::intToFixed(height),Math::intToFixed(width)),Math::milliToFixed(100),Math::intToFixed(100));
#			else
				Math::setMatrix4x4FromPerspectiveX(projectionMatrix,Math::degToRad(45),(float)height/(float)width,.1,100);
#			endif
		}

		distance=0;
	}

	~RenderToTexture(){
		triVertexBuffer=VertexBuffer::Ptr();
		triVertexData=VertexData::Ptr();
		triIndexBuffer=IndexBuffer::Ptr();
		triIndexData=IndexData::Ptr();

		quadVertexBuffer=VertexBuffer::Ptr();
		quadVertexData=VertexData::Ptr();
		quadIndexBuffer=IndexBuffer::Ptr();
		quadIndexData=IndexData::Ptr();

		quadTextureStage=TextureStage();
		renderTexture=RenderTexture::Ptr();

		if(renderer!=NULL){
			renderer->shutdown();
			delete renderer;
			renderer=NULL;
		}

		if(renderWindow!=NULL){
			renderWindow->shutdown();
			delete renderWindow;
			renderWindow=NULL;
		}
	}

	void run(){
		renderWindow->startEventLoop();
	}

	void render(){
		renderer->setRenderTarget(renderTexture);
		int i,j;
		for(i=0;i<16;++i){
			for(j=0;j<16;++j){
				int w=renderTexture->getWidth()/16;
				int h=renderTexture->getHeight()/16;
				renderer->setViewport(Viewport(w*i,h*j,w,h));
				Color color;
				if((i+j)%2==(System::mtime()/100)%2){
					color=Colors::COLOR_BLUE;
				}
				else{
					color=Colors::COLOR_RED;
				}
				renderer->clearRenderTargetBuffers(Renderer::RTB_COLOR,color);
			}
		}

		if(renderWindow!=NULL){
			renderer->setViewport(Viewport(0,0,renderWindow->getWidth(),renderWindow->getHeight()));
		}
		else{
			renderer->setViewport(Viewport(0,0,width,height));
		}

		renderer->clearRenderTargetBuffers(Renderer::RTB_DEPTH,Colors::COLOR_BLACK);

		renderer->beginScene();

		renderer->setProjectionMatrix(projectionMatrix);

		renderer->setViewMatrix(viewMatrix);

		Math::setMatrix4x4FromTranslate(modelMatrix,Vector3(0,0,-distance));
		renderer->setModelMatrix(modelMatrix);

		renderer->disableTextureStage(0);
		renderer->renderPrimitive(triVertexData,triIndexData);

		renderer->endScene();

		renderer->swapRenderTargetBuffers();

		renderer->setRenderTarget(renderWindow);
		renderer->setViewport(Viewport(0,0,renderWindow->getWidth(),renderWindow->getHeight()));
		renderer->clearRenderTargetBuffers(Renderer::RTB_COLOR|Renderer::RTB_DEPTH,Colors::COLOR_GREEN);
		renderer->beginScene();

		renderer->setProjectionMatrix(projectionMatrix);

		renderer->setViewMatrix(viewMatrix);

		Math::setMatrix4x4FromTranslate(modelMatrix,Vector3(0,0,-distance));
		renderer->setModelMatrix(modelMatrix);

		renderer->setTextureStage(0,quadTextureStage);
		renderer->renderPrimitive(quadVertexData,quadIndexData);

		renderer->endScene();

		renderer->swapRenderTargetBuffers();
	}

	void update(){
#		if defined(TOADLET_PEEPER_FIXED_POINT)
			distance=Math::intToFixed(6)+Math::sin(Math::milliToFixed(System::mtime())%Math::TWO_PI)*2;
#		else
			distance=6+Math::sin((int)(System::mtime()%Limits::MAX_INT)/1000.0)*2;
#		endif
	}

	void keyDown(int key){}
	void keyUp(int key){}
	void mouseMove(int x,int y){}
	void mouseDown(int x,int y,Mouse button){}
	void mouseUp(int x,int y,Mouse button){}
	void mouseScroll(int x,int y,int s){}
	void resized(int width,int height){}
	void closed(){}
	void focusGained(){}
	void focusLost(){}
	void mouseLeave(){}
	void mouseEnter(){}

	int width,height;
	RenderWindow *renderWindow;
	Renderer *renderer;
	RenderTexture::Ptr renderTexture;

	VertexBuffer::Ptr triVertexBuffer;
	VertexData::Ptr triVertexData;
	IndexBuffer::Ptr triIndexBuffer;
	IndexData::Ptr triIndexData;

	VertexBuffer::Ptr quadVertexBuffer;
	VertexData::Ptr quadVertexData;
	IndexBuffer::Ptr quadIndexBuffer;
	IndexData::Ptr quadIndexData;

	TextureStage quadTextureStage;

	toadlet::peeper::Matrix4x4 projectionMatrix;
	toadlet::peeper::Matrix4x4 viewMatrix;
	toadlet::peeper::Matrix4x4 modelMatrix;
	toadlet::peeper::scalar distance;
};

int uniformMain(int argc,char **argv){
	RenderToTexture test;
	if(test.renderTexture!=NULL && test.renderTexture->internal_getTexturePeer()!=NULL){
		test.run();
	}
	return 0;
}
