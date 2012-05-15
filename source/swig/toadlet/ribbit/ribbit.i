%module ribbit

%module(directors="1") ribbit

%include <toadlet/ribbit/AudioDevice.i>
%{
using namespace toadlet;
using namespace toadlet::ribbit;
%}

typedef float scalar;
