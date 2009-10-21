#ifndef EXPORTOPTIONS_H
#define EXPORTOPTIONS_H

class ExportOptions{
	public:
		BOOL mRootTransform;

		ExportOptions(){
			mRootTransform = TRUE;
		}

		virtual ~ExportOptions(){}
};

#endif