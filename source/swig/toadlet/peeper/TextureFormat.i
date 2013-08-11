%{
#	include <toadlet/peeper/TextureFormat.h>
%}

namespace toadlet{
namespace peeper{

class TextureFormat{
public:
	enum Dimension{
		Dimension_UNKNOWN=0,
		Dimension_D1,
		Dimension_D2,
		Dimension_D3,
		Dimension_CUBE,
	};

	enum CubeSide{
		CubeSide_POSITIVE_X=0,
		CubeSide_NEGATIVE_X=1,
		CubeSide_POSITIVE_Y=2,
		CubeSide_NEGATIVE_Y=3,
		CubeSide_POSITIVE_Z=4,
		CubeSide_NEGATIVE_Z=5,
		CubeSide_MAX=6,
	};

	enum Format{
		Format_UNKNOWN=				0,

		// Format semantics
		Format_MASK_SEMANTICS=		0xFF,
		Format_SEMANTIC_L=			1,
		Format_SEMANTIC_A=			2,
		Format_SEMANTIC_LA=			3,
		Format_SEMANTIC_R=			4,
		Format_SEMANTIC_RG=			5,
		Format_SEMANTIC_RGB=		6,
		Format_SEMANTIC_BGR=		7,
		Format_SEMANTIC_RGBA=		8,
		Format_SEMANTIC_BGRA=		9,
		Format_SEMANTIC_ARGB=		10,
		Format_SEMANTIC_ABGR=		11,
		Format_SEMANTIC_YUV=		12,
		Format_SEMANTIC_DEPTH=		13,

		// Format types
		Format_SHIFT_TYPES=			8,
		Format_MASK_TYPES=			0xFF00,
		Format_TYPE_UINT_8=			1<<Format_SHIFT_TYPES,
		Format_TYPE_UINT_16=		2<<Format_SHIFT_TYPES,
		Format_TYPE_UINT_24=		3<<Format_SHIFT_TYPES,
		Format_TYPE_UINT_32=		4<<Format_SHIFT_TYPES,
		Format_TYPE_FLOAT_16=		5<<Format_SHIFT_TYPES,
		Format_TYPE_FLOAT_32=		6<<Format_SHIFT_TYPES,
		Format_TYPE_UINT_5_6_5=		7<<Format_SHIFT_TYPES,
		Format_TYPE_UINT_5_5_5_1=	8<<Format_SHIFT_TYPES,
		Format_TYPE_UINT_1_5_5_5=	9<<Format_SHIFT_TYPES,
		Format_TYPE_UINT_4_4_4_4=	10<<Format_SHIFT_TYPES,
		Format_TYPE_DXT1=			11<<Format_SHIFT_TYPES,
		Format_TYPE_DXT2=			12<<Format_SHIFT_TYPES,
		Format_TYPE_DXT3=			13<<Format_SHIFT_TYPES,
		Format_TYPE_DXT4=			14<<Format_SHIFT_TYPES,
		Format_TYPE_DXT5=			15<<Format_SHIFT_TYPES,
		Format_TYPE_YUY2=			16<<Format_SHIFT_TYPES,
		Format_TYPE_NV12=			17<<Format_SHIFT_TYPES,

		Format_L_8=					Format_SEMANTIC_L|Format_TYPE_UINT_8,
		Format_R_8=					Format_SEMANTIC_R|Format_TYPE_UINT_8,
		Format_A_8=					Format_SEMANTIC_A|Format_TYPE_UINT_8,
		Format_LA_8=				Format_SEMANTIC_LA|Format_TYPE_UINT_8,
		Format_RG_8=				Format_SEMANTIC_RG|Format_TYPE_UINT_8,
		Format_RGB_8=				Format_SEMANTIC_RGB|Format_TYPE_UINT_8,
		Format_BGR_8=				Format_SEMANTIC_BGR|Format_TYPE_UINT_8,
		Format_RGBA_8=				Format_SEMANTIC_RGBA|Format_TYPE_UINT_8,
		Format_BGRA_8=				Format_SEMANTIC_BGRA|Format_TYPE_UINT_8,
		Format_ARGB_8=				Format_SEMANTIC_ARGB|Format_TYPE_UINT_8,
		Format_ABGR_8=				Format_SEMANTIC_ABGR|Format_TYPE_UINT_8,
		Format_DEPTH_8=				Format_SEMANTIC_DEPTH|Format_TYPE_UINT_8,
		Format_DEPTH_16=			Format_SEMANTIC_DEPTH|Format_TYPE_UINT_16,
		Format_DEPTH_24=			Format_SEMANTIC_DEPTH|Format_TYPE_UINT_24,
		Format_DEPTH_32=			Format_SEMANTIC_DEPTH|Format_TYPE_UINT_32,
		Format_L_F32=				Format_SEMANTIC_L|Format_TYPE_FLOAT_32,
		Format_A_F32=				Format_SEMANTIC_A|Format_TYPE_FLOAT_32,
		Format_LA_F32=				Format_SEMANTIC_LA|Format_TYPE_FLOAT_32,
		Format_RGB_F32=				Format_SEMANTIC_RGB|Format_TYPE_FLOAT_32,
		Format_BGR_F32=				Format_SEMANTIC_BGR|Format_TYPE_FLOAT_32,
		Format_RGBA_F32=			Format_SEMANTIC_RGBA|Format_TYPE_FLOAT_32,
		Format_BGRA_F32=			Format_SEMANTIC_BGRA|Format_TYPE_FLOAT_32,
		Format_ARGB_F32=			Format_SEMANTIC_ARGB|Format_TYPE_FLOAT_32,
		Format_ABGR_F32=			Format_SEMANTIC_ABGR|Format_TYPE_FLOAT_32,
		Format_DEPTH_F32=			Format_SEMANTIC_DEPTH|Format_TYPE_FLOAT_32,
		Format_RGB_5_6_5=			Format_SEMANTIC_RGB|Format_TYPE_UINT_5_6_5,
		Format_BGR_5_6_5=			Format_SEMANTIC_BGR|Format_TYPE_UINT_5_6_5,
		Format_RGBA_5_5_5_1=		Format_SEMANTIC_RGBA|Format_TYPE_UINT_5_5_5_1,
		Format_BGRA_5_5_5_1=		Format_SEMANTIC_BGRA|Format_TYPE_UINT_5_5_5_1,
		Format_ARGB_1_5_5_5=		Format_SEMANTIC_ARGB|Format_TYPE_UINT_1_5_5_5,
		Format_ABGR_1_5_5_5=		Format_SEMANTIC_ABGR|Format_TYPE_UINT_1_5_5_5,
		Format_RGBA_4_4_4_4=		Format_SEMANTIC_RGBA|Format_TYPE_UINT_4_4_4_4,
		Format_BGRA_4_4_4_4=		Format_SEMANTIC_BGRA|Format_TYPE_UINT_4_4_4_4,
		Format_ARGB_4_4_4_4=		Format_SEMANTIC_ARGB|Format_TYPE_UINT_4_4_4_4,
		Format_ABGR_4_4_4_4=		Format_SEMANTIC_ABGR|Format_TYPE_UINT_4_4_4_4,
		Format_RGB_DXT1=			Format_SEMANTIC_RGB|Format_TYPE_DXT1,
		Format_RGBA_DXT2=			Format_SEMANTIC_RGBA|Format_TYPE_DXT2,
		Format_RGBA_DXT3=			Format_SEMANTIC_RGBA|Format_TYPE_DXT3,
		Format_RGBA_DXT4=			Format_SEMANTIC_RGBA|Format_TYPE_DXT4,
		Format_RGBA_DXT5=			Format_SEMANTIC_RGBA|Format_TYPE_DXT5,
		Format_YUY2=				Format_SEMANTIC_YUV|Format_TYPE_YUY2,
		Format_NV12=				Format_SEMANTIC_YUV|Format_TYPE_NV12,
	};

	static short getRedBits(int format);
	static short getGreenBits(int format);
	static short getBlueBits(int format);
	static short getAlphaBits(int format);
	static short getPixelSize(int format);

	TextureFormat(int dimension1,int pixelFormat1,int width,int height,int depth,int mipMax1);
	TextureFormat(int dimension1,int pixelFormat1,int xMin1,int yMin1,int zMin1,int xMax1,int yMax1,int zMax1,int mip);

	void setPixelFormat(int pixelFormat1);

	void setOrigin(int x,int y,int z);

	void setSize(int width,int height,int depth);

	void setMips(int mipMin1,int mipMax1);

	void setPitches(int xPitch1,int yPitch1,int zPitch1);

	int getDimension() const;
	int getPixelFormat() const;
	int getXMin() const;
	int getYMin() const;
	int getZMin() const;
	int getXMax() const;
	int getYMax() const;
	int getZMax() const;
	int getWidth() const;
	int getHeight() const;
	int getDepth() const;
	int getXPitch() const;
	int getYPitch() const;
	int getZPitch() const;
	int getMipMin() const;
	int getMipMax() const;
	int getDataSize() const;
	int getSize() const;

	bool isPowerOf2();

	int getMipMaxPossible();

	int getOffset(int x,int y,int z);
};

}
}
