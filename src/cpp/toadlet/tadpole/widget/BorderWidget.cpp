/********** Copyright header - do not remove **********
 *
 * The Toadlet Engine
 *
 * Copyright 2009, Lightning Toads Productions, LLC
 *
 * Author(s): Alan Fischer, Andrew Fischer
 *
 * This file is part of The Toadlet Engine.
 *
 * The Toadlet Engine is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * The Toadlet Engine is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.

 * You should have received a copy of the GNU Lesser General Public License
 * along with The Toadlet Engine.  If not, see <http://www.gnu.org/licenses/>.
 *
 ********** Copyright header - do not remove **********/

#include <toadlet/peeper/CapabilitySet.h>
#include <toadlet/peeper/Colors.h>
#include <toadlet/tadpole/Engine.h>
#include <toadlet/tadpole/widget/BorderWidget.h>
#include <toadlet/tadpole/widget/ParentWidget.h>

using namespace toadlet::egg;
using namespace toadlet::peeper;

namespace toadlet{
namespace tadpole{
namespace widget{

BorderWidget::BorderWidget(Engine *engine):RenderableWidget(engine),
	mLightColor(Colors::WHITE),
	mDarkColor(Colors::GREY)
	//mVertexData,
	//mIndexData
{
	mType|=Type_BORDER;

	setLayout(Layout_FILL);

	VertexBuffer::ptr vertexBuffer=mEngine->loadVertexBuffer(VertexBuffer::ptr(new VertexBuffer(Buffer::UsageType_DYNAMIC,Buffer::AccessType_WRITE_ONLY,mEngine->getVertexFormats().POSITION_COLOR,12)));
	mVertexData=VertexData::ptr(new VertexData(vertexBuffer));

	IndexBuffer::ptr indexBuffer=mEngine->loadIndexBuffer(IndexBuffer::ptr(new IndexBuffer(Buffer::UsageType_STATIC,Buffer::AccessType_WRITE_ONLY,IndexBuffer::IndexFormat_UINT_16,24)));
	mIndexData=IndexData::ptr(new IndexData(IndexData::Primitive_TRIS,indexBuffer,0,indexBuffer->getSize()));
	{
		uint16 *data=(uint16*)indexBuffer->lock(Buffer::LockType_WRITE_ONLY);

		int i=0;
		data[i++]=0;
		data[i++]=1;
		data[i++]=2;
		data[i++]=2;
		data[i++]=1;
		data[i++]=3;
		data[i++]=2;
		data[i++]=3;
		data[i++]=4;
		data[i++]=4;
		data[i++]=3;
		data[i++]=5;

		data[i++]=6;
		data[i++]=7;
		data[i++]=8;
		data[i++]=7;
		data[i++]=9;
		data[i++]=8;
		data[i++]=8;
		data[i++]=9;
		data[i++]=10;
		data[i++]=10;
		data[i++]=9;
		data[i++]=11;

		indexBuffer->unlock();
	}
}

BorderWidget::~BorderWidget(){
}

void BorderWidget::setColors(const Color &lightColor,const Color &darkColor){
	mLightColor.set(lightColor);
	mDarkColor.set(darkColor);

	rebuild();
}

void BorderWidget::setSize(int width,int height){
	Super::setSize(width,height);

	rebuild();
}

void BorderWidget::render(Renderer *renderer){
	renderer->renderPrimitive(mVertexData,mIndexData);
}

void BorderWidget::rebuild(){
	uint32 lightColor(mLightColor.getABGR());
	uint32 darkColor(mDarkColor.getABGR());

	scalar left=0;
	scalar right=Math::fromInt(mWidth);
	scalar top=0;
	scalar bottom=Math::fromInt(mHeight);
	scalar thick=-Math::fromInt(2);

	vba.lock(mVertexData->getVertexBuffer(0),Buffer::LockType_WRITE_ONLY);

	vba.set3(	0,0, left,bottom,0);
	vba.setABGR(0,1, lightColor);

	vba.set3(	1,0, left+thick,bottom-thick,0);
	vba.setABGR(1,1, lightColor);

	vba.set3(	2,0, left,top,0);
	vba.setABGR(2,1, lightColor);

	vba.set3(	3,0, left+thick,top+thick,0);
	vba.setABGR(3,1, lightColor);

	vba.set3(	4,0, right,top,0);
	vba.setABGR(4,1, lightColor);

	vba.set3(	5,0, right-thick,top+thick,0);
	vba.setABGR(5,1, lightColor);

	vba.set3(	6,0, right,top,0);
	vba.setABGR(6,1, darkColor);

	vba.set3(	7,0, right-thick,top+thick,0);
	vba.setABGR(7,1, darkColor);

	vba.set3(	8,0, right,bottom,0);
	vba.setABGR(8,1, darkColor);

	vba.set3(	9,0, right-thick,bottom-thick,0);
	vba.setABGR(9,1, darkColor);

	vba.set3(	10,0, left,bottom,0);
	vba.setABGR(10,1, darkColor);

	vba.set3(	11,0, left+thick,bottom-thick,0);
	vba.setABGR(11,1, darkColor);

	vba.unlock();
}

}
}
}

