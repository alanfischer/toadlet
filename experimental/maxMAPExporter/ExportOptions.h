#ifndef EXPORTOPTIONS_H
#define EXPORTOPTIONS_H

class ExportOptions{
	public:
		ExportOptions(){
			mInvertYTexCoord=false;
			mTexturePath[0]=0;
		}

		virtual ~ExportOptions(){}

		bool mInvertYTexCoord;
		char mTexturePath[1024];
};

#endif