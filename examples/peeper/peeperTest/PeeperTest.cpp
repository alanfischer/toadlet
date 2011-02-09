#include "PeeperTest.h"

PeeperTest::PeeperTest():Application(){
}

PeeperTest::~PeeperTest(){
}

void PeeperTest::create(){
	Application::create();

	int typeBit=getRenderer()->getCapabilitySet().idealVertexFormatBit;
	VertexBufferAccessor vba;

	VertexFormat::ptr triVertexFormat(new VertexFormat(2));
	triVertexFormat->addVertexElement(VertexElement(VertexElement::Type_POSITION,VertexElement::Format(typeBit|VertexElement::Format_BIT_COUNT_3)));
	triVertexFormat->addVertexElement(VertexElement(VertexElement::Type_COLOR_DIFFUSE,VertexElement::Format(VertexElement::Format_COLOR_RGBA)));
	VertexBuffer::ptr triVertexBuffer=VertexBuffer::ptr(getRenderer()->createVertexBuffer());
	triVertexBuffer->create(Buffer::UsageFlags_STATIC,Buffer::AccessType_WRITE_ONLY,triVertexFormat,3);
	{
		vba.lock(triVertexBuffer);
		vba.set3(0,0,0,Math::ONE,0);
		vba.setABGR(0,1,0x000000FF);
		vba.set3(1,0,-Math::ONE,-Math::ONE,0);
		vba.setABGR(1,1,0x0000FF00);
		vba.set3(2,0,Math::ONE,-Math::ONE,0);
		vba.setABGR(2,1,0x00FF0000);
		vba.unlock();
	}
	triVertexData=VertexData::ptr(new VertexData(triVertexBuffer));
	triIndexData=IndexData::ptr(new IndexData(IndexData::Primitive_TRISTRIP,NULL,0,3));

	VertexFormat::ptr quadVertexFormat(new VertexFormat(1));
	quadVertexFormat->addVertexElement(VertexElement(VertexElement::Type_POSITION,VertexElement::Format(typeBit|VertexElement::Format_BIT_COUNT_3)));
	VertexBuffer::ptr quadVertexBuffer=VertexBuffer::ptr(getRenderer()->createVertexBuffer());
	quadVertexBuffer->create(Buffer::UsageFlags_STATIC,Buffer::AccessType_WRITE_ONLY,quadVertexFormat,4);
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
}

void PeeperTest::destroy(){
	triVertexData->destroy();
	triIndexData->destroy();

	quadVertexData->destroy();
	quadIndexData->destroy();

	Application::destroy();
}

void PeeperTest::render(Renderer *renderer){
	renderer->setViewport(Viewport(0,0,getWidth(),getHeight()));

	renderer->clear(Renderer::ClearFlag_COLOR|Renderer::ClearFlag_DEPTH,Colors::BLACK);
	renderer->beginScene();
		renderer->setProjectionMatrix(projectionMatrix);
		renderer->setViewMatrix(viewMatrix);
		Math::setMatrix4x4FromTranslate(modelMatrix,Vector3(-Math::fromMilli(1500),0,-Math::fromInt(6)));
		renderer->setModelMatrix(modelMatrix);
		renderer->renderPrimitive(triVertexData,triIndexData);

		Math::setMatrix4x4FromTranslate(modelMatrix,Vector3(Math::fromMilli(1500),0,-Math::fromInt(6)));
		renderer->setModelMatrix(modelMatrix);
		renderer->setAmbientColor(Color(Math::HALF,Math::HALF,Math::ONE,0));
		LightEffect effect(Color(Math::ONE,Math::ONE,Math::ONE,Math::ONE));
		renderer->setLightEffect(effect);
		renderer->setLighting(true);
		renderer->renderPrimitive(quadVertexData,quadIndexData);
		renderer->setLighting(false);
	renderer->endScene();

	renderer->swap();
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
	app.start(true);
	app.destroy();
	return 0;
}

