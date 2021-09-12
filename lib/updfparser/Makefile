
AR ?= $(CROSS)ar
CXX ?= $(CROSS)g++

CXXFLAGS=-Wall -fPIC -I./include
LDFLAGS=

BUILD_STATIC ?= 0
BUILD_SHARED ?= 1

TARGETS =
ifneq ($(BUILD_STATIC), 0)
  TARGETS += libupdfparser.a
endif
ifneq ($(BUILD_SHARED), 0)
  TARGETS += libupdfparser.so
endif

ifneq ($(DEBUG),)
CXXFLAGS += -ggdb -O0
else
CXXFLAGS += -O2
endif

SRCDIR      := src
INCDIR      := inc
BUILDDIR    := obj
TARGETDIR   := bin
SRCEXT      := cpp
OBJEXT      := o

SOURCES = src/uPDFParser.cpp src/uPDFTypes.cpp
OBJECTS     := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.$(OBJEXT)))

all: obj $(TARGETS)

obj:
	mkdir obj

$(BUILDDIR)/%.$(OBJEXT): $(SRCDIR)/%.$(SRCEXT)
	$(CXX) $(CXXFLAGS) -c $^ -o $@

libupdfparser.a: $(OBJECTS)
	$(AR) crs $@ obj/*.o

libupdfparser.so: $(OBJECTS)
	$(CXX) obj/*.o $(LDFLAGS) -o $@ -shared

test: tests/test.cpp libupdfparser.a
	g++ -ggdb -O0 $^ -o $@ -Iinclude libupdfparser.a

clean:
	rm -rf libupdfparser.so libupdfparser.a obj test
