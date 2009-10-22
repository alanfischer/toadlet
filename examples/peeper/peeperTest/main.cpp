#define USE_GLRENDERER
//#define USE_D3DRENDERER
//#define USE_NDSRENDERER
#define TOADLET_USE_STATIC
#define USE_LINK_PEEPER_PLUGIN

#include "../../ChooseLibs.h"
#include "../../ScreenSize.h"
#include "../../UniformMain.cpp"

#include <toadlet/peeper/Renderer.h>
#include <toadlet/peeper/RenderWindow.h>
#include <toadlet/peeper/Colors.h>
#include <toadlet/peeper/VertexData.h>
#include <toadlet/peeper/IndexData.h>
#include <toadlet/peeper/Viewport.h>
#include <toadlet/peeper/LightEffect.h>
#include <toadlet/egg/Logger.h>

using namespace toadlet::egg;
using namespace toadlet::peeper;

class PeeperTest:public RenderWindowListener{
public:
	PeeperTest(){
		renderWindow=NULL;
		renderer=NULL;
		bool result;

		ScreenSize screenSize;
		width=screenSize.getWidth();
		height=screenSize.getHeight();
		if(width>640){
			width=640;
		}
		if(height>480){
			height=480;
		}

#		if !defined(TOADLET_PLATFORM_NDS)
			renderWindow=new_RenderWindow();

			result=renderWindow->startup("toadlet::peeper::PeeperTest",0,0,width,height,false,toadlet::peeper::Visual()); // Use full path for xwindows conflicts
			if(result==false){
				TOADLET_LOG(NULL,Logger::LEVEL_ERROR,
					"Error starting render window");
				delete renderWindow;
				renderWindow=NULL;
				return;
			}

			renderWindow->setRenderWindowListener(this);
#		endif

		renderer=new_Renderer();
		result=renderer->startup(renderWindow,NULL);
		if(result==false){
			TOADLET_LOG(NULL,Logger::LEVEL_ERROR,
				"Error starting renderer");
			delete renderer;
			renderer=NULL;
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
			triIndexData=IndexData::Ptr(new IndexData(IndexData::PRIMITIVE_TRISTRIP,triIndexBuffer,0,3));
		}

		{
			VertexFormat vertexFormat(1);
			vertexFormat.addVertexElement(VertexElement(VertexElement::TYPE_POSITION,VertexElement::Format(typeBit|VertexElement::FORMAT_BIT_COUNT_3)));
			quadVertexBuffer=VertexBuffer::Ptr(new VertexBuffer(renderer,Buffer::UT_STATIC,Buffer::AT_WRITE_ONLY,vertexFormat,4));
			{
				VertexBufferAccessor a(quadVertexBuffer,Buffer::LT_WRITE_ONLY);
#				if defined(TOADLET_PEEPER_FIXED_POINT)
					a.position3x(0)=toadlet::egg::mathfixed::Vector3(-Math::ONE,Math::ONE,0);
					a.position3x(1)=toadlet::egg::mathfixed::Vector3(-Math::ONE,-Math::ONE,0);
					a.position3x(2)=toadlet::egg::mathfixed::Vector3(Math::ONE,-Math::ONE,0);
					a.position3x(3)=toadlet::egg::mathfixed::Vector3(Math::ONE,Math::ONE,0);
#				else
					a.position3f(0)=toadlet::egg::math::Vector3(-1,1,0);
					a.position3f(1)=toadlet::egg::math::Vector3(-1,-1,0);
					a.position3f(2)=toadlet::egg::math::Vector3(1,-1,0);
					a.position3f(3)=toadlet::egg::math::Vector3(1,1,0);
#				endif
			}
			quadVertexData=VertexData::Ptr(new VertexData(quadVertexBuffer));

			quadIndexBuffer=IndexBuffer::Ptr(new IndexBuffer(renderer,Buffer::UT_STATIC,Buffer::AT_WRITE_ONLY,IndexBuffer::IDT_UINT_16,4));
			{
				IndexBufferAccessor a(quadIndexBuffer,Buffer::LT_WRITE_ONLY);
				a.index16(0)=0;
				a.index16(1)=1;
				a.index16(2)=3;
				a.index16(3)=2;
			}
			quadIndexData=IndexData::Ptr(new IndexData(IndexData::PRIMITIVE_TRISTRIP,quadIndexBuffer,0,4));
		}

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
	}

	~PeeperTest(){
		triVertexBuffer=VertexBuffer::Ptr();
		triVertexData=VertexData::Ptr();
		triIndexBuffer=IndexBuffer::Ptr();
		triIndexData=IndexData::Ptr();

		quadVertexBuffer=VertexBuffer::Ptr();
		quadVertexData=VertexData::Ptr();
		quadIndexBuffer=IndexBuffer::Ptr();
		quadIndexData=IndexData::Ptr();

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
#		if !defined(TOADLET_PLATFORM_NDS)
			renderWindow->startEventLoop();
#		else
			while(true){
				render();
			}
#		endif
	}

	void render(){
		Renderer::Status status=renderer->getStatus();
		if(status!=Renderer::STATUS_OK){
			TOADLET_LOG(NULL,Logger::LEVEL_ERROR,
				String("Renderer status is ")+(int)status);
		}

		if(renderWindow!=NULL){
			renderer->setViewport(Viewport(0,0,renderWindow->getWidth(),renderWindow->getHeight()));
		}
		else{
			renderer->setViewport(Viewport(0,0,width,height));
		}

		renderer->clearRenderTargetBuffers(Renderer::RTB_COLOR|Renderer::RTB_DEPTH,Colors::COLOR_BLACK);
		renderer->beginScene();

		renderer->setProjectionMatrix(projectionMatrix);
		renderer->setViewMatrix(viewMatrix);

#		if defined(TOADLET_PEEPER_FIXED_POINT)
			Math::setMatrix4x4FromTranslate(modelMatrix,Vector3(-Math::milliToFixed(1500),0,-Math::intToFixed(6)));
#		else
			Math::setMatrix4x4FromTranslate(modelMatrix,Vector3(-1.5,0,-6));
#		endif
		renderer->setModelMatrix(modelMatrix);
		renderer->renderPrimitive(triVertexData,triIndexData);

#		if defined(TOADLET_PEEPER_FIXED_POINT)
			Math::setMatrix4x4FromTranslate(modelMatrix,Vector3(Math::milliToFixed(1500),0,-Math::intToFixed(6)));
#		else
			Math::setMatrix4x4FromTranslate(modelMatrix,Vector3(1.5,0,-6));
#		endif
		renderer->setModelMatrix(modelMatrix);
		renderer->setAmbientColor(Color(Math::HALF,Math::HALF,Math::ONE,0));
		LightEffect effect;
		effect.ambient=Color(Math::ONE,Math::ONE,Math::ONE,Math::ONE);
		renderer->setLightEffect(effect);
		renderer->setLighting(true);
		renderer->renderPrimitive(quadVertexData,quadIndexData);
		renderer->setLighting(false);

		renderer->endScene();

		renderer->swapRenderTargetBuffers();
	}

	void update(){}
	void keyDown(int key){}
	void keyUp(int key){}
	void mouseMove(int x,int y){}
	void mouseDown(Mouse button){}
	void mouseUp(Mouse button){}
	void mouseScroll(int s){}
	void resize(int width,int height){}
	void close(){}
	void focusGained(){}
	void focusLost(){}
	void mouseLeave(){}
	void mouseEnter(){}

	int width,height;
	RenderWindow *renderWindow;
	Renderer *renderer;

	VertexBuffer::Ptr triVertexBuffer;
	VertexData::Ptr triVertexData;
	IndexBuffer::Ptr triIndexBuffer;
	IndexData::Ptr triIndexData;

	VertexBuffer::Ptr quadVertexBuffer;
	VertexData::Ptr quadVertexData;
	IndexBuffer::Ptr quadIndexBuffer;
	IndexData::Ptr quadIndexData;

	toadlet::peeper::Matrix4x4 projectionMatrix;
	toadlet::peeper::Matrix4x4 viewMatrix;
	toadlet::peeper::Matrix4x4 modelMatrix;
};

int uniformMain(int argc,char **argv){
	PeeperTest test;
	if(test.renderer!=NULL){
		test.run();
	}
	
	return 0;
}
