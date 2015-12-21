
include ./Makefile.inc

TARGET		:= dbConnTest.exe
TARGLIB 		:= libe3RunDbConn.so

MAINDIR		:= $(shell pwd)
INCDIR			:= $(MAINDIR)/inc
SRCDIR		:= $(MAINDIR)/src
BINDIR 			:= $(MAINDIR)/bin
LIBDIR	 		:= $(MAINDIR)/lib

LINKDEFS		:= 

INCLUDES     	:= $(wildcard $(INCDIR)/*.h)                          
SOURCES      	:= $(wildcard $(SRCDIR)/*.cpp)
CSOURCES      	:= $(wildcard $(SRCDIR)/*.c)
OBJECTS	     	+= $(SOURCES:$(SRCDIR)/%.cpp=$(BINDIR)/%.o)
OBJECTS	     	+= $(CSOURCES:$(SRCDIR)/%.c=$(BINDIR)/%.o)
MAINOBJ		:= $(BINDIR)/main.o

#-------------------------------------------------
# primary targets
#-------------------------------------------------

.PHONY : exec, lib, clean

exec: $(TARGET)

$(TARGET): $(MAINOBJ) $(TARGLIB)
	@echo "Creating executable file $@"
	@$(LD) $(LDFLAGS) $^ -o $@  $(COMMLIBS) $(ROOTLIBS) $(MYSQLLIBS)

lib: $(TARGLIB)

$(TARGLIB): $(filter-out $(MAINOBJ),$(OBJECTS))
	@echo "Creating shared library $@"
	@$(LD) $(SHARED) $(LDFLAGS) $^ -o $@  $(COMMLIBS) $(MYSQLLIBS)

clean:
	@echo "Cleaning up ..."
	@$(DEL) $(MAINDIR)/$(TARGET) \
	$(MAINDIR)/$(TARGLIB) \
	$(LIBDIR)/lib* \
	$(OBJECTS) $(BINDIR)/*.d

#-------------------------------------------------
# generic rules
#-------------------------------------------------

$(BINDIR)/%.o : $(SRCDIR)/%.cpp; 
	@echo "Compiling $< ... "
	@$(CXX) $(CXXFLAGS) -I$(INCDIR) -c $< -o $@

$(BINDIR)/%.o : $(SRCDIR)/%.c; 
	@echo "Compiling $< ..."
	@$(C) $(CXXFLAGS) -I$(INCDIR) -c $< -o $@

$(BINDIR)/%.d : $(SRCDIR)/%.cpp; 
	@echo "Making dependencies for file $< ..."
	@set -e;\
	$(CXX) -MM $(CXXFLAGS) -I$(INCDIR) -w $< | \
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


