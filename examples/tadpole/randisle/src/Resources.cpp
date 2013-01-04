#include "Resources.h"

Resources *Resources::instance=NULL;

tbyte *Resources::createNoise(TextureFormat *format,int scale,int seed,scalar brightnessScale,scalar brightnessOffset){
	int width=format->getWidth(),height=format->getHeight();
	tbyte *data=new tbyte[format->getDataSize()];

	Noise noise(4,scale,4,seed,256);
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

	return data;
}
