###########################################################################
#
#   Makefile.inc
#
#  
#
#
###########################################################################

###########################################################################
#################   BEGIN USER-CONFIGURABLE OPTIONS   #####################
###########################################################################

#-------------------------------------------------
# specify OS target, which further differentiates
# the underlying OS; supported values are:
# macosx64, linux
#-------------------------------------------------

ifndef TARGETOS

UNAME = $(shell uname -a)

ifeq ($(firstword $(filter Linux,$(UNAME))),Linux)
TARGETOS = Linux
endif

# Autodetect PTR64
ifndef PTR64
ifeq ($(firstword $(filter x86_64,$(UNAME))),x86_64)
PTR64 = 1
endif
endif

endif #TARGETOS

#-------------------------------------------------
# specify program options; see each option below 
# for details
#-------------------------------------------------

# uncomment next line to build a debug version
DEBUG = 1

#-------------------------------------------------
# specify build options; see each option below 
# for details
#-------------------------------------------------
  
# uncomment next line if you want to force building for a 64-bit target
# PTR64 = 1

# uncomment next line to generate verbose build information
# VERBOSE = 1


###########################################################################
#################    END USER-CONFIGURABLE OPTIONS    #####################
###########################################################################

#-------------------------------------------------
# compile-time definitions
#-------------------------------------------------

DEL = rm -rf 

#-------------------------------------------------
# ROOT FLAGS&LIBS
#-------------------------------------------------

ROOTCFLAGS       := $(shell root-config --cflags)
ROOTLDFLAGS      := $(shell root-config --ldflags)
ROOTLIBS         := $(shell root-config --libs) -lMinuit -lNetx -lSQL -lRFIO -lTMVA -lXMLIO -lMLP -lTreePlayer
#ROOTGRAPHICSLIBS := -lGui -lGeom -lGeomPainter -lHistPainter
ROOTGLIBS     	 := $(shell root-config --glibs) -lGpad -lGX11 -lX3d -lGeom -lGeomPainter -lHistPainter

#-------------------------------------------------
# HEALPIX FLAGS&LIBS
#-------------------------------------------------

HEALDIR          := ../../LIBS/Healpix_2.20a/src/cxx/generic_gcc
INCHEALDIR       := $(HEALDIR)/include
HEALLIBS	 := -L$(HEALDIR)/lib -lhealpix_cxx -lcxxsupport -lpsht -lc_utils -lfftpack  

#-------------------------------------------------
# platform-specific definitions
#-------------------------------------------------

# compiler, linker and utilities

C 	:= $(shell root-config --cc)
CXX 	:= $(shell root-config --cxx)
FC 	:= $(shell root-config --f77)
LD 	:= $(shell root-config --ld)
SHARED 	:= -shared -fPIC

#-------------------------------------------------
# compile flags
#-------------------------------------------------

# no warning if when compiling C, give the address of one const char[length] 
# a non-const char * pointer
CXXFLAGS:= -O3 -fPIC -Wall -Wno-write-strings -fexceptions

# add -v if we need verbose build information
ifdef DEBUG
CXXFLAGS += -g
endif

# add -v if we need verbose build information
ifdef VERBOSE
CXXFLAGS += -v
endif

CXXFLAGS += $(ROOTCFLAGS)
CXXFLAGS += $(AMSSOFTFLAGS)

# Fortran flags
FCFLAGS:= -O3 -fPIC -Wall

#-------------------------------------------------
# link flags
#-------------------------------------------------

LDFLAGS	:= -O3
ifeq ($(PTR64),1)
LDFLAGS += $(ROOTLDFLAGS)
endif

#-------------------------------------------------
# comm libs
#-------------------------------------------------
COMMLIBS := -lgfortran

#-------------------------------------------------
#-------------------------------------------------
#AMS_NODE_LIB	:= -L$(ECMONDir)/AMSNode -lAMSNode

#AMS_ROOT_LIB    := $(ECMONDir)/AMSlib/lib/ntuple_slc4_PG.so
#AMS_ROOT_LIB   := -L$(ECMONDir)/AMSlib/lib/ -lntuple_slc4