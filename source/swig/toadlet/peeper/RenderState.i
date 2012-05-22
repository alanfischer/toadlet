%{
#	include <toadlet/peeper/RenderState.h>
%}

namespace toadlet{
namespace peeper{

%refobject RenderState "$this->retain();"
%unrefobject RenderState "$this->release();"

SWIG_JAVABODY_PROXY(public, public, RenderState)

class RenderState{
public:
	virtual ~RenderState();

	virtual void destroy()=0;

	%extend {
		void setAlpha(float alpha){
			toadlet::peeper::MaterialState materialState;
			$self->getMaterialState(materialState);
			materialState.ambient.w=alpha;
			materialState.diffuse.w=alpha;
			materialState.specular.w=alpha;
			materialState.emissive.w=alpha;
			$self->setMaterialState(materialState);
	
			if(alpha<1.0f){
				$self->setBlendState(toadlet::peeper::BlendState(toadlet::peeper::BlendState::Combination_ALPHA));
			}
			else{
				$self->setBlendState(toadlet::peeper::BlendState());
			}
		}
		
		float getAlpha(){
			toadlet::peeper::MaterialState materialState;
			$self->getMaterialState(materialState);
			return materialState.ambient.w;
		}
	}
};

}
}
