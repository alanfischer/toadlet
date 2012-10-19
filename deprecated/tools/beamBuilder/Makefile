### Basic definitions and includes ###

CPP_COMPILE	= $(CXX)
C_COMPILE	= $(CXX)
LINK		= $(CXX)
SRCDIR		= .
VPATH		= $(SRCDIR)
OBJS		= main.o BeamBuilder.o
SYSTEM_UNAME	= $(shell uname)

### Build Versions ###

VERSION	= DEBUG

ifeq "$(VERSION)" "RELEASE"
CFLAGS	= -O2 -ffast-math -fomit-frame-pointer -finline-functions
endif
ifeq "$(VERSION)" "DEBUG"
CFLAGS	= -g -fno-inline -DDEBUG
endif
ifeq "$(VERSION)" "PROFILE"
CFLAGS	= -g -pg
endif
ifeq "$(VERSION)" "DEPS"
CFLAGS	= -MM
endif

CFLAGS += -DTOADLET_USE_DYNAMIC

### Library flags ###

LDFLAGS	= -o

### Include and Library paths ###

INCLUDES	= -I$(TOADLET_BASE_DIR)/include
LIBS		= -L$(TOADLET_BASE_DIR)/lib -ltoadlet_egg_d -ltoadlet_hop_d -ltoadlet_knot_d -ltoadlet_peeper_d -ltoadlet_ribbit_d -ltoadlet_peeper_glrenderer_d -ltoadlet_ribbit_alplayer_d -ltoadlet_tadpole_d -ltoadlet_tadpole_hop_d -ltoadlet_pad_d
APP_NAME	= beambuilder

ifeq "$(SYSTEM_UNAME)" "Darwin"
CFLAGS += -DTOADLET_PLATFORM_OSX
LIBS += -framework Carbon
endif

### Making application ###

ifeq "$(VERSION)" "DEPS"
all: clobber depend

depend: $(OBJS)
else
all: App

App: $(OBJS)
	$(LINK) $(LDFLAGS) $(APP_NAME) $(OBJS) $(LIBS)
endif

### Making objects ##

ifeq "$(VERSION)" "DEPS"
$(OBJS): %.o: %.cpp
	$(CPP_COMPILE) -c $< $(CFLAGS) $(CXX_FLAGS) $(INCLUDES) >> Dependencies
else
$(OBJS): %.o: %.cpp
	$(CPP_COMPILE) -o $@ -c $< $(CFLAGS) $(CXX_FLAGS) $(INCLUDES)
endif

### Clean up ###

clean:
	rm -f *.o
	rm -f core.*

clobber:
	$(MAKE) clean
	rm -f Dependencies
	rm -f $(APP_NAME)

### Dependencies must be last ###

ifneq "$(VERSION)" "DEPS"
include Dependencies
endif



