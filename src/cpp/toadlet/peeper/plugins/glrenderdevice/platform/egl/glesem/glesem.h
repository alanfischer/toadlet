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

#ifndef GLESEM_H
#define GLESEM_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _WIN32
#	ifndef _CRT_SECURE_NO_WARNINGS
#		define _CRT_SECURE_NO_WARNINGS 1
#	endif
#	ifdef GLESEM_STATIC
#		define GLESEMAPI extern
#	else
#		ifdef GLESEM_BUILD
#			define GLESEMAPI extern __declspec(dllexport)
#		else
#			define GLESEMAPI extern __declspec(dllimport)
#		endif
#	endif
#	define GLESEMAPIENTRY __stdcall
#else
#	define GLESEMAPI extern
#	define GLESEMAPIENTRY
#endif

#define GLESEM_EGL_BUILD_VERSION 120
#define GLESEM_GL_BUILD_VERSION 110

#define GLESEM_ACCELERATED_IF_AVAILABLE 1
#define GLESEM_ACCELERATED_NO 0

GLESEMAPI int glesem_setSoftwareLibraryPath(char *path);

GLESEMAPI int glesem_init(int accelerated);
GLESEMAPI int glesem_initWithLibraryNames(char *eglLibrary,char *glLibrary);
GLESEMAPI int glesem_deinit();
GLESEMAPI int glesem_getInitialized();
GLESEMAPI int glesem_getAccelerated();

GLESEMAPI int glesem_egl_majorVersion;
GLESEMAPI int glesem_egl_minorVersion;
GLESEMAPI int glesem_egl_version_1_1;
GLESEMAPI int glesem_egl_version_1_2;

GLESEMAPI int glesem_gl_majorVersion;
GLESEMAPI int glesem_gl_minorVersion;
GLESEMAPI int glesem_gl_version_1_1;

#define GLESEM_DEFINEPROCEDURE(ret,name,sig) \
	typedef ret (GLESEMAPIENTRY *glesem_proc_##name) sig; \
	GLESEMAPI glesem_proc_##name name;

/* EGL */

typedef int EGLint;
#if (GLESEM_EGL_BUILD_VERSION >= 120)	
	typedef int EGLenum;
#endif
typedef unsigned int EGLBoolean;
typedef void* EGLConfig;
typedef void* EGLContext;
typedef void* EGLDisplay;
typedef void* EGLSurface;
#if (GLESEM_EGL_BUILD_VERSION >= 120)
	typedef void* EGLClientBuffer;
#endif

typedef EGLint NativeDisplayType;
typedef void* NativeWindowType;
typedef void* NativePixmapType;

#define EGL_DEFAULT_DISPLAY ((NativeDisplayType)0)
#define EGL_NO_CONTEXT ((EGLContext)0)
#define EGL_NO_DISPLAY ((EGLDisplay)0)
#define EGL_NO_SURFACE ((EGLSurface)0)

/* Versioning and extensions */
#define EGL_VERSION_1_0 1
#if (GLESEM_EGL_BUILD_VERSION >= 110)
#	define EGL_VERSION_1_1 1
#endif
#if (GLESEM_EGL_BUILD_VERSION >= 120)
#	define EGL_VERSION_1_2 1
#endif

/* Boolean */
#define EGL_FALSE							0
#define EGL_TRUE							1

/* Errors */
#define EGL_SUCCESS							0x3000
#define EGL_NOT_INITIALIZED					0x3001
#define EGL_BAD_ACCESS						0x3002
#define EGL_BAD_ALLOC						0x3003
#define EGL_BAD_ATTRIBUTE					0x3004
#define EGL_BAD_CONFIG						0x3005
#define EGL_BAD_CONTEXT						0x3006
#define EGL_BAD_CURRENT_SURFACE				0x3007
#define EGL_BAD_DISPLAY						0x3008
#define EGL_BAD_MATCH						0x3009
#define EGL_BAD_NATIVE_PIXMAP				0x300A
#define EGL_BAD_NATIVE_WINDOW				0x300B
#define EGL_BAD_PARAMETER					0x300C
#define EGL_BAD_SURFACE						0x300D
#define EGL_CONTEXT_LOST					0x300E

/* Config attributes */
#define EGL_BUFFER_SIZE						0x3020
#define EGL_ALPHA_SIZE						0x3021
#define EGL_BLUE_SIZE						0x3022
#define EGL_GREEN_SIZE						0x3023
#define EGL_RED_SIZE						0x3024
#define EGL_DEPTH_SIZE						0x3025
#define EGL_STENCIL_SIZE					0x3026
#define EGL_CONFIG_CAVEAT					0x3027
#define EGL_CONFIG_ID						0x3028
#define EGL_LEVEL							0x3029
#define EGL_MAX_PBUFFER_HEIGHT				0x302A
#define EGL_MAX_PBUFFER_PIXELS				0x302B
#define EGL_MAX_PBUFFER_WIDTH				0x302C
#define EGL_NATIVE_RENDERABLE				0x302D
#define EGL_NATIVE_VISUAL_ID				0x302E
#define EGL_NATIVE_VISUAL_TYPE				0x302F
#define EGL_PRESERVED_RESOURCES				0x3030
#define EGL_SAMPLES							0x3031
#define EGL_SAMPLE_BUFFERS					0x3032
#define EGL_SURFACE_TYPE					0x3033
#define EGL_TRANSPARENT_TYPE				0x3034
#define EGL_TRANSPARENT_BLUE_VALUE			0x3035
#define EGL_TRANSPARENT_GREEN_VALUE			0x3036
#define EGL_TRANSPARENT_RED_VALUE			0x3037
#if (GLESEM_EGL_BUILD_VERSION >= 110)
#	define EGL_BIND_TO_TEXTURE_RGB			0x3039
#	define EGL_BIND_TO_TEXTURE_RGBA			0x303A
#	define EGL_MIN_SWAP_INTERVAL			0x303B
#	define EGL_MAX_SWAP_INTERVAL			0x303C
#endif/* GLESEM_EGL_BUILD_VERSION >= 110 */
#if (GLESEM_EGL_BUILD_VERSION >= 120)
#	define EGL_LUMINANCE_SIZE				0x303D
#	define EGL_ALPHA_MASK_SIZE				0x303E
#	define EGL_COLOR_BUFFER_TYPE			0x303F
#	define EGL_RENDERABLE_TYPE				0x3040
#	define EGL_MATCH_NATIVE_PIXMAP			0x3041

/*! Unknown display resolution/aspect ratio */
#	define EGL_UNKNOWN			((EGLint)-1)

#	define EGL_RENDER_BUFFER				0x3086
#	define EGL_COLORSPACE					0x3087
#	define EGL_ALPHA_FORMAT					0x3088
#	define EGL_COLORSPACE_sRGB				0x3089
#	define EGL_COLORSPACE_LINEAR			0x308A
#	define EGL_ALPHA_Format_NONPRE			0x308B
#	define EGL_ALPHA_Format_PRE				0x308C
#	define EGL_CLIENT_APIS					0x308D
#	define EGL_RGB_BUFFER					0x308E
#	define EGL_LUMINANCE_BUFFER				0x308F
#	define EGL_HORIZONTAL_RESOLUTION		0x3090
#	define EGL_VERTICAL_RESOLUTION			0x3091
#	define EGL_PIXEL_ASPECT_RATIO			0x3092
#	define EGL_SWAP_BEHAVIOR				0x3093
#	define EGL_BUFFER_PRESERVED				0x3094
#	define EGL_BUFFER_DESTROYED				0x3095
/* CreatePbufferFromClientBuffer buffer types */
#	define EGL_OPENVG_IMAGE					0x3096
/* QueryContext targets */
#	define EGL_CONTEXT_CLIENT_TYPE			0x3097
#	define EGL_CONTEXT_CLIENT_VERSION		0x3098

#	define EGL_OPENGL_ES_API				0x30A0
#	define EGL_OPENVG_API					0x30A1

#endif /* GLESEM_EGL_BUILD_VERSION >= 120 */

/* Config attribute and value */
#define EGL_NONE							0x3038
/* 0x303D - 0x304F reserved for additional config attributes. */

/* Config values */
#define EGL_DONT_CARE						((EGLint) -1)
#define EGL_PBUFFER_BIT						0x01
#define EGL_PIXMAP_BIT						0x02
#define EGL_WINDOW_BIT						0x04
#define EGL_SLOW_CONFIG						0x3050
#define EGL_NON_CONFORMANT_CONFIG			0x3051
#define EGL_TRANSPARENT_RGB					0x3052

#if (GLESEM_EGL_BUILD_VERSION >= 110)
#	define EGL_NO_TEXTURE					0x305C
#	define EGL_TEXTURE_RGB					0x305D
#	define EGL_TEXTURE_RGBA					0x305E
#	define EGL_TEXTURE_2D					0x305F
#endif /* GLESEM_EGL_BUILD_VERSION >= 110 */

#if (GLESEM_EGL_BUILD_VERSION >= 120)
#	define EGL_OPENGL_ES_BIT				0x01
#	define EGL_OPENVG_BIT					0x02
#	define EGL_OPENGL_ES2_BIT				0x04
#	define EGL_DISPLAY_SCALING				10000
#endif /* GLESEM_EGL_BUILD_VERSION >= 120 */

/* String names */
#define EGL_VENDOR							0x3053
#define EGL_VERSION							0x3054
#define EGL_EXTENSIONS						0x3055

/* Surface attributes */
#define EGL_HEIGHT							0x3056
#define EGL_WIDTH							0x3057
#define EGL_LARGEST_PBUFFER					0x3058
#if (GLESEM_EGL_BUILD_VERSION >= 110)
#	define EGL_TEXTURE_FORMAT				0x3080
#	define EGL_TEXTURE_TARGET				0x3081
#	define EGL_MIPMAP_TEXTURE				0x3082
#	define EGL_MIPMAP_LEVEL					0x3083

/* BindTexImage/ReleaseTexImage buffer target */
#	define EGL_BACK_BUFFER					0x3084
#	define EGL_SINGLE_BUFFER				0x3085
#endif /* GLESEM_EGL_BUILD_VERSION >= 110 */

/* Current surfaces */
#define EGL_DRAW							0x3059
#define EGL_READ							0x305A

/* Engines */
#define EGL_CORE_NATIVE_ENGINE				0x305B

GLESEM_DEFINEPROCEDURE(EGLint,eglGetError,(void));

GLESEM_DEFINEPROCEDURE(EGLDisplay,eglGetDisplay,(NativeDisplayType displayID));
GLESEM_DEFINEPROCEDURE(EGLBoolean,eglInitialize,(EGLDisplay dpy, EGLint* major, EGLint* minor));
GLESEM_DEFINEPROCEDURE(EGLBoolean,eglTerminate,(EGLDisplay dpy));

GLESEM_DEFINEPROCEDURE(const char*,eglQueryString,(EGLDisplay dpy, EGLint name));

GLESEM_DEFINEPROCEDURE(EGLBoolean,eglGetConfigs,(EGLDisplay dpy, EGLConfig* configs, EGLint config_size, EGLint* num_config));
GLESEM_DEFINEPROCEDURE(EGLBoolean,eglChooseConfig,(EGLDisplay dpy, const EGLint* attrib_list, EGLConfig* configs, EGLint config_size, EGLint* num_config));
GLESEM_DEFINEPROCEDURE(EGLBoolean,eglGetConfigAttrib,(EGLDisplay dpy, EGLConfig config, EGLint attribute, EGLint* value));

GLESEM_DEFINEPROCEDURE(EGLSurface,eglCreateWindowSurface,(EGLDisplay dpy, EGLConfig config, NativeWindowType win, const EGLint* attrib_list));
GLESEM_DEFINEPROCEDURE(EGLSurface,eglCreatePbufferSurface,(EGLDisplay dpy, EGLConfig config, const EGLint* attrib_list));
GLESEM_DEFINEPROCEDURE(EGLSurface,eglCreatePixmapSurface,(EGLDisplay dpy, EGLConfig config, NativePixmapType pixmap, const EGLint* attrib_list));
GLESEM_DEFINEPROCEDURE(EGLBoolean,eglDestroySurface,(EGLDisplay dpy, EGLSurface surface));
GLESEM_DEFINEPROCEDURE(EGLBoolean,eglQuerySurface,(EGLDisplay dpy, EGLSurface surface, EGLint attribute, EGLint* value));

#if (GLESEM_EGL_BUILD_VERSION >= 110)
	/* EGL 1.1 render-to-texture APIs */
	GLESEM_DEFINEPROCEDURE(EGLBoolean,eglSurfaceAttrib,(EGLDisplay dpy, EGLSurface surface, EGLint attribute, EGLint value));
	GLESEM_DEFINEPROCEDURE(EGLBoolean,eglBindTexImage,(EGLDisplay dpy, EGLSurface surface, EGLint buffer));
	GLESEM_DEFINEPROCEDURE(EGLBoolean,eglReleaseTexImage,(EGLDisplay dpy, EGLSurface surface, EGLint buffer));

	/* EGL 1.1 swap control API */
	GLESEM_DEFINEPROCEDURE(EGLBoolean,eglSwapInterval,(EGLDisplay dpy, EGLint interval));
#endif /* GLESEM_EGL_BUILD_VERSION >= 110 */

GLESEM_DEFINEPROCEDURE(EGLContext,eglCreateContext,(EGLDisplay dpy, EGLConfig config, EGLContext share_list, const EGLint* attrib_list));
GLESEM_DEFINEPROCEDURE(EGLBoolean,eglDestroyContext,(EGLDisplay dpy, EGLContext ctx));
GLESEM_DEFINEPROCEDURE(EGLBoolean,eglMakeCurrent,(EGLDisplay dpy, EGLSurface draw, EGLSurface read, EGLContext ctx));

GLESEM_DEFINEPROCEDURE(EGLContext,eglGetCurrentContext,(void));
GLESEM_DEFINEPROCEDURE(EGLSurface,eglGetCurrentSurface,(EGLint readdraw));
GLESEM_DEFINEPROCEDURE(EGLDisplay,eglGetCurrentDisplay,(void));
GLESEM_DEFINEPROCEDURE(EGLBoolean,eglQueryContext,(EGLDisplay dpy, EGLContext ctx, EGLint attribute, EGLint* value));

GLESEM_DEFINEPROCEDURE(EGLBoolean,eglWaitGL,(void));
GLESEM_DEFINEPROCEDURE(EGLBoolean,eglWaitNative,(EGLint engine));
GLESEM_DEFINEPROCEDURE(EGLBoolean,eglSwapBuffers,(EGLDisplay dpy, EGLSurface surface));
GLESEM_DEFINEPROCEDURE(EGLBoolean,eglCopyBuffers,(EGLDisplay dpy, EGLSurface surface, NativePixmapType target));

GLESEM_DEFINEPROCEDURE(void*,eglGetProcAddress,(const char *procname));

#if (GLESEM_EGL_BUILD_VERSION >= 120)
	GLESEM_DEFINEPROCEDURE(EGLSurface,eglCreatePbufferFromClientBuffer,(EGLDisplay dpy, EGLenum buftype, EGLClientBuffer buffer, EGLConfig config, const EGLint *attrib_list));
	GLESEM_DEFINEPROCEDURE(EGLBoolean,eglWaitClient,(void));
	GLESEM_DEFINEPROCEDURE(EGLBoolean,eglBindAPI,(EGLenum api));
	GLESEM_DEFINEPROCEDURE(EGLenum,eglQueryAPI,(void));
	GLESEM_DEFINEPROCEDURE(EGLBoolean,eglReleaseThread,(void));
#endif /* GLESEM_EGL_BUILD_VERSION >= 120 */

GLESEMAPI int GLESEMAPIENTRY glesem_eglInitialize(EGLDisplay display);

/* GL */

typedef void GLvoid;
typedef unsigned int GLenum;
typedef unsigned char GLboolean;
typedef unsigned int GLbitfield;
typedef signed char GLbyte;
typedef short GLshort;
typedef int GLint;
typedef int GLsizei;
typedef unsigned char GLubyte;
typedef unsigned short GLushort;
typedef unsigned int GLuint;
typedef float GLfloat;
typedef float GLclampf;
typedef int GLclampx;
typedef int GLfixed;

#if (GLESEM_GL_BUILD_VERSION >= 110)
	typedef long int GLintptr;
	typedef long int GLsizeiptr;
#endif /* GLESEM_GL_BUILD_VERSION >= 110 */

/* extensions */
#define GL_OES_VERSION_1_0							1
#define GL_OES_byte_coordinates						1
#define GL_OES_fixed_point							1
#define GL_OES_single_precision						1
#define GL_OES_read_format							1
#define GL_OES_query_matrix							1
#define GL_OES_compressed_paletted_texture			1

#if (GLESEM_GL_BUILD_VERSION >= 110)
#	define GL_OES_VERSION_1_1						1
#	define GL_OES_matrix_palette					1
#	define GL_OES_point_sprite						1
#	define GL_OES_point_size_array					1
#	define GL_OES_draw_texture						1
#	define GL_OES_matrix_get						1
#endif /* GLESEM_GL_BUILD_VERSION >= 110 */

/* boolean values */
#define GL_FALSE									0
#define GL_TRUE										1

/* common values */
#define GL_ZERO										0
#define GL_ONE										1

/* comparison functions */
#define GL_NEVER									0x0200
#define GL_LESS										0x0201
#define GL_EQUAL									0x0202
#define GL_LEQUAL									0x0203
#define GL_GREATER									0x0204
#define GL_NOTEQUAL									0x0205
#define GL_GEQUAL									0x0206
#define GL_ALWAYS									0x0207

/* data types */
#define GL_BYTE										0x1400
#define GL_UNSIGNED_BYTE							0x1401
#define GL_SHORT									0x1402
#define GL_UNSIGNED_SHORT							0x1403
#define GL_FLOAT									0x1406
#define GL_FIXED									0x140C

/* primitives */
#define GL_POINTS									0x0000
#define GL_LINES									0x0001
#define GL_LINE_LOOP								0x0002
#define GL_LINE_STRIP								0x0003
#define GL_TRIANGLES								0x0004
#define GL_TRIANGLE_STRIP							0x0005
#define GL_TRIANGLE_FAN								0x0006

/* vertex arrays */
#define GL_VERTEX_ARRAY								0x8074
#define GL_NORMAL_ARRAY								0x8075
#define GL_COLOR_ARRAY								0x8076
#define GL_TEXTURE_COORD_ARRAY						0x8078
#if (GLESEM_GL_BUILD_VERSION >= 110)
#	define GL_MATRIX_INDEX_ARRAY_OES				0x8844
#	define GL_WEIGHT_ARRAY_OES						0x86AD
#	define GL_POINT_SIZE_ARRAY_OES					0x8B9C
#endif /* GLESEM_GL_BUILD_VERSION >= 110 */

#if (GLESEM_GL_BUILD_VERSION >= 110)
	/* vertex buffer objects */
#	define GL_ARRAY_BUFFER							0x8892
#	define GL_ELEMENT_ARRAY_BUFFER					0x8893
#	define GL_STREAM_DRAW							0x88E0
#	define GL_STATIC_DRAW							0x88E4
#	define GL_DYNAMIC_DRAW							0x88E8
#	define GL_BUFFER_SIZE							0x8764
#	define GL_BUFFER_USAGE							0x8765
#	define GL_BUFFER_ACCESS							0x88BB
#	define GL_WRITE_ONLY							0x88B9
#endif /* GLESEM_GL_BUILD_VERSION >= 110 */

/* matrix mode */
#define GL_MODELVIEW								0x1700
#define GL_PROJECTION								0x1701
#define GL_TEXTURE									0x1702

#if (GLESEM_GL_BUILD_VERSION >= 110)
#	define GL_MATRIX_PALETTE_OES					0x8840
#endif /* GLESEM_GL_BUILD_VERSION >= 110 */

/* smoothing */
#define GL_POINT_SMOOTH								0x0B10
#define GL_LINE_SMOOTH								0x0B20

/* geometry */
#define GL_CW										0x0900
#define GL_CCW										0x0901
#define GL_FRONT									0x0404
#define GL_BACK										0x0405
#define GL_CULL_FACE								0x0B44
#define GL_POLYGON_OFFSET_FILL						0x8037

/* lighting */
#define GL_LIGHTING									0x0B50
#define GL_LIGHT0									0x4000
#define GL_LIGHT1									0x4001
#define GL_LIGHT2									0x4002
#define GL_LIGHT3									0x4003
#define GL_LIGHT4									0x4004
#define GL_LIGHT5									0x4005
#define GL_LIGHT6									0x4006
#define GL_LIGHT7									0x4007
#define GL_SPOT_EXPONENT							0x1205
#define GL_SPOT_CUTOFF								0x1206
#define GL_CONSTANT_ATTENUATION						0x1207
#define GL_LINEAR_ATTENUATION						0x1208
#define GL_QUADRATIC_ATTENUATION					0x1209
#define GL_AMBIENT									0x1200
#define GL_DIFFUSE									0x1201
#define GL_SPECULAR									0x1202
#define GL_EMISSION									0x1600
#define GL_SHININESS								0x1601
#define GL_POSITION									0x1203
#define GL_SPOT_DIRECTION							0x1204
#define GL_AMBIENT_AND_DIFFUSE						0x1602
#define GL_LIGHT_MODEL_TWO_SIDE						0x0B52
#define GL_LIGHT_MODEL_AMBIENT						0x0B53
#define GL_FRONT_AND_BACK							0x0408
#define GL_FLAT										0x1D00
#define GL_SMOOTH									0x1D01
#define GL_COLOR_MATERIAL							0x0B57
#define GL_NORMALIZE								0x0BA1

/* blending */
#define GL_ADD										0x0104
#define GL_BLEND									0x0BE2
#define GL_SRC_COLOR								0x0300
#define GL_ONE_MINUS_SRC_COLOR						0x0301
#define GL_SRC_ALPHA								0x0302
#define GL_ONE_MINUS_SRC_ALPHA						0x0303
#define GL_DST_ALPHA								0x0304
#define GL_ONE_MINUS_DST_ALPHA						0x0305
#define GL_DST_COLOR								0x0306
#define GL_ONE_MINUS_DST_COLOR						0x0307
#define GL_SRC_ALPHA_SATURATE						0x0308

/* fog */
#define GL_FOG										0x0B60
#define GL_FOG_DENSITY								0x0B62
#define GL_FOG_START								0x0B63
#define GL_FOG_END									0x0B64
#define GL_FOG_MODE									0x0B65
#define GL_FOG_COLOR								0x0B66
#define GL_EXP										0x0800
#define GL_EXP2										0x0801

/* logic ops */
#define GL_CLEAR									0x1500
#define GL_AND										0x1501
#define GL_AND_REVERSE								0x1502
#define GL_COPY										0x1503
#define GL_AND_INVERTED								0x1504
#define GL_NOOP										0x1505
#define GL_XOR										0x1506
#define GL_OR										0x1507
#define GL_NOR										0x1508
#define GL_EQUIV									0x1509
#define GL_INVERT									0x150A
#define GL_OR_REVERSE								0x150B
#define GL_COPY_INVERTED							0x150C
#define GL_OR_INVERTED								0x150D
#define GL_NAND										0x150E
#define GL_SET										0x150F

/* capabilities */
#define GL_DEPTH_TEST								0x0B71
#define GL_STENCIL_TEST								0x0B90
#define GL_ALPHA_TEST								0x0BC0
#define GL_DITHER									0x0BD0
#define GL_COLOR_LOGIC_OP							0x0BF2
#define GL_SCISSOR_TEST								0x0C11
#define GL_RESCALE_NORMAL							0x803A

#if (GLESEM_GL_BUILD_VERSION >= 110)
#	define GL_POINT_SPRITE_OES						0x8861
#endif /* GLESEM_GL_BUILD_VERSION >= 110 */

/* stencil ops */
#define GL_KEEP										0x1E00
#define GL_REPLACE									0x1E01
#define GL_INCR										0x1E02
#define GL_DECR										0x1E03

/* buffers, pixel reading */
#define GL_ALPHA									0x1906
#define GL_RGB										0x1907
#define GL_RGBA										0x1908
#define GL_LUMINANCE								0x1909
#define GL_LUMINANCE_ALPHA							0x190A

/* getters */
#define GL_SMOOTH_POINT_SIZE_RANGE					0x0B12
#define GL_SMOOTH_LINE_WIDTH_RANGE					0x0B22
#define GL_ALIASED_POINT_SIZE_RANGE					0x846D
#define GL_ALIASED_LINE_WIDTH_RANGE					0x846E
#define GL_MAX_LIGHTS								0x0D31
#define GL_MAX_TEXTURE_SIZE							0x0D33
#define GL_MAX_MODELVIEW_STACK_DEPTH				0x0D36
#define GL_MAX_PROJECTION_STACK_DEPTH				0x0D38
#define GL_MAX_TEXTURE_STACK_DEPTH					0x0D39
#define GL_MAX_VIEWPORT_DIMS						0x0D3A
#define GL_SUBPIXEL_BITS							0x0D50
#define GL_RED_BITS									0x0D52
#define GL_GREEN_BITS								0x0D53
#define GL_BLUE_BITS								0x0D54
#define GL_ALPHA_BITS								0x0D55
#define GL_DEPTH_BITS								0x0D56
#define GL_STENCIL_BITS								0x0D57
#define GL_MAX_ELEMENTS_VERTICES					0x80E8
#define GL_MAX_ELEMENTS_INDICES						0x80E9
#define GL_MAX_TEXTURE_UNITS						0x84E2
#define GL_NUM_COMPRESSED_TEXTURE_FORMATS			0x86A2
#define GL_COMPRESSED_TEXTURE_FORMATS				0x86A3
#define GL_IMPLEMENTATION_COLOR_READ_TYPE_OES		0x8B9A
#define GL_IMPLEMENTATION_COLOR_READ_Format_OES		0x8B9B

#if (GLESEM_GL_BUILD_VERSION >= 110)
#	define GL_MAX_PALETTE_MATRICES_OES				0x8842
#	define GL_MAX_VERTEX_UNITS_OES					0x86A4
#	define GL_MAX_CLIP_PLANES						0x0D32

/* dynamic gets */
#	define GL_CLIENT_ACTIVE_TEXTURE					0x84E1
#	define GL_VERTEX_ARRAY_SIZE						0x807A
#	define GL_VERTEX_ARRAY_TYPE						0x807B
#	define GL_VERTEX_ARRAY_POINTER					0x808E
#	define GL_VERTEX_ARRAY_STRIDE					0x807C
#	define GL_NORMAL_ARRAY_TYPE						0x807E
#	define GL_NORMAL_ARRAY_STRIDE					0x807F
#	define GL_NORMAL_ARRAY_POINTER					0x808F
#	define GL_COLOR_ARRAY_SIZE						0x8081
#	define GL_COLOR_ARRAY_TYPE						0x8082
#	define GL_COLOR_ARRAY_STRIDE					0x8083
#	define GL_COLOR_ARRAY_POINTER					0x8090
#	define GL_TEXTURE_COORD_ARRAY_SIZE				0x8088
#	define GL_TEXTURE_COORD_ARRAY_TYPE				0x8089
#	define GL_TEXTURE_COORD_ARRAY_STRIDE			0x808A
#	define GL_TEXTURE_COORD_ARRAY_POINTER			0x8092
#	define GL_ARRAY_BUFFER_BINDING					0x8894
#	define GL_VERTEX_ARRAY_BUFFER_BINDING			0x8896
#	define GL_NORMAL_ARRAY_BUFFER_BINDING			0x8897
#	define GL_COLOR_ARRAY_BUFFER_BINDING			0x8898
#	define GL_TEXTURE_COORD_ARRAY_BUFFER_BINDING	0x889A
#	define GL_ELEMENT_ARRAY_BUFFER_BINDING			0x8895
#	define GL_VIEWPORT								0x0BA2
#	define GL_DEPTH_RANGE							0x0B70
#	define GL_MATRIX_MODE							0x0BA0
#	define GL_SHADE_MODEL							0x0B54
#	define GL_POINT_SIZE							0x0B11
#	define GL_LINE_WIDTH							0x0B21
#	define GL_CULL_FACE_MODE						0x0B45
#	define GL_FRONT_FACE							0x0B46
#	define GL_POLYGON_OFFSET_FACTOR					0x8038
#	define GL_POLYGON_OFFSET_UNITS					0x2A00
#	define GL_TEXTURE_BINDING_2D					0x8069
#	define GL_ACTIVE_TEXTURE						0x84E0
#	define GL_SCISSOR_BOX							0x0C10
#	define GL_ALPHA_TEST_FUNC						0x0BC1
#	define GL_ALPHA_TEST_REF						0x0BC2
#	define GL_STENCIL_FUNC							0x0B92
#	define GL_STENCIL_VALUE_MASK					0x0B93
#	define GL_STENCIL_REF							0x0B97
#	define GL_STENCIL_FAIL							0x0B94
#	define GL_STENCIL_PASS_DEPTH_FAIL				0x0B95
#	define GL_STENCIL_PASS_DEPTH_PASS				0x0B96
#	define GL_DEPTH_FUNC							0x0B74
#	define GL_BLEND_SRC								0x0BE1
#	define GL_BLEND_DST								0x0BE0
#	define GL_LOGIC_OP_MODE							0x0BF0
#	define GL_COLOR_WRITEMASK						0x0C23
#	define GL_DEPTH_WRITEMASK						0x0B72
#	define GL_STENCIL_WRITEMASK						0x0B98
#	define GL_COLOR_CLEAR_VALUE						0x0C22
#	define GL_DEPTH_CLEAR_VALUE						0x0B73
#	define GL_STENCIL_CLEAR_VALUE					0x0B91
#	define GL_MODELVIEW_MATRIX						0x0BA6
#	define GL_PROJECTION_MATRIX						0x0BA7
#	define GL_TEXTURE_MATRIX						0x0BA8
#	define GL_MODELVIEW_STACK_DEPTH					0x0BA3
#	define GL_PROJECTION_STACK_DEPTH				0x0BA4
#	define GL_TEXTURE_STACK_DEPTH					0x0BA5
#	define GL_MATRIX_INDEX_ARRAY_SIZE_OES			0x8846
#	define GL_MATRIX_INDEX_ARRAY_TYPE_OES			0x8847
#	define GL_MATRIX_INDEX_ARRAY_STRIDE_OES			0x8848
#	define GL_MATRIX_INDEX_ARRAY_POINTER_OES		0x8849
#	define GL_MATRIX_INDEX_ARRAY_BUFFER_BINDING_OES 0x8B9E
#	define GL_WEIGHT_ARRAY_SIZE_OES					0x86AB
#	define GL_WEIGHT_ARRAY_TYPE_OES					0x86A9
#	define GL_WEIGHT_ARRAY_STRIDE_OES				0x86AA
#	define GL_WEIGHT_ARRAY_POINTER_OES				0x86AC
#	define GL_WEIGHT_ARRAY_BUFFER_BINDING_OES		0x889E
#	define GL_POINT_SIZE_ARRAY_TYPE_OES				0x898A
#	define GL_POINT_SIZE_ARRAY_STRIDE_OES			0x898B
#	define GL_POINT_SIZE_ARRAY_POINTER_OES			0x898C
#	define GL_POINT_SIZE_ARRAY_BUFFER_BINDING_OES	0x8B9F
#	define GL_SAMPLE_COVERAGE_INVERT				0x80AB
#	define GL_SAMPLE_COVERAGE_VALUE					0x80AA
#	define GL_POINT_SIZE_MIN						0x8126
#	define GL_POINT_SIZE_MAX						0x8127
#	define GL_POINT_FADE_THRESHOLD_SIZE				0x8128
#	define GL_POINT_DISTANCE_ATTENUATION			0x8129
#	define GL_CURRENT_COLOR							0x0B00
#	define GL_CURRENT_NORMAL						0x0B02
#	define GL_CURRENT_TEXTURE_COORDS				0x0B03
#	define GL_MODELVIEW_MATRIX_FLOAT_AS_INT_BITS_OES	0x898D
#	define GL_PROJECTION_MATRIX_FLOAT_AS_INT_BITS_OES	0x898E
#	define GL_TEXTURE_MATRIX_FLOAT_AS_INT_BITS_OES		0x898F
#endif /* GLESEM_GL_BUILD_VERSION >= 110 */

/* clip planes */
#if (GLESEM_GL_BUILD_VERSION >= 110)
#	define GL_CLIP_PLANE0							0x3000
#	define GL_CLIP_PLANE1							0x3001
#	define GL_CLIP_PLANE2							0x3002
#	define GL_CLIP_PLANE3							0x3003
#	define GL_CLIP_PLANE4							0x3004
#	define GL_CLIP_PLANE5							0x3005
#endif /* GLESEM_GL_BUILD_VERSION >= 110 */

/* hints */
#define GL_PERSPECTIVE_CORRECTION_HINT				0x0C50
#define GL_LINE_SMOOTH_HINT							0x0C52
#define GL_POINT_SMOOTH_HINT						0x0C51
#define GL_FOG_HINT									0x0C54
#define GL_DONT_CARE								0x1100
#define GL_FASTEST									0x1101
#define GL_NICEST									0x1102

#if (GLESEM_GL_BUILD_VERSION >= 110)
#	define GL_GENERATE_MIPMAP_HINT					0x8192
#endif /* GLESEM_GL_BUILD_VERSION >= 110 */

/* pixel store */
#define GL_UNPACK_ALIGNMENT							0x0CF5
#define GL_PACK_ALIGNMENT							0x0D05

/* multisample */
#define GL_MULTISAMPLE								0x809D
#define GL_SAMPLE_ALPHA_TO_COVERAGE					0x809E
#define GL_SAMPLE_ALPHA_TO_ONE						0x809F
#define GL_SAMPLE_COVERAGE							0x80A0
#define GL_SAMPLE_BUFFERS							0x80A8
#define GL_SAMPLES									0x80A9

/* texture mapping */
#define GL_TEXTURE_2D								0x0DE1
#define GL_TEXTURE_ENV								0x2300
#define GL_TEXTURE_ENV_MODE							0x2200
#define GL_TEXTURE_MAG_FILTER						0x2800
#define GL_TEXTURE_MIN_FILTER						0x2801
#define GL_TEXTURE_WRAP_S							0x2802
#define GL_TEXTURE_WRAP_T							0x2803
#define GL_TEXTURE_ENV_COLOR						0x2201
#define GL_MODULATE									0x2100
#define GL_DECAL									0x2101
#define GL_NEAREST									0x2600
#define GL_LINEAR									0x2601
#define GL_NEAREST_MIPMAP_NEAREST					0x2700
#define GL_LINEAR_MIPMAP_NEAREST					0x2701
#define GL_NEAREST_MIPMAP_LINEAR					0x2702
#define GL_LINEAR_MIPMAP_LINEAR						0x2703
#define GL_REPEAT									0x2901
#define GL_CLAMP_TO_EDGE							0x812F

#if (GLESEM_GL_BUILD_VERSION >= 110)
#	define GL_GENERATE_MIPMAP						0x8191
#	define GL_COORD_REPLACE_OES						0x8862
#	define GL_TEXTURE_CROP_RECT_OES					0x8B9D
#endif /* GLESEM_GL_BUILD_VERSION >= 110 */

/* tex env combine */
#if (GLESEM_GL_BUILD_VERSION >= 110)
#	define GL_COMBINE								0x8570
#	define GL_COMBINE_RGB							0x8571
#	define GL_COMBINE_ALPHA							0x8572
#	define GL_SOURCE0_RGB							0x8580
#	define GL_SOURCE1_RGB							0x8581
#	define GL_SOURCE2_RGB							0x8582
#	define GL_SOURCE0_ALPHA							0x8588
#	define GL_SOURCE1_ALPHA							0x8589
#	define GL_SOURCE2_ALPHA							0x858A
#	define GL_SRC0_RGB								GL_SOURCE0_RGB		
#	define GL_SRC1_RGB								GL_SOURCE1_RGB		
#	define GL_SRC2_RGB								GL_SOURCE2_RGB		
#	define GL_SRC0_ALPHA							GL_SOURCE0_ALPHA		
#	define GL_SRC1_ALPHA							GL_SOURCE1_ALPHA		
#	define GL_SRC2_ALPHA							GL_SOURCE2_ALPHA		
#	define GL_OPERAND0_RGB							0x8590
#	define GL_OPERAND1_RGB							0x8591
#	define GL_OPERAND2_RGB							0x8592
#	define GL_OPERAND0_ALPHA						0x8598
#	define GL_OPERAND1_ALPHA						0x8599
#	define GL_OPERAND2_ALPHA						0x859A
#	define GL_RGB_SCALE								0x8573
#	define GL_ALPHA_SCALE							0x0D1C
#	define GL_ADD_SIGNED							0x8574
#	define GL_INTERPOLATE							0x8575
#	define GL_SUBTRACT								0x84E7
#	define GL_DOT3_RGB								0x86AE
#	define GL_DOT3_RGBA								0x86AF
#	define GL_CONSTANT								0x8576
#	define GL_PRIMARY_COLOR							0x8577
#	define GL_PREVIOUS								0x8578
#endif /* GLESEM_GL_BUILD_VERSION >= 110 */

/* paletted internal formats */
#define GL_PALETTE4_RGB8_OES						0x8B90
#define GL_PALETTE4_RGBA8_OES						0x8B91
#define GL_PALETTE4_R5_G6_B5_OES					0x8B92
#define GL_PALETTE4_RGBA4_OES						0x8B93
#define GL_PALETTE4_RGB5_A1_OES						0x8B94
#define GL_PALETTE8_RGB8_OES						0x8B95
#define GL_PALETTE8_RGBA8_OES						0x8B96
#define GL_PALETTE8_R5_G6_B5_OES					0x8B97
#define GL_PALETTE8_RGBA4_OES						0x8B98
#define GL_PALETTE8_RGB5_A1_OES						0x8B99

/* utility */
#define GL_VENDOR									0x1F00
#define GL_RENDERER									0x1F01
#define GL_VERSION									0x1F02
#define GL_EXTENSIONS								0x1F03

/* errors */
#define GL_NO_ERROR									0
#define GL_INVALID_ENUM								0x0500
#define GL_INVALID_VALUE							0x0501
#define GL_INVALID_OPERATION						0x0502
#define GL_STACK_OVERFLOW							0x0503
#define GL_STACK_UNDERFLOW							0x0504
#define GL_OUT_OF_MEMORY							0x0505

/* texture formats */
#define GL_UNSIGNED_SHORT_5_6_5						0x8363
#define GL_UNSIGNED_SHORT_4_4_4_4					0x8033
#define GL_UNSIGNED_SHORT_5_5_5_1					0x8034

/* buffer bits */
#define GL_DEPTH_BUFFER_BIT							0x00000100
#define GL_STENCIL_BUFFER_BIT						0x00000400
#define GL_COLOR_BUFFER_BIT							0x00004000

/* ARB_multitexture */
#define GL_TEXTURE0									0x84C0
#define GL_TEXTURE1									0x84C1
#define GL_TEXTURE2									0x84C2
#define GL_TEXTURE3									0x84C3
#define GL_TEXTURE4									0x84C4
#define GL_TEXTURE5									0x84C5
#define GL_TEXTURE6									0x84C6
#define GL_TEXTURE7									0x84C7
#define GL_TEXTURE8									0x84C8
#define GL_TEXTURE9									0x84C9
#define GL_TEXTURE10								0x84CA
#define GL_TEXTURE11								0x84CB
#define GL_TEXTURE12								0x84CC
#define GL_TEXTURE13								0x84CD
#define GL_TEXTURE14								0x84CE
#define GL_TEXTURE15								0x84CF
#define GL_TEXTURE16								0x84D0
#define GL_TEXTURE17								0x84D1
#define GL_TEXTURE18								0x84D2
#define GL_TEXTURE19								0x84D3
#define GL_TEXTURE20								0x84D4
#define GL_TEXTURE21								0x84D5
#define GL_TEXTURE22								0x84D6
#define GL_TEXTURE23								0x84D7
#define GL_TEXTURE24								0x84D8
#define GL_TEXTURE25								0x84D9
#define GL_TEXTURE26								0x84DA
#define GL_TEXTURE27								0x84DB
#define GL_TEXTURE28								0x84DC
#define GL_TEXTURE29								0x84DD
#define GL_TEXTURE30								0x84DE
#define GL_TEXTURE31								0x84DF

GLESEM_DEFINEPROCEDURE(void,glActiveTexture,(GLenum texture));
GLESEM_DEFINEPROCEDURE(void,glAlphaFunc,(GLenum func, GLclampf ref));
GLESEM_DEFINEPROCEDURE(void,glAlphaFuncx,(GLenum func, GLclampx ref));
GLESEM_DEFINEPROCEDURE(void,glBindTexture,(GLenum target, GLuint texture));
GLESEM_DEFINEPROCEDURE(void,glBlendFunc,(GLenum sfactor, GLenum dfactor));
GLESEM_DEFINEPROCEDURE(void,glClear,(GLbitfield mask));
GLESEM_DEFINEPROCEDURE(void,glClearColor,(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha));
GLESEM_DEFINEPROCEDURE(void,glClearColorx,(GLclampx red, GLclampx green, GLclampx blue, GLclampx alpha));
GLESEM_DEFINEPROCEDURE(void,glClearDepthf,(GLclampf depth));
GLESEM_DEFINEPROCEDURE(void,glClearDepthx,(GLclampx depth));
GLESEM_DEFINEPROCEDURE(void,glClearStencil,(GLint s));
GLESEM_DEFINEPROCEDURE(void,glClientActiveTexture,(GLenum texture));
GLESEM_DEFINEPROCEDURE(void,glColor4f,(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha));
GLESEM_DEFINEPROCEDURE(void,glColor4x,(GLfixed red, GLfixed green, GLfixed blue, GLfixed alpha));
GLESEM_DEFINEPROCEDURE(void,glColorMask,(GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha));
GLESEM_DEFINEPROCEDURE(void,glColorPointer,(GLint size, GLenum type, GLsizei stride, const GLvoid* ptr));
GLESEM_DEFINEPROCEDURE(void,glCompressedTexImage2D,(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const GLvoid* data));
GLESEM_DEFINEPROCEDURE(void,glCompressedTexSubImage2D,(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const GLvoid* data));
GLESEM_DEFINEPROCEDURE(void,glCopyTexImage2D,(GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border));
GLESEM_DEFINEPROCEDURE(void,glCopyTexSubImage2D,(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height));
GLESEM_DEFINEPROCEDURE(void,glCullFace,(GLenum mode));
GLESEM_DEFINEPROCEDURE(void,glDeleteTextures,(GLsizei n, const GLuint* textures));
GLESEM_DEFINEPROCEDURE(void,glDepthFunc,(GLenum func));
GLESEM_DEFINEPROCEDURE(void,glDepthMask,(GLboolean flag));
GLESEM_DEFINEPROCEDURE(void,glDepthRangef,(GLclampf nearValue, GLclampf farValue));
GLESEM_DEFINEPROCEDURE(void,glDepthRangex,(GLclampx nearValue, GLclampx farValue));
GLESEM_DEFINEPROCEDURE(void,glDisable,(GLenum cap));
GLESEM_DEFINEPROCEDURE(void,glDisableClientState,(GLenum cap));
GLESEM_DEFINEPROCEDURE(void,glDrawArrays,(GLenum mode, GLint first, GLsizei count));
GLESEM_DEFINEPROCEDURE(void,glDrawElements,(GLenum mode, GLsizei count, GLenum type, const GLvoid* indices));
GLESEM_DEFINEPROCEDURE(void,glEnable,(GLenum cap));
GLESEM_DEFINEPROCEDURE(void,glEnableClientState,(GLenum cap));
GLESEM_DEFINEPROCEDURE(void,glFinish,(void));
GLESEM_DEFINEPROCEDURE(void,glFlush,(void));
GLESEM_DEFINEPROCEDURE(void,glFogf,(GLenum pname, GLfloat param));
GLESEM_DEFINEPROCEDURE(void,glFogfv,(GLenum pname, const GLfloat* params));
GLESEM_DEFINEPROCEDURE(void,glFogx,(GLenum pname, GLfixed param));
GLESEM_DEFINEPROCEDURE(void,glFogxv,(GLenum pname, const GLfixed* params));
GLESEM_DEFINEPROCEDURE(void,glFrontFace,(GLenum mode));
GLESEM_DEFINEPROCEDURE(void,glFrustumf,(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat near_val, GLfloat far_val));
GLESEM_DEFINEPROCEDURE(void,glFrustumx,(GLfixed left, GLfixed right, GLfixed bottom, GLfixed top, GLfixed near_val, GLfixed far_val));
GLESEM_DEFINEPROCEDURE(void,glGenTextures,(GLsizei n, GLuint* textures));
GLESEM_DEFINEPROCEDURE(GLenum,glGetError,(void));
GLESEM_DEFINEPROCEDURE(void,glGetIntegerv,(GLenum pname, GLint* params));
GLESEM_DEFINEPROCEDURE(const GLubyte*,glGetString,(GLenum name));
GLESEM_DEFINEPROCEDURE(void,glHint,(GLenum target, GLenum mode));
GLESEM_DEFINEPROCEDURE(void,glLightf,(GLenum light, GLenum pname, GLfloat param));
GLESEM_DEFINEPROCEDURE(void,glLightfv,(GLenum light, GLenum pname, const GLfloat* params));
GLESEM_DEFINEPROCEDURE(void,glLightx,(GLenum light, GLenum pname, GLfixed param));
GLESEM_DEFINEPROCEDURE(void,glLightxv,(GLenum light, GLenum pname, const GLfixed* params));
GLESEM_DEFINEPROCEDURE(void,glLightModelf,(GLenum pname, GLfloat param));
GLESEM_DEFINEPROCEDURE(void,glLightModelfv,(GLenum pname, const GLfloat* params));
GLESEM_DEFINEPROCEDURE(void,glLightModelx,(GLenum pname, GLfixed param));
GLESEM_DEFINEPROCEDURE(void,glLightModelxv,(GLenum pname, const GLfixed* params));
GLESEM_DEFINEPROCEDURE(void,glLineWidth,(GLfloat width));
GLESEM_DEFINEPROCEDURE(void,glLineWidthx,(GLfixed width));
GLESEM_DEFINEPROCEDURE(void,glLoadIdentity,(void));
GLESEM_DEFINEPROCEDURE(void,glLoadMatrixf,(const GLfloat* m));
GLESEM_DEFINEPROCEDURE(void,glLoadMatrixx,(const GLfixed* m));
GLESEM_DEFINEPROCEDURE(void,glLogicOp,(GLenum opcode));
GLESEM_DEFINEPROCEDURE(void,glMaterialf,(GLenum face, GLenum pname, GLfloat param));
GLESEM_DEFINEPROCEDURE(void,glMaterialfv,(GLenum face, GLenum pname, const GLfloat* params));
GLESEM_DEFINEPROCEDURE(void,glMaterialx,(GLenum face, GLenum pname, GLfixed param));
GLESEM_DEFINEPROCEDURE(void,glMaterialxv,(GLenum face, GLenum pname, const GLfixed* params));
GLESEM_DEFINEPROCEDURE(void,glMatrixMode,(GLenum mode));
GLESEM_DEFINEPROCEDURE(void,glMultiTexCoord4f,(GLenum target, GLfloat s, GLfloat t, GLfloat r, GLfloat q));
GLESEM_DEFINEPROCEDURE(void,glMultiTexCoord4x,(GLenum target, GLfixed s, GLfixed t, GLfixed r, GLfixed q));
GLESEM_DEFINEPROCEDURE(void,glMultMatrixf,(const GLfloat* m));
GLESEM_DEFINEPROCEDURE(void,glMultMatrixx,(const GLfixed* m));
GLESEM_DEFINEPROCEDURE(void,glNormal3f,(GLfloat nx, GLfloat ny, GLfloat nz));
GLESEM_DEFINEPROCEDURE(void,glNormal3x,(GLfixed nx, GLfixed ny, GLfixed nz));
GLESEM_DEFINEPROCEDURE(void,glNormalPointer,(GLenum type, GLsizei stride, const GLvoid* ptr));
GLESEM_DEFINEPROCEDURE(void,glOrthof,(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat near_val, GLfloat far_val));
GLESEM_DEFINEPROCEDURE(void,glOrthox,(GLfixed left, GLfixed right, GLfixed bottom, GLfixed top, GLfixed near_val, GLfixed far_val));
GLESEM_DEFINEPROCEDURE(void,glPixelStorei,(GLenum pname, GLint param));
GLESEM_DEFINEPROCEDURE(void,glPointSize,(GLfloat size));
GLESEM_DEFINEPROCEDURE(void,glPointSizex,(GLfixed size));
GLESEM_DEFINEPROCEDURE(void,glPolygonOffset,(GLfloat factor, GLfloat units));
GLESEM_DEFINEPROCEDURE(void,glPolygonOffsetx,(GLfixed factor, GLfixed units));
GLESEM_DEFINEPROCEDURE(void,glPopMatrix,(void));
GLESEM_DEFINEPROCEDURE(void,glPushMatrix,(void));
GLESEM_DEFINEPROCEDURE(void,glReadPixels,(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid* pixels));
GLESEM_DEFINEPROCEDURE(void,glRotatef,(GLfloat angle, GLfloat x, GLfloat y, GLfloat z));
GLESEM_DEFINEPROCEDURE(void,glRotatex,(GLfixed angle, GLfixed x, GLfixed y, GLfixed z));
GLESEM_DEFINEPROCEDURE(void,glSampleCoverage,(GLclampf value, GLboolean invert));
GLESEM_DEFINEPROCEDURE(void,glSampleCoveragex,(GLclampx value, GLboolean invert));
GLESEM_DEFINEPROCEDURE(void,glScalef,(GLfloat x, GLfloat y, GLfloat z));
GLESEM_DEFINEPROCEDURE(void,glScalex,(GLfixed x, GLfixed y, GLfixed z));
GLESEM_DEFINEPROCEDURE(void,glScissor,(GLint x, GLint y, GLsizei width, GLsizei height));
GLESEM_DEFINEPROCEDURE(void,glShadeModel,(GLenum mode));
GLESEM_DEFINEPROCEDURE(void,glStencilFunc,(GLenum func, GLint ref, GLuint mask));
GLESEM_DEFINEPROCEDURE(void,glStencilMask,(GLuint mask));
GLESEM_DEFINEPROCEDURE(void,glStencilOp,(GLenum fail, GLenum zfail, GLenum zpass));
GLESEM_DEFINEPROCEDURE(void,glTexCoordPointer,(GLint size, GLenum type, GLsizei stride, const GLvoid* ptr));
GLESEM_DEFINEPROCEDURE(void,glTexEnvf,(GLenum target, GLenum pname, GLfloat param));
GLESEM_DEFINEPROCEDURE(void,glTexEnvfv,(GLenum target, GLenum pname, const GLfloat* params));
GLESEM_DEFINEPROCEDURE(void,glTexEnvx,(GLenum target, GLenum pname, GLfixed param));
GLESEM_DEFINEPROCEDURE(void,glTexEnvxv,(GLenum target, GLenum pname, const GLfixed* params));
GLESEM_DEFINEPROCEDURE(void,glTexImage2D,(GLenum target, GLint level, GLint internalFormat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid* pixels));
GLESEM_DEFINEPROCEDURE(void,glTexParameterf,(GLenum target, GLenum pname, GLfloat param));
GLESEM_DEFINEPROCEDURE(void,glTexParameterx,(GLenum target, GLenum pname, GLfixed param));
GLESEM_DEFINEPROCEDURE(void,glTexSubImage2D,(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid* pixels));
GLESEM_DEFINEPROCEDURE(void,glTranslatef,(GLfloat x, GLfloat y, GLfloat z));
GLESEM_DEFINEPROCEDURE(void,glTranslatex,(GLfixed x, GLfixed y, GLfixed z));
GLESEM_DEFINEPROCEDURE(void,glVertexPointer,(GLint size, GLenum type, GLsizei stride, const GLvoid* ptr));
GLESEM_DEFINEPROCEDURE(void,glViewport,(GLint x, GLint y, GLsizei width, GLsizei height));

#if (GLESEM_GL_BUILD_VERSION >= 110)
	GLESEM_DEFINEPROCEDURE(void,glBindBuffer,(GLenum target, GLuint buffer));
	GLESEM_DEFINEPROCEDURE(void,glBufferData,(GLenum target, GLsizeiptr size, const GLvoid* data, GLenum usage));
	GLESEM_DEFINEPROCEDURE(void,glBufferSubData,(GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid* data));
	GLESEM_DEFINEPROCEDURE(void,glColor4ub,(GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha));
	GLESEM_DEFINEPROCEDURE(void,glCurrentPaletteMatrixOES,(GLuint matrix));
	GLESEM_DEFINEPROCEDURE(void,glDeleteBuffers,(GLsizei n, const GLuint* buffers));
	GLESEM_DEFINEPROCEDURE(void,glGenBuffers,(GLsizei n, GLuint* buffers));
	GLESEM_DEFINEPROCEDURE(void,glGetBooleanv,(GLenum pname, GLboolean* params));
	GLESEM_DEFINEPROCEDURE(void,glGetBufferParameteriv,(GLenum target, GLenum pname, GLint* params));
	GLESEM_DEFINEPROCEDURE(void,glGetClipPlanef,(GLenum plane, GLfloat* equation));
	GLESEM_DEFINEPROCEDURE(void,glGetClipPlanex,(GLenum plane, GLfixed* equation));
	GLESEM_DEFINEPROCEDURE(void,glGetFloatv,(GLenum pname, GLfloat* params));
	GLESEM_DEFINEPROCEDURE(void,glGetFixedv,(GLenum pname, GLfixed* params));
	GLESEM_DEFINEPROCEDURE(void,glGetLightfv,(GLenum light, GLenum pname, GLfloat* params));
	GLESEM_DEFINEPROCEDURE(void,glGetLightxv,(GLenum light, GLenum pname, GLfixed* params));
	GLESEM_DEFINEPROCEDURE(void,glGetMaterialfv,(GLenum face, GLenum pname, GLfloat* params));
	GLESEM_DEFINEPROCEDURE(void,glGetMaterialxv,(GLenum face, GLenum pname, GLfixed* params));
	GLESEM_DEFINEPROCEDURE(void,glGetPointerv,(GLenum pname, GLvoid** params));
	GLESEM_DEFINEPROCEDURE(void,glGetTexEnvfv,(GLenum target, GLenum pname, GLfloat* params));
	GLESEM_DEFINEPROCEDURE(void,glGetTexEnviv,(GLenum target, GLenum pname, GLint* params));
	GLESEM_DEFINEPROCEDURE(void,glGetTexEnvxv,(GLenum target, GLenum pname, GLfixed* params));
	GLESEM_DEFINEPROCEDURE(void,glGetTexParameteriv,(GLenum target, GLenum pname, GLint* params));
	GLESEM_DEFINEPROCEDURE(void,glGetTexParameterfv,(GLenum target, GLenum pname, GLfloat* params));
	GLESEM_DEFINEPROCEDURE(void,glGetTexParameterxv,(GLenum target, GLenum pname, GLfixed* params));
	GLESEM_DEFINEPROCEDURE(GLboolean,glIsEnabled,(GLenum cap));
	GLESEM_DEFINEPROCEDURE(GLboolean,glIsTexture,(GLuint texture));
	GLESEM_DEFINEPROCEDURE(GLboolean,glIsBuffer,(GLuint buffer));
	GLESEM_DEFINEPROCEDURE(void,glLoadPaletteFromModelViewMatrixOES,(void));
	GLESEM_DEFINEPROCEDURE(void,glMatrixIndexPointerOES,(GLint size, GLenum type, GLsizei stride, const GLvoid* pointer));
	GLESEM_DEFINEPROCEDURE(void,glWeightPointerOES,(GLint size, GLenum type, GLsizei stride, const GLvoid* pointer));
	GLESEM_DEFINEPROCEDURE(void,glClipPlanef,(GLenum plane, const GLfloat* equation));
	GLESEM_DEFINEPROCEDURE(void,glClipPlanex,(GLenum plane, const GLfixed* equation));
	GLESEM_DEFINEPROCEDURE(void,glPointSizePointerOES,(GLenum type, GLsizei stride, const GLvoid* pointer));
	GLESEM_DEFINEPROCEDURE(void,glPointParameterfv,(GLenum pname, const GLfloat* params));
	GLESEM_DEFINEPROCEDURE(void,glPointParameterxv,(GLenum pname, const GLfixed* params));
	GLESEM_DEFINEPROCEDURE(void,glPointParameterf,(GLenum pname, GLfloat params));
	GLESEM_DEFINEPROCEDURE(void,glPointParameterx,(GLenum pname, GLfixed params));
	GLESEM_DEFINEPROCEDURE(void,glDrawTexfOES,(GLfloat sx, GLfloat sy, GLfloat sz, GLfloat sw, GLfloat sh));
	GLESEM_DEFINEPROCEDURE(void,glDrawTexxOES,(GLfixed sx, GLfixed sy, GLfixed sz, GLfixed sw, GLfixed sh));
	GLESEM_DEFINEPROCEDURE(void,glDrawTexiOES,(GLint sx, GLint sy, GLint sz, GLint sw, GLint sh));
	GLESEM_DEFINEPROCEDURE(void,glDrawTexsOES,(GLshort sx, GLshort sy, GLshort sz, GLshort sw, GLshort sh));
	GLESEM_DEFINEPROCEDURE(void,glDrawTexfvOES,(GLfloat* params));
	GLESEM_DEFINEPROCEDURE(void,glDrawTexxvOES,(GLfixed* params));
	GLESEM_DEFINEPROCEDURE(void,glDrawTexivOES,(GLint* params));
	GLESEM_DEFINEPROCEDURE(void,glDrawTexsvOES,(GLshort* params));
	GLESEM_DEFINEPROCEDURE(void,glTexEnvi,(GLenum target, GLenum pname, GLint param));
	GLESEM_DEFINEPROCEDURE(void,glTexEnviv,(GLenum target, GLenum pname, const GLint* params));
	GLESEM_DEFINEPROCEDURE(void,glTexParameteri,(GLenum target, GLenum pname, GLint param));
	GLESEM_DEFINEPROCEDURE(void,glTexParameterfv,(GLenum target, GLenum pname, const GLfloat* param));
	GLESEM_DEFINEPROCEDURE(void,glTexParameterxv,(GLenum target, GLenum pname, const GLfixed* param));
	GLESEM_DEFINEPROCEDURE(void,glTexParameteriv,(GLenum target, GLenum pname, const GLint* param));
#endif /* GLESEM_GL_BUILD_VERSION >= 110 */

GLESEM_DEFINEPROCEDURE(GLbitfield,glQueryMatrixxOES,(GLfixed mantissa[16], GLint exponent[16]));

GLESEMAPI int GLESEMAPIENTRY glesem_glInitialize();

#ifdef __cplusplus
}
#endif

#endif
