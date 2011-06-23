#include "Logo.h"

#include <toadlet/peeper/ShaderState.h>

/*

WHAT DO WE KNOW?

We know how D3D9,D3D10,GLSL lay out constants: (Which I will rename to Uniform because Constant Variables is wtf)

D3D9:
	Set Uniform by CachedIndex

D3D10:
	Set BufferContents by CachedIndex;
	Set Buffer by CachedIndex;

GL:
	Set Uniform by CachedIndex

GLUBO:
	Set Uniform by CachedIndex
	Set BufferContents by CachedIndex;
	Set Buffer by CachedIndex;

So we need a Buffer abstraction to cover all those cases.

D3D10:
	Create a D3D10UniformBuffer
	Find BufferIndex in Shader
	Find UniformIndex in Buffer

	Set Buffer Contents
	Set Buffer to Shader

D3D9:
	Create a D3D9UniformBuffer
	Find UniformIndex in Shader

	Set Buffer Contents
	Set Uniforms from Buffer Contents

GL:
	Create a GLUniformBuffer
	Find UniformIndex in Program

	Set Buffer Contents
	Set Uniforms from Buffer Contents

GLUBO:
	Create a GLBuffer (our class for anything related to vertex_buffer_objects
	Find BufferIndex in Program
	Find UniformOffset in Buffer
	
	Set Buffer Contents
	Set Buffer to Program

Buffer interface must:
	Set Buffer Contents

Should the Number of Buffers created be up to the User?
Or should something in the reflection tell us how many to allocate and what size?

I think we should be able to query for the number of buffers, and size.  Then what is actually done is up to the user.
That way they could create their own to force D3D9/GL to be more efficient instead of only getting 1 buffer that does everything, and needs to be reset every frame.

... Constant Buffers ... SamplerStates ... Resources ...

GL - Resources are in ConstantBuffers and Texture/Sampler are tied together
D3D9 - Resources 

ShaderState{
	Shaders
	UniformBuffers(Buffers)
	Textures(Resources)
	SamplerStates(extras)

	void setVariable(...);
	void setTexture(...);
	void setSampler(...);
}

RenderState{
	RenderStates
	Textures
	SamplerStates
}

Should a UniformBuffer be created with a Shader?  That would let you obtain the mappings, at least for D3D9/D3D10
NO - a UniformBuffer doesn't need to know the mappings.  Its job is simply to map a block of memory, and maybe set individual parts
	(Or would we need it for D3D9??)  Uniform buffer interface should not be concerned about shader reflection.



*/

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
Shader::ptr vs,fs;
ShaderState::ptr ss;
VariableBuffer::ptr vb;

String sp[]={
	"glsl",
	"hlsl"
};
String vsc[]={
	"#version 150\n" \
	"in vec4 POSITION;\n" \
	"in vec3 NORMAL;\n" \
	"out vec4 color;\n" \
	"uniform mat4 ModelViewProjectionMatrix;;\n" \
	"void main(){\n" \
		"gl_Position = ModelViewProjectionMatrix * POSITION;\n" \
		"color = vec4(NORMAL.x,NORMAL.y,NORMAL.z,1.0);\n" \
	"}",

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
	"}"
};
String fsc[]={
	"in vec4 color;\n" \
	"void main(){\n" \
		"gl_FragColor = color;\n" \
	"}",

	"struct PIN{\n" \
		"float4 position : SV_POSITION;\n" \
		"float4 color: COLOR;\n" \
	"};\n" \
	"float4 main(PIN pin): SV_TARGET{" \
	"	return pin.color;\n" \
	"}"
};

void Logo::create(){
	Application::create("d3d10");

	mEngine->setDirectory("../../../data");

	scene=Scene::ptr(new Scene(mEngine));

//	DecalShadowSceneRenderer::ptr sceneRenderer(new DecalShadowSceneRenderer(scene));
//	sceneRenderer->setPlane(Plane(Math::Z_UNIT_VECTOR3,-30));
//	scene->setSceneRenderer(sceneRenderer);

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

vs=getEngine()->getShaderManager()->createShader(Shader::ShaderType_VERTEX,sp,vsc,2);
fs=getEngine()->getShaderManager()->createShader(Shader::ShaderType_FRAGMENT,sp,fsc,2);
VariableBufferFormat::ptr vbf;
	for(int i=0;i<meshNode->getNumSubMeshes();++i){
		meshNode->getSubMesh(i)->material->setShader(Shader::ShaderType_VERTEX,vs);
		meshNode->getSubMesh(i)->material->setShader(Shader::ShaderType_FRAGMENT,fs);
vbf=meshNode->getSubMesh(i)->material->getShaderState()->getVariableBufferFormat(Shader::ShaderType_VERTEX,0);
	}

vb=getEngine()->getBufferManager()->createVariableBuffer(Buffer::Usage_BIT_DYNAMIC,Buffer::Access_BIT_WRITE,vbf);

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
#if 1
Matrix4x4 shaderMatrix;
shaderMatrix.set(cameraNode->getProjectionMatrix()*cameraNode->getViewMatrix());
vb->update((tbyte*)shaderMatrix.getData(),0,16*4);
#endif

	renderDevice->beginScene();
renderDevice->setShaderState(meshNode->getSubMesh(0)->material->getShaderState());
renderDevice->setBuffer(0,vb);
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
