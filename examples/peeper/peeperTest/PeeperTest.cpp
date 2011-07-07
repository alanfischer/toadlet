#include "PeeperTest.h"

PeeperTest::PeeperTest():Application(){
}

PeeperTest::~PeeperTest(){
}

void PeeperTest::create(){
	Application::create();

	RenderCaps caps;
	getRenderDevice()->getRenderCaps(caps);
	int idealType=caps.idealVertexFormatType;
	VertexBufferAccessor vba;

	/// @todo: Update PeeperTest to have a shader path also
	TOADLET_ASSERT(caps.vertexFixedFunction && caps.fragmentFixedFunction);

	VertexFormat::ptr triVertexFormat=VertexFormat::ptr(getRenderDevice()->createVertexFormat());
	triVertexFormat->addElement(VertexFormat::Semantic_POSITION,0,idealType|VertexFormat::Format_COUNT_3);
	triVertexFormat->addElement(VertexFormat::Semantic_COLOR,0,VertexFormat::Format_TYPE_COLOR_RGBA);
	VertexBuffer::ptr triVertexBuffer=VertexBuffer::ptr(getRenderDevice()->createVertexBuffer());
	triVertexBuffer->create(Buffer::Usage_BIT_STATIC,Buffer::Access_BIT_WRITE,triVertexFormat,3);
	{
		vba.lock(triVertexBuffer);
		vba.set3(0,0,0,Math::ONE,0);
		vba.setARGB(0,1,0x000000FF);
		vba.set3(1,0,-Math::ONE,-Math::ONE,0);
		vba.setARGB(1,1,0x0000FF00);
		vba.set3(2,0,Math::ONE,-Math::ONE,0);
		vba.setARGB(2,1,0x00FF0000);
		vba.unlock();
	}
	triVertexData=VertexData::ptr(new VertexData(triVertexBuffer));
	triIndexData=IndexData::ptr(new IndexData(IndexData::Primitive_TRISTRIP,NULL,0,3));
	triRenderState=RenderState::ptr(getRenderDevice()->createRenderState());
	triRenderState->setMaterialState(MaterialState(false,true));

	VertexFormat::ptr quadVertexFormat=VertexFormat::ptr(getRenderDevice()->createVertexFormat());
	quadVertexFormat->addElement(VertexFormat::Semantic_POSITION,0,idealType|VertexFormat::Format_COUNT_3);
	VertexBuffer::ptr quadVertexBuffer=VertexBuffer::ptr(getRenderDevice()->createVertexBuffer());
	quadVertexBuffer->create(Buffer::Usage_BIT_STATIC,Buffer::Access_BIT_WRITE,quadVertexFormat,4);
	{
		vba.lock(quadVertexBuffer);
		vba.set3(0,0,-Math::ONE,Math::ONE,0);
		vba.set3(1,0,-Math::ONE,-Math::ONE,0);
		vba.set3(2,0,Math::ONE,Math::ONE,0);
		vba.set3(3,0,Math::ONE,-Math::ONE,0);
		vba.unlock();
	}
	quadVertexData=VertexData::ptr(new VertexData(quadVertexBuffer));
	quadIndexData=IndexData::ptr(new IndexData(IndexData::Primitive_TRISTRIP,NULL,0,4));
	quadRenderState=RenderState::ptr(getRenderDevice()->createRenderState());
	quadRenderState->setMaterialState(MaterialState(Math::ONE_VECTOR4));
}

void PeeperTest::destroy(){
	triRenderState->destroy();
	triVertexData->destroy();
	triIndexData->destroy();

	quadRenderState->destroy();
	quadVertexData->destroy();
	quadIndexData->destroy();

	Application::destroy();
}

void PeeperTest::render(RenderDevice *device){
	device->setViewport(Viewport(0,0,getWidth(),getHeight()));

	device->clear(RenderDevice::ClearType_BIT_COLOR|RenderDevice::ClearType_BIT_DEPTH,Math::ZERO_VECTOR4);
	device->beginScene();
		device->setMatrix(RenderDevice::MatrixType_PROJECTION,projectionMatrix);
		device->setMatrix(RenderDevice::MatrixType_VIEW,viewMatrix);

		Math::setMatrix4x4FromTranslate(modelMatrix,Vector3(-Math::fromMilli(1500),0,-Math::fromInt(6)));
		device->setMatrix(RenderDevice::MatrixType_MODEL,modelMatrix);
		device->setRenderState(triRenderState);
		device->renderPrimitive(triVertexData,triIndexData);

		Math::setMatrix4x4FromTranslate(modelMatrix,Vector3(Math::fromMilli(1500),0,-Math::fromInt(6)));
		device->setMatrix(RenderDevice::MatrixType_MODEL,modelMatrix);
		device->setAmbientColor(Vector4(0,0,Math::ONE,Math::ONE));
		device->setRenderState(quadRenderState);
		device->renderPrimitive(quadVertexData,quadIndexData);
	device->endScene();

	device->swap();
}

void PeeperTest::resized(int width,int height){
	if(width>=height){
		Math::setMatrix4x4FromPerspectiveY(projectionMatrix,Math::degToRad(Math::fromInt(45)),Math::div(Math::fromInt(width),Math::fromInt(height)),Math::fromInt(1),Math::fromInt(100));
	}
	else{
		Math::setMatrix4x4FromPerspectiveX(projectionMatrix,Math::degToRad(Math::fromInt(45)),Math::div(Math::fromInt(height),Math::fromInt(width)),Math::fromInt(1),Math::fromInt(100));
	}
}

int toadletMain(int argc,char **argv){
	PeeperTest app;
	app.create();
	app.start();
	app.destroy();
	return 0;
}

