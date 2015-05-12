
include ./Makefile.inc

TARGET		:= CoincAtCnaf.exe

MAINDIR		:= $(shell pwd)
INCDIR			:= $(MAINDIR)/inc
SRCDIR		:= $(MAINDIR)/src
BINDIR 			:= $(MAINDIR)/bin
LIBDIR 			:= $(MAINDIR)/lib

INCLUDES     	:= $(wildcard $(INCDIR)/*.h)                          
SOURCES      	:= $(wildcard $(SRCDIR)/*.C)
CSOURCES      	:= $(wildcard $(SRCDIR)/*.c)
OBJECTS	     	+= $(SOURCES:$(SRCDIR)/%.C=$(BINDIR)/%.o)
OBJECTS	     	+= $(CSOURCES:$(SRCDIR)/%.c=$(BINDIR)/%.o)

#LINKDEFS	:= THealpixMap TVMap

#-------------------------------------------------
# primary targets
#-------------------------------------------------

.PHONY : clean

#exec: $(TARGET)

$(TARGET): $(OBJECTS)
	@echo "Creating executable file $@"
	@$(LD) $(LDFLAGS) $^ -o $@  $(ROOTLIBS)  \
	 $(MYSQLLIBS) $(E3LIBS) 

clean:
	@echo "Cleaning up ..."
	@$(DEL) $(MAINDIR)/$(TARGET) \
	$(OBJECTS) $(BINDIR)/*.d \

#-------------------------------------------------
# generic rules
#-------------------------------------------------

$(BINDIR)/%.o : $(SRCDIR)/%.C; 
	@echo "Compiling $< ... "
	@$(CXX) $(CXXFLAGS) -I$(INCDIR) -c $< -o $@

$(BINDIR)/%.o : $(SRCDIR)/%.c; 
	@echo "Compiling $< ..."
	@$(C) $(CXXFLAGS) -I$(INCDIR) -c $< -o $@

$(BINDIR)/%.o : $(SRCDIR)/%.f; 
	@echo "Compiling $< ..."
	@$(FC) $(FCFLAGS) -c $< -o $@

$(BINDIR)/%.d : $(SRCDIR)/%.C; 
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


