%module pytoadlet

// Grab a 4 element array as a Python 4-tuple
%typemap(in) float[4](float temp[4]) {   // temp[4] becomes a local variable
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
