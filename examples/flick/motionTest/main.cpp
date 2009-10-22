#define USE_GLRENDERER
//#define USE_D3DRENDERER
#define USE_LINK_PEEPER_PLUGIN
#define USE_HTCMOTIONDETECTOR
#define USE_LINK_FLICK_PLUGIN
#define TOADLET_USE_STATIC

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
#include <toadlet/flick/MotionDetector.h>
#include <toadlet/tadpole/Engine.h>
#include <toadlet/tadpole/sg/CameraEntity.h>
#include <toadlet/tadpole/sg/MeshEntity.h>
#include <toadlet/egg/System.h>
#include <toadlet/egg/Logger.h>
#include <toadlet/egg/mathfixed/Math.h>

using namespace toadlet;
using namespace toadlet::egg;
using namespace toadlet::egg::mathfixed;
using namespace toadlet::egg::mathfixed::Math;
using namespace toadlet::peeper;
using namespace toadlet::flick;
using namespace toadlet::tadpole;
using namespace toadlet::tadpole::sg;

class FlickTest:public RenderWindowListener{
public:
	FlickTest(){
		renderWindow=NULL;
		renderer=NULL;
		motionDetector=NULL;
		motionFileID=0;
		motionFile=NULL;
		engine=NULL;
		bool result;
		
		v=ZERO_VECTOR3;
		a=ZERO_VECTOR3;
		ao=ZERO_VECTOR3;
		aoo=ZERO_VECTOR3;
		initial=false;
		
		ScreenSize screenSize;
		width=screenSize.getWidth();
		height=screenSize.getHeight();
		if(width>640){
			width=640;
		}
		if(height>480){
			height=480;
		}

		renderWindow=new_RenderWindow();

		result=renderWindow->startup("toadlet::flick::FlickTest",0,0,width,height,false,toadlet::peeper::Visual()); // Use full path for xwindows conflicts
		if(result==false){
			TOADLET_LOG(NULL,Logger::LEVEL_ERROR,
				"Error starting render window");
			delete renderWindow;
			renderWindow=NULL;
			return;
		}

		renderWindow->setRenderWindowListener(this);

		renderer=new_Renderer();
		result=renderer->startup(renderWindow,NULL);
		if(result==false){
			TOADLET_LOG(NULL,Logger::LEVEL_ERROR,
				"Error starting renderer");
			delete renderer;
			renderer=NULL;
			return;
		}

		motionDetector=new_HTCMotionDetector();
		result=motionDetector->create();
		if(result){
			newMotionFile();
			result=motionDetector->startup();
		}
		if(result==false){
			delete motionDetector;
			motionDetector=NULL;
			return;
		}

		engine=Engine::newEngineWithNoHandlers();

		engine->setRenderer(renderer);

		engine->setSceneManager(new SceneManager(engine));

#		if defined(TOADLET_PEEPER_FIXED_POINT)
			int typeBit=VertexElement::FORMAT_BIT_FIXED_32;
#		else
			int typeBit=VertexElement::FORMAT_BIT_FLOAT_32;
#		endif

		{
			VertexFormat vertexFormat(2);
			vertexFormat.addVertexElement(VertexElement(VertexElement::TYPE_POSITION,VertexElement::Format(typeBit|VertexElement::FORMAT_BIT_COUNT_3)));
			vertexFormat.addVertexElement(VertexElement(VertexElement::TYPE_COLOR,VertexElement::Format(VertexElement::FORMAT_COLOR_RGBA)));
			VertexBuffer::Ptr triVertexBuffer=engine->loadVertexBuffer(VertexBuffer::Ptr(new VertexBuffer(Buffer::UT_STATIC,Buffer::AT_WRITE_ONLY,vertexFormat,3)));
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
			VertexData::Ptr triVertexData(new VertexData(triVertexBuffer));

			IndexBuffer::Ptr triIndexBuffer=engine->loadIndexBuffer(IndexBuffer::Ptr(new IndexBuffer(Buffer::UT_STATIC,Buffer::AT_WRITE_ONLY,IndexBuffer::IDT_UINT_16,3)));
			{
				IndexBufferAccessor a(triIndexBuffer,Buffer::LT_WRITE_ONLY);
				a.index16(0)=0;
				a.index16(1)=1;
				a.index16(2)=2;
			}
			IndexData::Ptr triIndexData(new IndexData(IndexData::PRIMITIVE_TRISTRIP,triIndexBuffer,0,3));

			Mesh::Ptr mesh(new Mesh());
			mesh->staticVertexData=triVertexData;

			Mesh::SubMesh::Ptr subMesh(new Mesh::SubMesh());
			mesh->subMeshes.push_back(subMesh);
			subMesh->indexData=triIndexData;

			Material::Ptr material(new Material());
			subMesh->material=material;

			MeshEntity::Ptr meshEntity(new MeshEntity(engine));
			meshEntity->load(mesh);
#			if defined(TOADLET_TADPOLE_FIXED_POINT)
				meshEntity->setTranslate(-Math::milliToFixed(1500),0,-Math::intToFixed(6));
#			else
				meshEntity->setTranslate(-1.5,0,-6);
#			endif
			engine->getSceneManager()->attach(meshEntity);
		}

		{
			VertexFormat vertexFormat(1);
			vertexFormat.addVertexElement(VertexElement(VertexElement::TYPE_POSITION,VertexElement::Format(typeBit|VertexElement::FORMAT_BIT_COUNT_3)));
			VertexBuffer::Ptr quadVertexBuffer=engine->loadVertexBuffer(VertexBuffer::Ptr(new VertexBuffer(Buffer::UT_STATIC,Buffer::AT_WRITE_ONLY,vertexFormat,4)));
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
			VertexData::Ptr quadVertexData(new VertexData(quadVertexBuffer));

			IndexBuffer::Ptr quadIndexBuffer=engine->loadIndexBuffer(IndexBuffer::Ptr(new IndexBuffer(Buffer::UT_STATIC,Buffer::AT_WRITE_ONLY,IndexBuffer::IDT_UINT_16,4)));
			{
				IndexBufferAccessor a(quadIndexBuffer,Buffer::LT_WRITE_ONLY);
				a.index16(0)=0;
				a.index16(1)=1;
				a.index16(2)=3;
				a.index16(3)=2;
			}
			IndexData::Ptr quadIndexData(new IndexData(IndexData::PRIMITIVE_TRISTRIP,quadIndexBuffer,0,4));

			Mesh::Ptr mesh(new Mesh());
			mesh->staticVertexData=quadVertexData;

			Mesh::SubMesh::Ptr subMesh(new Mesh::SubMesh());
			mesh->subMeshes.push_back(subMesh);
			subMesh->indexData=quadIndexData;

			Material::Ptr material(new Material());
			subMesh->material=material;
			engine->getSceneManager()->setAmbientColor(Color(Math::HALF,Math::HALF,Math::ONE,0));
			LightEffect effect;
			effect.ambient=Color(Math::ONE,Math::ONE,Math::ONE,Math::ONE);
			material->setLightEffect(effect);
			material->setLighting(true);

			MeshEntity::Ptr meshEntity(new MeshEntity(engine));
			meshEntity->load(mesh);
#			if defined(TOADLET_TADPOLE_FIXED_POINT)
				meshEntity->setTranslate(Math::milliToFixed(1500),0,-Math::intToFixed(6));
#			else
				meshEntity->setTranslate(1.5,0,-6);
#			endif
			engine->getSceneManager()->attach(meshEntity);
		}

		cameraEntity=CameraEntity::Ptr(new CameraEntity(engine));
		engine->getSceneManager()->attach(cameraEntity);
		if(width>=height){
#			if defined(TOADLET_PEEPER_FIXED_POINT)
				cameraEntity->setProjectionFovY(Math::degToRad(Math::intToFixed(45)),Math::div(Math::intToFixed(width),Math::intToFixed(height)),Math::milliToFixed(100),Math::intToFixed(100));
#			else
				cameraEntity->setProjectionFovY(Math::degToRad(45),(float)width/(float)height,.1,100);
#			endif
		}
		else{
#			if defined(TOADLET_PEEPER_FIXED_POINT)
				cameraEntity->setProjectionFovX(Math::degToRad(Math::intToFixed(45)),Math::div(Math::intToFixed(height),Math::intToFixed(width)),Math::milliToFixed(100),Math::intToFixed(100));
#			else
				cameraEntity->setProjectionFovX(Math::degToRad(45),(float)height/(float)width,.1,100);
#			endif
		}
		if(renderWindow!=NULL){
			cameraEntity->setViewport(Viewport(0,0,renderWindow->getWidth(),renderWindow->getHeight()));
		}
		else{
			cameraEntity->setViewport(Viewport(0,0,width,height));
		}

		lastTime=System::mtime();
	}

	~FlickTest(){
		if(engine!=NULL){
			delete engine;
			engine=NULL;
		}

		if(motionDetector!=NULL){
			if(motionFile!=NULL){
				fclose(motionFile);
			}
			motionDetector->shutdown();
			motionDetector->destroy();
			delete motionDetector;
			motionDetector=NULL;
		}

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
		Renderer::Status status=renderer->getStatus();
		if(status!=Renderer::STATUS_OK){
			TOADLET_LOG(NULL,Logger::LEVEL_ERROR,
				String("Renderer status is ")+(int)status);
		}

		engine->getSceneManager()->render(renderWindow,cameraEntity);

		renderer->swapRenderTargetBuffers();
	}

	void newMotionFile(){
		if(motionFile!=NULL){
			fclose(motionFile);
		}
		motionFile=fopen((String("/motion_")+(motionFileID++)+".txt").c_str(),"w");
	}

	void update(){
		uint64 time=System::mtime();
		if(time-lastTime>50){
			const MotionDetector::MotionData &motionData=motionDetector->pollMotionData();
			a.x=intToFixed(motionData.x);
			a.y=intToFixed(motionData.y);
			a.z=intToFixed(motionData.z+900);
			
			if(initial){
				initial=false;
				aoo=ao;
				ao=a;
				lastTime=time;
				return;
			}
			
			// Improved euler integration
			// v = v + 0.5dt * (ao + a)
			int dt=milliToFixed(time-lastTime);
			Vector3 tmp;
			Math::add(tmp,a,ao);
			Math::mul(tmp,dt>>1);
			Math::add(v,tmp);
			
			// Simple simson's rule
			// v = dt/3 * (aoo + 4ao + a)
			Math::mul(tmp,ao,4);
			Math::add(tmp,aoo);
			Math::add(tmp,a);
			Math::mul(tmp,dt/3);
				
			fprintf(motionFile,"Accel: %d,%d,%d\n",fixedToInt(a.x),fixedToInt(a.y),fixedToInt(a.z));
			fprintf(motionFile,"Euler Vel: %d,%d,%d\n",fixedToInt(v.x),fixedToInt(v.y),fixedToInt(v.z));
			fprintf(motionFile,"Simpson Vel: %d,%d,%d\n",fixedToInt(tmp.x),fixedToInt(tmp.y),fixedToInt(tmp.z));
			
			aoo=ao;
			ao=a;
			lastTime=time;
		}
	}
	
	void keyDown(int key){
		if(key=='f'){
			newMotionFile();
		}
	}
	
	void keyUp(int key){}
	void mouseMove(int x,int y){}
	void mouseDown(Mouse button){}
	void mouseUp(Mouse button){}
	void mouseScroll(int s){}
	void mouseLeave(){}
	void mouseEnter(){}
	void focusGained(){}
	void focusLost(){}
	void resized(int width,int height){}
	void closed(){}

	int width,height;
	RenderWindow *renderWindow;
	Renderer *renderer;
	MotionDetector *motionDetector;
	Engine *engine;

	uint64 lastTime;

	int motionFileID;
	FILE *motionFile;
	
	Vector3 v;
	Vector3 a;
	Vector3 ao;
	Vector3 aoo;
	bool initial;

	CameraEntity::Ptr cameraEntity;
};

int uniformMain(int argc,char **argv){
	FlickTest test;
	if(test.engine!=NULL){
		test.run();
	}
	
	return 0;
}
