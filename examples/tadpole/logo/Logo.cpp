#include "Logo.h"

class GravityFollower:public NodeListener,MotionDeviceListener{
public:
	GravityFollower(MotionDevice *device){
		mDevice=device;
		mDevice->setListener(this);
	}

	void nodeDestroyed(Node *node){
		mDevice->setListener(NULL);
	}

	void logicUpdated(Node *node,int dt){
		mLastTranslate.set(mTranslate);
		mLastRotate.set(mRotate);

		mMotionMutex.lock();

			Vector3 up;
			// When the phone is vertical, we're at 0,-1,0
			// When the phone is horizontal, we're at 1,0,0
			// Grab just the x,y component of this, and move it to x,z, since we are looking along the y.
			// Store the y component to use for calculating our eye height
			up.set(mMotionData.acceleration);
			scalar z=up.z;
			up.z=up.y;
			up.y=0;
			if(Math::normalizeCarefully(up,0)){
				Vector3 eye(0,-z-Math::ONE,-z);
				Math::normalize(eye);
				Math::mul(eye,Math::fromInt(150));

				((CameraNode*)node)->setLookAt(eye,Math::ZERO_VECTOR3,up);
				mTranslate.set(node->getTranslate());
				mRotate.set(node->getRotate());
			}

		mMotionMutex.unlock();
	}
	
	void frameUpdated(Node *node,int dt){
		Vector3 translate;
		Math::lerp(translate,mLastTranslate,mTranslate,node->getScene()->getLogicFraction());
		node->setTranslate(translate);
		Quaternion rotate;
		Math::slerp(rotate,mLastRotate,mRotate,node->getScene()->getLogicFraction());
		node->setRotate(rotate);
	}

	void motionDetected(const MotionDevice::MotionData &motionData){
		mMotionMutex.lock();
		mMotionData.set(motionData);
		mMotionMutex.unlock();
	}

	MotionDevice *mDevice;
	Mutex mMotionMutex;
	MotionDevice::MotionData mMotionData;
	Vector3 mTranslate,mLastTranslate;
	Quaternion mRotate,mLastRotate;
};

Logo::Logo():Application(){
}

Logo::~Logo(){
}
Shader::ptr vs,fs,gs;
void Logo::create(){
	Application::create("gl");

	mEngine->setDirectory("../../../data");

	scene=Scene::ptr(new Scene(mEngine));

	DecalShadowSceneRenderer::ptr sceneRenderer(new DecalShadowSceneRenderer(scene));
	sceneRenderer->setPlane(Plane(Math::Z_UNIT_VECTOR3,-30));
	scene->setSceneRenderer(sceneRenderer);

	LightNode::ptr light=getEngine()->createNodeType(LightNode::type(),scene);
	LightState state;
	state.type=LightState::Type_DIRECTION;
	state.direction=Math::NEG_Z_UNIT_VECTOR3;
	light->setLightState(state);
	scene->getRoot()->attach(light);

 	meshNode=getEngine()->createNodeType(MeshNode::type(),scene);
	meshNode->setMesh("lt.xmsh");
	meshNode->getController()->start();
	meshNode->getController()->setCycling(Controller::Cycling_REFLECT);
	scene->getRoot()->attach(meshNode);

	cameraNode=getEngine()->createNodeType(CameraNode::type(),scene);
	cameraNode->setLookAt(Vector3(0,-Math::fromInt(150),0),Math::ZERO_VECTOR3,Math::Z_UNIT_VECTOR3);
	cameraNode->setClearColor(Colors::BLUE);
	scene->getRoot()->attach(cameraNode);

#if 1
	const char *vc=
		"varying vec4 color;\n" \
		"void main(){\n" \
		" gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;\n" \
		" color = vec4(gl_Normal.x,gl_Normal.y,gl_Normal.z,1.0);\n" \
		"}";
	const char *fc=
		"varying vec4 color;\n" \
		"void main(){\n" \
		" gl_FragColor = color;\n" \
		"}";
	const char *gc=
		"#version 150\n" \
		"layout(triangles) in;\n" \
		"layout(line_strip, max_vertices = 3) out\n;" \
		"void main(){\n" \
		" for(int i=0;i<gl_in.length();i++){\n" \
		"  gl_Position=gl_in[(i+1)%gl_in.length()].gl_Position;\n" \
		"  EmitVertex();\n" \
		" }\n" \
		" EndPrimitive();\n" \
		"}";
#else
	const char *vc=
		"struct VIN{\n" \
			"float4 position : POSITION;\n" \
			"float3 normal : NORMAL;\n" \
		"};\n" \
		"struct VOUT{\n" \
			"float4 position : SV_POSITION;\n" \
			"float4 color : COLOR;\n" \
		"};\n" \
		"float4x4 ModelViewProjectionMatrix;\n" \
		"VOUT main(VIN vin){\n" \
		"	VOUT vout;\n" \
		"	vout.position=mul(vin.position,ModelViewProjectionMatrix);\n" \
		"	vout.color=float4(vin.normal.x,vin.normal.y,vin.normal.z,1.0);\n" \
		"	return vout;\n" \
		"}";
	const char *fc=
		"struct PIN{\n" \
			"float4 position : SV_POSITION;\n" \
			"float4 color: COLOR;\n" \
		"};\n" \
		"float4 main(PIN pin): SV_TARGET{" \
		"	return pin.color;\n" \
		"}";
	const char *gc=
		"struct GIN{\n" \
			"float4 position : POSITION;\n" \
		"};\n" \
		"[maxvertexcount(4)]\n" \
		"void main(GIN point[1],inout TriangleStream<PS_INPUT> triStream){" \
			"PS_INPUT v;" \
			"triStream.Append(v);" \
		"}";
#endif
vs=getEngine()->getShaderManager()->createShader(Shader::ShaderType_VERTEX,vc);
fs=getEngine()->getShaderManager()->createShader(Shader::ShaderType_FRAGMENT,fc);
//gs=getEngine()->getShaderManager()->createShader(Shader::ShaderType_GEOMETRY,gc);

// Only looks good if running on device, in simulator its always a top down view
#if 0
	MotionDevice *motionDevice=getMotionDevice();
	if(motionDevice!=NULL){
		cameraNode->addNodeListener(NodeListener::ptr(new GravityFollower(motionDevice)));
		motionDevice->startup();
	}
#endif
}

void Logo::destroy(){
	scene->destroy();

	Application::destroy();
}

void Logo::resized(int width,int height){
	if(cameraNode!=NULL && width>0 && height>0){
		if(width>=height){
			cameraNode->setProjectionFovY(Math::degToRad(Math::fromInt(45)),Math::div(Math::fromInt(width),Math::fromInt(height)),Math::fromInt(1),Math::fromInt(200));
		}
		else{
			cameraNode->setProjectionFovX(Math::degToRad(Math::fromInt(45)),Math::div(Math::fromInt(height),Math::fromInt(width)),Math::fromInt(1),Math::fromInt(200));
		}
		cameraNode->setViewport(Viewport(0,0,width,height));
	}
}

void Logo::render(RenderDevice *renderDevice){
	renderDevice->beginScene();
renderDevice->setShader(Shader::ShaderType_VERTEX,vs);
renderDevice->setShader(Shader::ShaderType_FRAGMENT,fs);
//renderDevice->setShader(Shader::ShaderType_GEOMETRY,gs);
		cameraNode->render(renderDevice);
	renderDevice->endScene();
	renderDevice->swap();
}

void Logo::update(int dt){
	scene->update(dt);
}

int toadletMain(int argc,char **argv){
	Logo app;
	app.create();
	app.start();
	app.destroy();
	return 0;
}
