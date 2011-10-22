#include "Resources.h"

Resources *Resources::instance=NULL;

Image::ptr Resources::createPoint(int width,int height){
	Image::ptr image(Image::createAndReallocate(Image::Dimension_D2,Image::Format_A_8,width,height));

	uint8 *data=image->getData();
	int x=0,y=0;
	for(y=0;y<height;y++){
		for(x=0;x<width;x++){
			float v=1.0;
			v=v*(Math::length(Vector2(x-width/2,y-height/2))/(width/2));
			if(v<0) v=0;
			if(v>1) v=1;
			v=pow(v,1.25f);

			data[y*width+x]=255*v;
		}
	}

	return image;
}

Image::ptr Resources::createNoise(int width,int height,int scale,int seed,scalar brightnessScale,scalar brightnessOffset){
	Image::ptr image(Image::createAndReallocate(Image::Dimension_D2,Image::Format_RGB_8,width,height));

	Noise noise(4,scale,4,seed,256);
	uint8 *data=image->getData();
	int x=0,y=0;
	for(y=0;y<height;y++){
		for(x=0;x<width;x++){
			float n=(noise.tileablePerlin2((float)x/(float)width,(float)y/(float)height,1,1)*0.5 + 0.5);
			n=Math::madd(n,brightnessScale,brightnessOffset);
			n=Math::clamp(0,Math::ONE,n);

			data[(y*width+x)*3+0]=n*255;
			data[(y*width+x)*3+1]=n*255;
			data[(y*width+x)*3+2]=n*255;
		}
	}

	return image;
}
