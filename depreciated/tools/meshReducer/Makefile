### Basic definitions and includes ###

CPP_COMPILE	= $(CXX)
C_COMPILE	= $(CXX)
LINK		= $(CXX)
SRCDIR		= . reducer
VPATH		= $(SRCDIR)
OBJS		= ReducerGUI.o main.o Reducer.o Triangle.o Vertex.o
APP_NAME	= meshReducer

### Build Versions ###

ifeq "$(VERSION)" ""
VERSION	= DEBUG
endif
ifeq "$(VERSION)" "RELEASE"
CFLAGS	= -O2 -ffast-math -fomit-frame-pointer -finline-functions
endif
ifeq "$(VERSION)" "DEBUG"
CFLAGS	= -g -fno-inline -DDEBUG
SUF     = _d
endif
ifeq "$(VERSION)" "PROFILE"
CFLAGS	= -g -pg
endif
ifeq "$(VERSION)" "DEPS"
CFLAGS	= -MM
endif

### Library flags ###

LDFLAGS	= -o

### Include and Library paths ###

INCLUDES	= -I$(TOADLET_BASE_DIR)/include -I$(SRCDIR)
LIBS		= -L$(TOADLET_INSTALL_DIR)/lib -ltoadlet_egg$(SUF) -ltoadlet_hop$(SUF) -ltoadlet_peeper$(SUF) -ltoadlet_peeper_glrenderer$(SUF) -ltoadlet_ribbit$(SUF) -ltoadlet_ribbit_alplayer$(SUF) -ltoadlet_tadpole$(SUF) -ltoadlet_tadpole_hop$(SUF) -ltoadlet_pad$(SUF)

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

