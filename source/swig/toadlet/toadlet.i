%module pytoadlet

%typemap(in) float[4](float temp[4]){
  if (PyTuple_Check($input)) {
    if (!PyArg_ParseTuple($input,"ffff",temp,temp+1,temp+2,temp+3)) {
      PyErr_SetString(PyExc_TypeError,"tuple must have 4 elements");
      return NULL;
    }
    $1 = &temp[0];
  } else {
    PyErr_SetString(PyExc_TypeError,"expected a tuple.");
    return NULL;
  }
}

%typemap(in) float[3](float temp[3]){
  if (PyTuple_Check($input)) {
    if (!PyArg_ParseTuple($input,"fff",temp,temp+1,temp+2)) {
      PyErr_SetString(PyExc_TypeError,"tuple must have 3 elements");
      return NULL;
    }
    $1 = &temp[0];
  } else {
    PyErr_SetString(PyExc_TypeError,"expected a tuple.");
    return NULL;
  }
}

%include <toadlet/peeper/RenderTarget.i>
%include <toadlet/peeper/RenderDevice.i>
%{
using namespace toadlet::peeper;
%}

%include <toadlet/tadpole/Engine.i>
%include <toadlet/tadpole/Scene.i>
%include <toadlet/tadpole/Node.i>
%include <toadlet/tadpole/CameraNode.i>
%include <toadlet/tadpole/MeshNode.i>
%{
using namespace toadlet::tadpole;
using namespace toadlet::tadpole::node;
%}
