### Basic definitions and includes ###

CPP_COMPILE	= $(CXX)
C_COMPILE	= $(CXX)
LINK		= $(CXX)
SRCDIR		= .
VPATH		= $(SRCDIR)
OBJS		= main.o 
APP_NAME	= meshTextureCombiner

### Build Versions ###

ifeq "$(VERSION)" ""
VERSION	= DEBUG
endif
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

CFLAGS	+= -DTOADLET_POSIX

### Library flags ###

LDFLAGS	= -o

### Include and Library paths ###

INCLUDES	= -I$(TOADLET_BASE_DIR)/include -I$(SRCDIR)
LIBS		= -L$(TOADLET_BASE_DIR)/lib -ltoadlet_egg -ltoadlet_peeper -ltoadlet_ribbit -ltoadlet_tadpole -ltoadlet_goo -ldl

### Making application ###

ifeq "$(VERSION)" "DEPS"
all: clobber depend

depend: $(OBJS)
else
all: App

App: $(OBJS)
	$(LINK) $(LDFLAGS) $(APP_NAME) $(OBJS) $(LIBS)
endif

install:
	cp $(APP_NAME) $(HOME)/bin

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

