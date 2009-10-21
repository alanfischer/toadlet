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

#include <toadlet/egg/image/ImageUtilities.h>
#include <toadlet/egg/math/Math.h>
#include <toadlet/egg/Error.h>

using namespace toadlet::egg;
using namespace toadlet::egg::math;

namespace toadlet{
namespace egg{
namespace image{

bool ImageUtilities::scaleImage(Image *input,Image *output,bool useWrapping){
	unsigned int i,j;
	float xIndex;
	float yIndex;
	unsigned int y1,y2,x1,x2;

	if(input->getDimension()!=Image::Dimension_D2 || (input->getFormat()&Image::Format_BIT_UINT_8)==0){
		Error::invalidParameters(Categories::TOADLET_EGG,
			"ImageUtilities::scaleImage: Not D2 and byte");
		return false;
	}

	//Do we want to make it tilable?
	unsigned int inputWidth=input->getWidth();
	unsigned int inputHeight=input->getHeight();
	if(useWrapping==false){
		inputWidth-=1;
		inputHeight-=1;
	}

	unsigned int outputWidth=output->getWidth();
	unsigned int outputHeight=output->getHeight();

	//Do the scaling
	for(j=0;j<outputHeight;++j){
		yIndex=(float)j*(float)(inputHeight)/(float)(outputHeight-1);
		y1=(unsigned char)(yIndex);
		y2=(unsigned char)(yIndex+1.0);
		yIndex-=y1;

		y1=y1%inputHeight;
		y2=y2%inputHeight;

		for(i=0;i<outputWidth;++i){
			xIndex=(float)i*(float)(inputWidth)/(float)(outputWidth-1);
			x1=(unsigned char)(xIndex);
			x2=(unsigned char)(xIndex+1.0);
			xIndex-=x1;

			x1=x1%inputWidth;
			x2=x2%inputWidth;

			float ul = (1.0f - xIndex) * (1.0f - yIndex);
			float ll = (1.0f - xIndex) * yIndex;
			float ur = xIndex * (1.0f - yIndex);
			float lr = xIndex * yIndex;

			Pixel<uint8> p1,p2,p3,p4;
			
			input->getPixel(p1,x1,y1);
			input->getPixel(p2,x2,y1);
			input->getPixel(p3,x1,y2);
			input->getPixel(p4,x2,y2);

			p1.r=(unsigned char)((float)p1.r*ul + (float)p2.r*ur + (float)p3.r*ll + (float)p4.r*lr);
			p1.g=(unsigned char)((float)p1.g*ul + (float)p2.g*ur + (float)p3.g*ll + (float)p4.g*lr);
			p1.b=(unsigned char)((float)p1.b*ul + (float)p2.b*ur + (float)p3.b*ll + (float)p4.b*lr);
			p1.a=(unsigned char)((float)p1.a*ul + (float)p2.a*ur + (float)p3.a*ll + (float)p4.a*lr);

			output->setPixel(p1,i,j);
		}
	}

	return true;
}

bool ImageUtilities::convertRGBtoRGBA(Image *input,Image *output,unsigned char clearColor1,unsigned char clearColor2,unsigned char clearColor3){
	unsigned int i;

	unsigned int inputWidth=input->getWidth();
	unsigned int inputHeight=input->getHeight();

	unsigned int outputWidth=output->getWidth();
	unsigned int outputHeight=output->getHeight();;

	if(inputWidth!=outputWidth || inputHeight!=outputHeight){
		Error::invalidParameters(Categories::TOADLET_EGG,
			"ImageUtilities::convertRGBtoRGBA: input & output sizes do not match");
		return false;
	}

	if(	input->getDimension()!=Image::Dimension_D2 ||
		output->getDimension()!=Image::Dimension_D2 ||
		input->getFormat()!=Image::Format_RGB_8 ||
		output->getFormat()!=Image::Format_RGBA_8)
	{
		Error::invalidParameters(Categories::TOADLET_EGG,
			"ImageUtilities::convertRGBtoRGBA: Not 2D or not Format_RGB_8 & Format_RGBA_8");
		return false;
	}

	unsigned char *inputData=input->getData();
	unsigned char *outputData=output->getData();

	for(i=0;i<inputWidth*inputHeight;++i){
		outputData[i*4+0]=inputData[i*3+0];
		outputData[i*4+1]=inputData[i*3+1];
		outputData[i*4+2]=inputData[i*3+2];

		if(inputData[i*3]==clearColor1 && inputData[i*3+1]==clearColor2 && inputData[i*3+2]==clearColor3)
			outputData[i*4+3]=0;
		else
			outputData[i*4+3]=255;
	}

	return true;
}

bool ImageUtilities::makeNormalMapFromBumpMap(Image *normalmap,Image *bumpmap,float scale){
	if(bumpmap->getDimension()!=Image::Dimension_D2 || (bumpmap->getFormat()&Image::Format_BIT_UINT_8)==0){
		Error::invalidParameters(Categories::TOADLET_EGG,
			"ImageUtilities::makeNormalMapFromBumpMap: Not D2 and byte");
		return false;
	}

	unsigned int width=bumpmap->getWidth();
	unsigned int height=bumpmap->getHeight();

	normalmap->reallocate(bumpmap->getDimension(),Image::Format_RGBA_8,width,height,1,false);

	float gbfact=1;
	float div=3;

	if(bumpmap->getFormat()==Image::Format_L_8 || bumpmap->getFormat()==Image::Format_LA_8){
		gbfact=0;
		div=1;
	}

	unsigned int i,j;
	for(i=1;i<width;++i){
		for(j=1;j<height;++j){
			Pixel<uint8> p1,p2,p3;

			bumpmap->getPixel(p1,i,j);
			bumpmap->getPixel(p2,i-1,j);
			bumpmap->getPixel(p3,i,j-1);

			float h1,h2,h3;
			h1=((float)p1.r+(float)p1.g*gbfact+(float)p1.b*gbfact)/div;
			h2=((float)p2.r+(float)p2.g*gbfact+(float)p2.b*gbfact)/div;
			h3=((float)p3.r+(float)p3.g*gbfact+(float)p3.b*gbfact)/div;

			float originalHeight=h1;

			h1*=scale;
			h2*=scale;
			h3*=scale;

			Vector3 point1(i,j,h1),point2(i-1,j,h2),point3(i,j-1,h3);
			Vector3 normal;
			Math::cross(normal,point2-point1,point3-point1);
			Math::normalize(normal);

			p1.r=(unsigned char)((normal.x+1)*127.5f);
			p1.g=(unsigned char)((normal.y+1)*127.5f);
			p1.b=(unsigned char)((normal.z+1)*127.5f);
			p1.a=(unsigned char)originalHeight;
			normalmap->setPixel(p1,i,j);
		}
	}

	Pixel<uint8> pixel;
	for(i=0;i<width;++i){
		normalmap->getPixel(pixel,i,height-1);
		normalmap->setPixel(pixel,i,0);
	}

	for(i=0;i<height;++i){
		normalmap->getPixel(pixel,width-1,i);
		normalmap->setPixel(pixel,0,i);
	}

	return true;
}

bool ImageUtilities::makeDiffuseAlphaMapFromDiffuseOpacity(Image *output,Image *diffuse,Image *opacity){
	if(diffuse->getDimension()!=Image::Dimension_D2 || (diffuse->getFormat()&Image::Format_BIT_UINT_8)==0){
		Error::invalidParameters(Categories::TOADLET_EGG,
			"ImageUtilities::makeDiffuseAlphaMapFromDiffuseOpacity: Not 2D and byte");
		return false;
	}

	unsigned int diffuseWidth=diffuse->getWidth();
	unsigned int diffuseHeight=diffuse->getHeight();

	unsigned int opacityWidth=opacity->getWidth();
	unsigned int opacityHeight=opacity->getHeight();

	if(diffuseWidth!=opacityWidth || diffuseHeight!=opacityHeight){
		Error::invalidParameters(Categories::TOADLET_EGG,
			"ImageUtilities::makeDiffuseAlphaMapFromDiffuseOpacity: width & height do not match");
		return false;
	}

	output->reallocate(diffuse->getDimension(),Image::Format_RGBA_8,diffuseWidth,diffuseHeight,1,false);

	unsigned int i,j;
	for(i=0;i<diffuseWidth;++i){
		for(j=0;j<diffuseHeight;++j){
			Pixel<uint8> dp,op;

			diffuse->getPixel(dp,i,j);
			opacity->getPixel(op,i,j);

			dp.a=(op.r+op.g+op.b)/3;

			output->setPixel(dp,i,j);
		}
	}

	return true;
}

}
}
}
