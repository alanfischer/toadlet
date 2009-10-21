/********** Copyright header - do not remove **********
 *
 * The Toadlet Engine
 *
 * Copyright 2009, Lightning Toads Productions, LLC
 *
 * Author(s): Alan Fischer, Andrew Fischer
 *
 * This file is part of The Toadlet Engine.
 *
 * The Toadlet Engine is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * The Toadlet Engine is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.

 * You should have received a copy of the GNU Lesser General Public License
 * along with The Toadlet Engine.  If not, see <http://www.gnu.org/licenses/>.
 *
 ********** Copyright header - do not remove **********/

#ifndef ALEM_H
#define ALEM_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _WIN32
#	ifndef _CRT_SECURE_NO_WARNINGS
#		define _CRT_SECURE_NO_WARNINGS 1
#	endif
#	ifdef ALEM_STATIC
#		define ALEMAPI extern
#	else
#		ifdef ALEM_BUILD
#			define ALEMAPI extern __declspec(dllexport)
#		else
#			define ALEMAPI extern __declspec(dllimport)
#		endif
#	endif
#	define ALEMAPIENTRY __cdecl
#else
#	define ALEMAPI extern
#	define ALEMAPIENTRY
#endif

#define ALEM_ACCELERATED_IF_AVAILABLE 1
#define ALEM_ACCELERATED_NO 0

ALEMAPI int alem_setSoftwareLibraryPath(char *path);

ALEMAPI int alem_init(int accelerated);
ALEMAPI int alem_initWithLibraryNames(char *alLibrary);
ALEMAPI int alem_deinit();
ALEMAPI int alem_getInitialized();
ALEMAPI int alem_getAccelerated();

#define ALEM_DEFINEPROCEDURE(ret,name,sig) \
	typedef ret (ALEMAPIENTRY *alem_proc_##name) sig; \
	ALEMAPI alem_proc_##name name;

/* Common types */

typedef char ALboolean;
typedef signed char ALbyte;
typedef unsigned char ALubyte;
typedef unsigned char ALCchar;
typedef short ALshort;
typedef unsigned short ALushort;
typedef unsigned int ALuint;
typedef int ALint;
typedef float ALfloat;
typedef double ALdouble;
typedef signed int ALsizei;
#define ALvoid void
typedef int ALenum;
typedef unsigned int ALbitfield;
typedef ALfloat ALclampf;
typedef ALdouble ALclampd;

/* ALC */

typedef int ALCenum;

#define ALC_INVALID                              0

/* int Hz */
#define ALC_FREQUENCY                            0x100
#define ALC_REFRESH                              0x101

/* AL_TRUE,AL_FALSE */
#define ALC_SYNC                                 0x102

/* Errors */
#define ALC_NO_ERROR                             0
#define ALC_INVALID_DEVICE                       0x200
#define ALC_INVALID_CONTEXT                      0x201
#define ALC_INVALID_ENUM                         0x202
#define ALC_INVALID_VALUE                        0x203
#define ALC_OUT_OF_MEMORY                        0x204

#define ALC_DEFAULT_DEVICE_SPECIFIER             0x300
#define ALC_DEVICE_SPECIFIER                     0x301
#define ALC_EXTENSIONS                           0x302

#define ALC_MAJOR_VERSION                        0x303
#define ALC_MINOR_VERSION                        0x304

#define ALC_ATTRIBUTES_SIZE                      0x305
#define ALC_ALL_ATTRIBUTES                       0x306

#define ALC_FALSE                                0
#define ALC_TRUE                                 (!(ALC_FALSE))

typedef struct ALCdevice_struct ALCdevice;
typedef struct ALCcontext_struct ALCcontext;

ALEM_DEFINEPROCEDURE(ALCcontext*,alcCreateContext,(ALCdevice* device,ALint* attrlist));
ALEM_DEFINEPROCEDURE(ALCenum,alcMakeContextCurrent,(ALCcontext* context));
ALEM_DEFINEPROCEDURE(ALCcontext*,alcProcessContext,(ALCcontext* context));
ALEM_DEFINEPROCEDURE(void,alcSuspendContext,(ALCcontext* context));
ALEM_DEFINEPROCEDURE(ALCenum,alcDestroyContext,(ALCcontext* context));
ALEM_DEFINEPROCEDURE(ALCcontext*,alcGetCurrentContext,(ALvoid));

ALEM_DEFINEPROCEDURE(ALCenum,alcGetError,(ALCdevice* device));

ALEM_DEFINEPROCEDURE(ALCdevice*,alcOpenDevice,(const ALubyte *tokstr));
ALEM_DEFINEPROCEDURE(void,alcCloseDevice,(ALCdevice* device));

ALEM_DEFINEPROCEDURE(ALboolean,alcIsExtensionPresent,(ALCdevice* device,ALubyte* extName));
ALEM_DEFINEPROCEDURE(ALvoid*,alcGetProcAddress,(ALCdevice* device,ALubyte* funcName));
ALEM_DEFINEPROCEDURE(ALenum,alcGetEnumValue,(ALCdevice* device,ALubyte* enumName));

ALEM_DEFINEPROCEDURE(ALCdevice*,alcGetContextsDevice,(ALCcontext* context));

ALEM_DEFINEPROCEDURE(ALubyte*,alcGetString,(ALCdevice* device,ALenum token));
ALEM_DEFINEPROCEDURE(void,alcGetIntegerv,(ALCdevice* device,ALenum token,ALsizei size,ALint* dest));

/* AL */

#define AL_INVALID                                -1
#define AL_NONE                                   0
#define AL_FALSE                                  0
#define AL_TRUE                                   1
#define AL_SOURCE_TYPE                            0x0200
#define AL_SOURCE_RELATIVE                        0x0202
#define AL_CONE_INNER_ANGLE                       0x1001
#define AL_CONE_OUTER_ANGLE                       0x1002
#define AL_PITCH                                  0x1003
#define AL_POSITION                               0x1004
#define AL_DIRECTION                              0x1005
#define AL_VELOCITY                               0x1006
#define AL_LOOPING                                0x1007
#define AL_STREAMING                              0x1008
#define AL_BUFFER                                 0x1009
#define AL_GAIN                                   0x100A
#define AL_BYTE_LOKI                              0x100C
#define AL_MIN_GAIN                               0x100D
#define AL_MAX_GAIN                               0x100E
#define AL_ORIENTATION                            0x100F
#define AL_SOURCE_STATE                           0x1010
#define AL_INITIAL                                0x1011
#define AL_PLAYING                                0x1012
#define AL_PAUSED                                 0x1013
#define AL_STOPPED                                0x1014

/* Buffer parameters */
#define AL_BUFFERS_QUEUED                         0x1015
#define AL_BUFFERS_PROCESSED                      0x1016
#define AL_PENDING                                0x1017
#define AL_PROCESSED                              0x1018

/* Format specifiers */
#define AL_FORMAT_MONO8                           0x1100
#define AL_FORMAT_MONO16                          0x1101
#define AL_FORMAT_STEREO8                         0x1102
#define AL_FORMAT_STEREO16                        0x1103

#define AL_REFERENCE_DISTANCE                     0x1020
#define AL_ROLLOFF_FACTOR                         0x1021
#define AL_CONE_OUTER_GAIN                        0x1022
#define AL_MAX_DISTANCE                           0x1023
#define AL_FREQUENCY                              0x2001
#define AL_BITS                                   0x2002
#define AL_CHANNELS                               0x2003
#define AL_SIZE                                   0x2004

/* Buffer states */
#define AL_UNUSED                                 0x2010
#define AL_QUEUED                                 0x2011
#define AL_CURRENT                                0x2012

/* Errors */
#define AL_NO_ERROR                               AL_FALSE
#define AL_INVALID_NAME                           0xA001
#define AL_ILLEGAL_ENUM                           0xA002
#define AL_INVALID_VALUE                          0xA003
#define AL_ILLEGAL_COMMAND                        0xA004
#define AL_OUT_OF_MEMORY                          0xA005

/* Context strings */
#define AL_VENDOR                                 0xB001
#define AL_VERSION                                0xB002
#define AL_RENDERER                               0xB003
#define AL_EXTENSIONS                             0xB004

#define AL_DOPPLER_FACTOR                         0xC000
#define AL_DOPPLER_VELOCITY                       0xC001
#define AL_DISTANCE_SCALE                         0xC002

/* Distance models */
#define AL_DISTANCE_MODEL                         0xD000
#define AL_INVERSE_DISTANCE                       0xD001
#define AL_INVERSE_DISTANCE_CLAMPED               0xD002

#define AL_ENV_ROOM_IASIG                         0x3001
#define AL_ENV_ROOM_HIGH_FREQUENCY_IASIG          0x3002
#define AL_ENV_ROOM_ROLLOFF_FACTOR_IASIG          0x3003
#define AL_ENV_DECAY_TIME_IASIG                   0x3004
#define AL_ENV_DECAY_HIGH_FREQUENCY_RATIO_IASIG   0x3005
#define AL_ENV_REFLECTIONS_IASIG                  0x3006
#define AL_ENV_REFLECTIONS_DELAY_IASIG            0x3006
#define AL_ENV_REVERB_IASIG                       0x3007
#define AL_ENV_REVERB_DELAY_IASIG                 0x3008
#define AL_ENV_DIFFUSION_IASIG                    0x3009
#define AL_ENV_DENSITY_IASIG                      0x300A
#define AL_ENV_HIGH_FREQUENCY_REFERENCE_IASIG     0x300B

ALEM_DEFINEPROCEDURE(void,alEnable,(ALenum capability));
ALEM_DEFINEPROCEDURE(void,alDisable,(ALenum capability));
ALEM_DEFINEPROCEDURE(ALboolean,alIsEnabled,(ALenum capability));

ALEM_DEFINEPROCEDURE(void,alHint,(ALenum target,ALenum mode));

ALEM_DEFINEPROCEDURE(void,alGetBooleanv,(ALenum param,ALboolean* data));
ALEM_DEFINEPROCEDURE(void,alGetIntegerv,(ALenum param,ALint* data));
ALEM_DEFINEPROCEDURE(void,alGetFloatv,(ALenum param,ALfloat* data));
ALEM_DEFINEPROCEDURE(void,alGetDoublev,(ALenum param,ALdouble* data));
ALEM_DEFINEPROCEDURE(const ALubyte*,alGetString,(ALenum param));

ALEM_DEFINEPROCEDURE(ALboolean,alGetBoolean,(ALenum param));
ALEM_DEFINEPROCEDURE(ALint,alGetInteger,(ALenum param));
ALEM_DEFINEPROCEDURE(ALfloat,alGetFloat,(ALenum param));
ALEM_DEFINEPROCEDURE(ALdouble,alGetDouble,(ALenum param));

ALEM_DEFINEPROCEDURE(ALenum,alGetError,(ALvoid));
ALEM_DEFINEPROCEDURE(ALboolean,alIsExtensionPresent,(const ALubyte* fname));
ALEM_DEFINEPROCEDURE(void*,alGetProcAddress,(const ALubyte* fname));
ALEM_DEFINEPROCEDURE(ALenum,alGetEnumValue,(const ALubyte* ename));

ALEM_DEFINEPROCEDURE(void,alListenerf,(ALenum pname,ALfloat param));
ALEM_DEFINEPROCEDURE(void,alListeneri,(ALenum pname,ALint param));
ALEM_DEFINEPROCEDURE(void,alListener3f,(ALenum pname,ALfloat f1,ALfloat f2,ALfloat f3));
ALEM_DEFINEPROCEDURE(void,alListenerfv,(ALenum pname,ALfloat* param));

ALEM_DEFINEPROCEDURE(void,alGetListeneri,(ALenum pname,ALint* param));
ALEM_DEFINEPROCEDURE(void,alGetListenerf,(ALenum pname,ALfloat* param));
ALEM_DEFINEPROCEDURE(void,alGetListeneriv,(ALenum pname,ALint* param));
ALEM_DEFINEPROCEDURE(void,alGetListenerfv,(ALenum pname,ALfloat* param));
ALEM_DEFINEPROCEDURE(void,alGetListener3f,(ALenum pname,ALfloat *f1,ALfloat *f2,ALfloat *f3));

ALEM_DEFINEPROCEDURE(void,alGenSources,(ALsizei n,ALuint* sources));
ALEM_DEFINEPROCEDURE(void,alDeleteSources,(ALsizei n,ALuint* sources));
ALEM_DEFINEPROCEDURE(ALboolean,alIsSource,(ALuint sid));

ALEM_DEFINEPROCEDURE(void,alSourcef,(ALuint sid,ALenum pname,ALfloat param));
ALEM_DEFINEPROCEDURE(void,alSourcei,(ALuint sid,ALenum pname,ALint param));
ALEM_DEFINEPROCEDURE(void,alSource3f,(ALuint sid,ALenum pname,ALfloat f1,ALfloat f2,ALfloat f3));
ALEM_DEFINEPROCEDURE(void,alSourcefv,(ALuint sid,ALenum pname,ALfloat* param));

ALEM_DEFINEPROCEDURE(void,alGetSourcei,(ALuint sid,ALenum pname,ALint* param));
ALEM_DEFINEPROCEDURE(void,alGetSourcef,(ALuint sid,ALenum pname,ALfloat* param));
ALEM_DEFINEPROCEDURE(void,alGetSourceiv,(ALuint sid,ALenum pname,ALint* param));
ALEM_DEFINEPROCEDURE(void,alGetSourcefv,(ALuint sid,ALenum pname,ALfloat* param));
ALEM_DEFINEPROCEDURE(void,alGetSource3f,(ALuint sid,ALenum pname,ALfloat *f1,ALfloat *f2,ALfloat *f3));

ALEM_DEFINEPROCEDURE(void,alSourcePlayv,(ALsizei ns,ALuint* ids));
ALEM_DEFINEPROCEDURE(void,alSourceStopv,(ALsizei ns,ALuint* ids));
ALEM_DEFINEPROCEDURE(void,alSourceRewindv,(ALsizei ns,ALuint* ids));
ALEM_DEFINEPROCEDURE(void,alSourcePausev,(ALsizei ns,ALuint* ids));

ALEM_DEFINEPROCEDURE(void,alSourcePlay,(ALuint sid));
ALEM_DEFINEPROCEDURE(void,alSourcePause,(ALuint sid));
ALEM_DEFINEPROCEDURE(void,alSourceRewind,(ALuint sid));
ALEM_DEFINEPROCEDURE(void,alSourceStop,(ALuint sid));

ALEM_DEFINEPROCEDURE(void,alGenBuffers,(ALsizei n,ALuint* buffers));
ALEM_DEFINEPROCEDURE(void,alDeleteBuffers,(ALsizei n,ALuint* buffers));
ALEM_DEFINEPROCEDURE(ALboolean,alIsBuffer,(ALuint buffer));
ALEM_DEFINEPROCEDURE(void,alBufferData,(ALuint buffer,ALenum format,ALvoid *data,ALsizei size,ALsizei freq));

ALEM_DEFINEPROCEDURE(void,alGetBufferi,(ALuint buffer,ALenum param,ALint* value));
ALEM_DEFINEPROCEDURE(void,alGetBufferf,(ALuint buffer,ALenum param,ALfloat* value));
ALEM_DEFINEPROCEDURE(void,alGetBufferiv,(ALuint buffer,ALenum param,ALint* value));
ALEM_DEFINEPROCEDURE(void,alGetBufferfv,(ALuint buffer,ALenum param,ALfloat* value));

ALEM_DEFINEPROCEDURE(void,alSourceQueueBuffers,(ALuint sid,ALsizei numEntries,ALuint* bids));
ALEM_DEFINEPROCEDURE(void,alSourceUnqueueBuffers,(ALuint sid,ALsizei numEntries,ALuint* bids));
ALEM_DEFINEPROCEDURE(void,alQueuei,(ALuint sid,ALenum param,ALint value));

ALEM_DEFINEPROCEDURE(void,alDopplerFactor,(ALfloat value));
ALEM_DEFINEPROCEDURE(void,alDopplerVelocity,(ALfloat value));
ALEM_DEFINEPROCEDURE(void,alDistanceModel,(ALenum distanceModel));

#ifdef __cplusplus
}
#endif

#endif
