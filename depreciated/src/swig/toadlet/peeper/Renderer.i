%{
#	include <toadlet/peeper/Renderer.h>
%}

namespace toadlet{
namespace peeper{

class Renderer{
public:
	typedef unsigned int Options;

	enum Status{
		STATUS_OK=0,
		STATUS_UNAVAILABLE,
		STATUS_NEEDSRESET,
	};

	enum RenderTargetBuffer{
		RTB_COLOR=1,
		RTB_DEPTH=2,
		RTB_STENCIL=4,
	};

	enum FaceCulling{
		FC_NONE=0,
		FC_CCW=1,
		FC_CW=2,
		FC_FRONT=1,
		FC_BACK=2,
	};

	enum DepthTest{
		DT_NONE,
		DT_NEVER,
		DT_LESS,
		DT_EQUAL,
		DT_LEQUAL,
		DT_GREATER,
		DT_NOTEQUAL,
		DT_GEQUAL,
		DT_ALWAYS,
	};

	enum AlphaTest{
		AT_NONE,
		AT_LESS,
		AT_EQUAL,
		AT_LEQUAL,
		AT_GREATER,
		AT_NOTEQUAL,
		AT_GEQUAL,
		AT_ALWAYS,
	};

	enum TextureCoordinateGeneration{
		TCG_DISABLED=0,
		TCG_OBJECTSPACE,
		TCG_CAMERASPACE,
	};

	enum FogType{
		FT_NONE=0,
		FT_LINEAR,
	};

	enum FillType{
		FT_POINT=0,
		FT_LINE,
		FT_SOLID,
	};

	enum Shading{
		SHADING_FLAT=0,
		SHADING_SMOOTH,
	};

	enum NormalizeType{
		NT_NONE=0,
		NT_RESCALE,
		NT_NORMALIZE,
	};

	virtual bool startup(RenderContext *renderContext,Options *options)=0;
	virtual bool shutdown()=0;
	virtual Status getStatus()=0;
	virtual bool reset()=0;

	virtual void swapRenderTargetBuffers()=0;
	virtual void beginScene()=0;
	virtual void endScene()=0;
};

}
}
