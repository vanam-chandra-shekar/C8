INCLUDEDIR = ray chip8 main util
SOURCEDIR = main chip8 util
LIBDIR = ray
BINDIR = bin
LANGUAGE = cpp
PREPROCESSER = WINDOWS
LIBRARIES = raylib gdi32 winmm

TARGET = main

CXX = clang++
CXXSTD = 20

INCLUDEDIR := $(foreach D,$(INCLUDEDIR),$(patsubst %,-I%,$(D)))
LIBDIR := $(foreach D,$(LIBDIR),$(patsubst %,-L%,$(D)))
PREPROCESSER := $(foreach D,$(PREPROCESSER),$(patsubst %,-D%,$(D)))
LIBRARIES := $(foreach D,$(LIBRARIES),$(patsubst %,-l%,$(D)))
SOURCEFILES := $(foreach D,$(SOURCEDIR),$(wildcard $(D)/*.$(LANGUAGE)))
OBJFILES := $(patsubst %.$(LANGUAGE),%.o,$(foreach D,$(SOURCEFILES),$(patsubst $(dir $(D))%,$(BINDIR)/%,$(D))))
DEPFILES := $(patsubst %.o,%.d,$(OBJFILES))

ifndef (CONFIG)
	CONFIG = DEBUG
endif

ifndef (VERBOS)
	SILENT = @
endif

ifeq ($(CONFIG),DEBUG)
	DEPFLAGS = -MP -MD
	CXXFLAGS = --std=c++$(CXXSTD) $(INCLUDEDIR) $(DEPFLAGS) -g $(PREPROCESSER)
endif

ifeq ($(CONFIG),RELEASE)
	DEPFLAGS = -MP -MD
	OPT = O2
	CXXFLAGS = --std=c++$(CXXSTD) $(INCLUDEDIR) $(DEPFLAGS) $(OPT) $(PREPROCESSER)
endif

LIBFLAGS = $(LIBDIR) $(LIBRARIES)

VPATH = $(SOURCEDIR)

.PHONY: all clean

all:$(TARGET)

$(TARGET):$(OBJFILES)
	$(SILENT)echo ===== Linking =====
	$(SILENT)$(CXX) $^ -o $@ $(LIBFLAGS)

$(BINDIR)/%.o:%.$(LANGUAGE) | $(BINDIR)
	$(SILENT) echo [Compiling $<]
	$(SILENT)$(CXX)  -c $< -o $@ $(CXXFLAGS)

$(BINDIR):
	$(SILENT)echo Creating $@ Dir
	$(SILENT)mkdir $@

clean:
	$(SILENT)echo ===== CLEANING =====
	$(SILENT)del /Q $(TARGET).exe
	$(SILENT)del /Q $(BINDIR)


-include $(DEPFILES)
