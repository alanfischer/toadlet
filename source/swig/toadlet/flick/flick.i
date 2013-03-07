%module flick

%module(directors="1") flick

%include <arrays_java.i>
%include <../toadlet_egg.i>
%include <enumtypeunsafe.swg>
%javaconst(1);

%include <toadlet/flick/InputData.i>
%include <toadlet/flick/InputDevice.i>
%include <toadlet/flick/BaseInputDeviceListener.i>
%include <toadlet/flick/BaseInputDevice.i>
%{
using namespace toadlet;
using namespace toadlet::flick;
%}
