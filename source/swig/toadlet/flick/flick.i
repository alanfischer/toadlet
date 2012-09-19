%module flick

%module(directors="1") flick

%include <arrays_java.i>

%include <toadlet/flick/InputData.i>
%include <toadlet/flick/InputDevice.i>
%include <toadlet/flick/BaseInputDeviceListener.i>
%include <toadlet/flick/BaseInputDevice.i>
%{
using namespace toadlet;
using namespace toadlet::flick;
%}

typedef float scalar;
typedef long long uint64;
