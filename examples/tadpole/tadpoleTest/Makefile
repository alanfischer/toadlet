### Basic definitions and includes ###

CPP_COMPILE	= $(CXX)
C_COMPILE	= $(CXX)
LINK		= $(CXX)
SRCDIR		= .
VPATH		= $(SRCDIR)
OBJS		= main.o
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


### Platforms ###

ifeq "$(SYSTEM_UNAME)" "Linux"
CFLAGS += -DTOADLET_USE_DYNAMIC -DTOADLET_PLATFORM_LINUX -DTOADLET_PLATFORM_POSIX
endif
ifeq "$(SYSTEM_UNAME)" "Darwin"
CFLAGS += -DTOADLET_USE_DYNAMIC -DTOADLET_PLATFORM_OSX  -DTOADLET_PLATFORM_POSIX
LIBS = -framework Carbon
endif


### Library flags ###

LDFLAGS	= -o


### Include and Library paths ###

INCLUDES	+= -I$(TOADLET_BASE_DIR)/include
LIBS		+= -L$(TOADLET_BASE_DIR)/lib -ltoadlet_egg -ltoadlet_peeper -ltoadlet_peeper_glrenderer -ltoadlet_ribbit -ltoadlet_tadpole -ltoadlet_pad
APP_NAME	= tadpoleTest


### Making application ###

ifeq "$(VERSION)" "DEPS"
all: clobber depend

depend: $(OBJS)
else
all: tadpoleTest

tadpoleTest: $(OBJS)
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

