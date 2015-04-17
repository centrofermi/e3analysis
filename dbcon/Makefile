
include ./Makefile.inc

TARGET		:= ltcube.exe

MAINDIR		:= $(shell pwd)
INCDIR		:= $(MAINDIR)/inc
SRCDIR		:= $(MAINDIR)/src
BINDIR 		:= $(MAINDIR)/bin
LIBDIR 		:= $(MAINDIR)/lib

INCLUDES     	:= $(wildcard $(INCDIR)/*.h)                          
SOURCES      	:= $(wildcard $(SRCDIR)/*.cpp)
#SOURCES      	+= $(wildcard $(SRCDIR)/*.cxx)
CSOURCES      	:= $(wildcard $(SRCDIR)/*.c)
OBJECTS	     	+= $(SOURCES:$(SRCDIR)/%.cpp=$(BINDIR)/%.o)
#OBJECTS	 	+= $(SOURCES:$(SRCDIR)/%.cxx=$(BINDIR)/%.o)
OBJECTS	     	+= $(CSOURCES:$(SRCDIR)/%.c=$(BINDIR)/%.o)
AMSLIBS		:= $(AMSWD)/lib/$(shell root-config --arch)/ntuple_slc4_PG.so  

LINKDEFS	:= THealpixMap TVMap

#-------------------------------------------------
# primary targets
#-------------------------------------------------

.PHONY : clean

exec: $(TARGET)

$(TARGET): $(OBJECTS) $(AMSLIBS)
	@echo "Creating executable file $@"
	@$(LD) $(LDFLAGS) -D_PGTRACK_ $^ -o $@  $(COMMLIBS) $(ROOTLIBS) 
clean:
	@echo "Cleaning up ..."
	@$(DEL) $(MAINDIR)/$(TARGET) \
	$(OBJECTS) $(BINDIR)/*.d \

#-------------------------------------------------
# generic rules
#-------------------------------------------------

$(BINDIR)/%.o : $(SRCDIR)/%.cpp; 
	@echo "Compiling $< ... "
	@$(CXX) $(CXXFLAGS) -I$(INCDIR) -I$(INCHEALDIR) -I$(AMSWD)/include -D_PGTRACK_ -c $< -o $@

$(BINDIR)/%.o : $(SRCDIR)/%.c; 
	@echo "Compiling $< ..."
	@$(C) $(CXXFLAGS) -I$(INCDIR) -c $< -o $@

$(BINDIR)/%.o : $(SRCDIR)/%.f; 
	@echo "Compiling $< ..."
	@$(FC) $(FCFLAGS) -c $< -o $@

$(BINDIR)/%.d : $(SRCDIR)/%.cpp; 
	@echo "Making dependencies for file $< ..."
	@set -e;\
	$(CXX) -MM $(CXXFLAGS) -I$(INCDIR) -I$(INCHEALDIR) -I$(AMSWD)/include -D_PGTRACK_ -w $< | \
	sed 's!$*\.o!$(BINDIR)& $@!' >$@;\
	[ -s $@ ] || rm -f $@		

$(BINDIR)/%.d : $(SRCDIR)/%.c; 
	@echo "Making dependencies for file $< ..."
	@set -e;\
	$(C) -MM $(CXXFLAGS) -I$(INCDIR) -w $< | \
	sed 's!$*\.o!$(BINDIR)& $@!' >$@;\
	[ -s $@ ] || rm -f $@		

ifneq ($(MAKECMDGOALS),clean)
#-include $(SOURCES:$(SRCDIR)/%.cpp=$(BINDIR)/%.d)
endif


