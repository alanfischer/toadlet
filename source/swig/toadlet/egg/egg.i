%module egg

%module(directors="1") egg

%include <arrays_java.i>
%include <../toadlet_egg.i>
%include <enumtypeunsafe.swg>
%javaconst(1);

%include <toadlet/egg/Error.i>
%include <toadlet/egg/Log.i>
%include <toadlet/egg/Logger.i>
%{
using namespace toadlet;
using namespace toadlet::egg;
%}
