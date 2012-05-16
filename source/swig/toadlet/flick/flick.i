%module flick

%module(directors="1") flick

%include <toadlet/flick/InputData.i>
%include <toadlet/flick/InputDevice.i>
%include <toadlet/flick/InputDeviceListener.i>
%include <toadlet/flick/BaseInputDevice.i>
%{
using namespace toadlet;
using namespace toadlet::flick;
%}

typedef float scalar;
