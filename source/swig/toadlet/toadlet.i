%module pytoadlet

%module(directors="1") pytoadlet

%include <toadlet/peeper/RenderTarget.i>
%include <toadlet/peeper/RenderDevice.i>
%include <toadlet/ribbit/AudioDevice.i>
%{
using namespace toadlet;
using namespace toadlet::peeper;
using namespace toadlet::ribbit;
%}

typedef float scalar;

%typemap(in) String{
	$1=PyString_AsString($input);
}

%typemap(out) String{
	$result=PyString_FromString($1);
}

%typemap(in) float[4](float temp[4]){
	if(PyTuple_Check($input)){
		if(!PyArg_ParseTuple($input,"ffff",temp,temp+1,temp+2,temp+3)){
			PyErr_SetString(PyExc_TypeError,"tuple must have 4 elements");
			return NULL;
		}
		$1=&temp[0];
	}
	else{
		PyErr_SetString(PyExc_TypeError,"expected a tuple.");
		return NULL;
	}
}

%typemap(in) float[3](float temp[3]){
	if(PyTuple_Check($input)){
		if(!PyArg_ParseTuple($input,"fff",temp,temp+1,temp+2)){
			PyErr_SetString(PyExc_TypeError,"tuple must have 3 elements");
			return NULL;
		}
		$1=&temp[0];
		}
		else{
		PyErr_SetString(PyExc_TypeError,"expected a tuple.");
		return NULL;
	}
}

%include <toadlet/tadpole/Engine.i>
%include <toadlet/tadpole/Scene.i>
%include <toadlet/tadpole/Component.i>
%include <toadlet/tadpole/ActionComponent.i>
%include <toadlet/tadpole/AnimationActionComponent.i>
%include <toadlet/tadpole/AudioComponent.i>
%include <toadlet/tadpole/Node.i>
%include <toadlet/tadpole/CameraNode.i>
%include <toadlet/tadpole/MeshNode.i>
%include <toadlet/tadpole/LightNode.i>
%include <toadlet/tadpole/animation/Animation.i>
%include <toadlet/tadpole/animation/BaseAnimation.i>
%include <toadlet/tadpole/animation/MeshAnimation.i>
%{
using namespace toadlet::tadpole;
using namespace toadlet::tadpole::animation;
using namespace toadlet::tadpole::node;
%}
