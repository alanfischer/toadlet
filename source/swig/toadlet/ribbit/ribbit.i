%module ribbit

%module(directors="1") ribbit

%include <arrays_java.i>

%include <toadlet/ribbit/AudioFormat.i>
%include <toadlet/ribbit/AudioStream.i>
%include <toadlet/ribbit/AudioBuffer.i>
%include <toadlet/ribbit/Audio.i>
%include <toadlet/ribbit/AudioDevice.i>
%include <toadlet/ribbit/BaseAudioBuffer.i>
%include <toadlet/ribbit/BaseAudio.i>
%include <toadlet/ribbit/BaseAudioDevice.i>
%{
using namespace toadlet;
using namespace toadlet::ribbit;
%}

typedef float scalar;
typedef long long uint64;
typedef signed char tbyte;
