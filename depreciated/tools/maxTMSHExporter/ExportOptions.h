#ifndef EXPORTOPTIONS_H
#define EXPORTOPTIONS_H

class ExportOptions{
	public:
		BOOL mExportSkeleton;
		BOOL mExportAnimations;
		BOOL mExportGeometry;
		BOOL mExportVertexNormals;
		BOOL mExportVertexColors;
		BOOL mExportTextureCoordinates;
		BOOL mExportMaterials;
		BOOL mPreview;
		BOOL mSkipNullMaterials;
		char mPreviewTexturePath[1024];

		ExportOptions(){
			mExportSkeleton = FALSE;
			mExportAnimations = FALSE;
			mExportGeometry = TRUE;
			mExportVertexNormals = TRUE;
			mExportVertexColors = FALSE;
			mExportTextureCoordinates = TRUE;
			mExportMaterials = TRUE;
			mPreview = TRUE;
			mSkipNullMaterials = TRUE;
			mPreviewTexturePath[0] = 0;
		}

		virtual ~ExportOptions(){}
};

#endif