
#####################################################################
##                                                                 ##
##                                                                 ##
##                                                                 ##
#####################################################################

# --------------------------- System and ROOT variables ---------------------------

S             = src
I             = include
O             = obj
L             = lib
B             = bin

SRC           = $(wildcard $(S)/TP*.cxx) $(S)/Dict.cxx
INCD          = $(wildcard $(I)/TP*.hh)
INC           = $(notdir $(INCD))
OBJD          = $(patsubst $(S)/%.cxx, $(O)/%.o, $(SRC))
OBJ           = $(notdir $(OBJD))

OSTYPE       := $(subst -,,$(shell uname))

#ROOTGLIBS    := $(shell root-config --glibs)
#ROOTCFLAGS   := $(shell root-config --cflags)
#ROOTLDFLAGS  := $(shell root-config --ldflags)

ROOTGLIBS    := $(shell root-config --libs --glibs) -lEG -lFoam
ROOTCFLAGS   := $(shell root-config --cflags)
ROOTLDFLAGS  := $(shell root-config --ldflags)

BIN_INSTALL_DIR = $(HOME)/$(B)

vpath %.cxx $(S)
vpath %.hh  $(I)
vpath %.o  $(O)

# ------------------------ Architecture dependent settings ------------------------

ifeq ($(OSTYPE),Darwin)
	LIB_AnaPANDA = $(L)/libAnaPANDA.dylib
	SOFLAGS = -dynamiclib -single_module -undefined dynamic_lookup -install_name $(CURDIR)/$(LIB_AnaPANDA)
	POST_LIB_BUILD = @ln $(L)/libAnaPANDA.dylib $(L)/libAnaPANDA.so
endif

ifeq ($(OSTYPE),Linux)
	LIB_AnaPANDA = $(L)/libAnaPANDA.so
	SOFLAGS = -shared
	POST_LIB_BUILD = 
endif

# -------------------------------- Compile options --------------------------------

CCCOMP      = g++
CXXFLAGS    = -g -O3 -Wall -fPIC $(ROOTCFLAGS) -I./$(I)
LDFLAGS     = -g -O3 $(ROOTLDFLAGS)

# ------------------------------------ targets ------------------------------------

all:	$(LIB_AnaPANDA) \
	$(B)/anapanda

$(B)/anapanda: $(LIB_AnaPANDA) $(S)/anapanda.cxx
	@echo
	@echo "Building the <anapanda> tool ..."
	@mkdir -p $(B)
	@$(CCCOMP) $(CXXFLAGS) $(ROOTGLIBS) $(ROOTCFLAGS) $(ROOTLDFLAGS) $(CURDIR)/$(LIB_AnaPANDA) \
		    -o $(B)/anapanda $(S)/anapanda.cxx

$(LIB_AnaPANDA): $(OBJ)
	@echo
	@echo "Building libAnaPANDA ..."
	@mkdir -p $(L)
	@rm -f $(L)/libAnaPANDA.*
	@$(CCCOMP) $(LDFLAGS) $(ROOTGLIBS) $(SOFLAGS) $(OBJD) -o $(LIB_AnaPANDA)
	@$(POST_LIB_BUILD)

$(S)/Dict.cxx: $(INC) $(I)/LinkDef.hh 
	@echo
	@echo "Creating CaLIB dictionary ..."
	@rootcint -v -f $@ -c -I./$(I) -p $(INC) $(I)/LinkDef.hh

%.o: %.cxx
	@echo "Compiling $(notdir $<) ..."
	@mkdir -p $(O)
	@$(CCCOMP) $(CXXFLAGS) -o $(O)/$@ -c $< 

docs:
	@echo "Creating HTML documentation ..."
	@rm -r -f htmldoc
#       @root -b -n -q $(S)/htmldoc.C > /dev/null 2>&1
	@root -b -n -q $(S)/htmldoc.C
	@echo "Done."

clean:
	@echo "Cleaning AnaPANDA distribution ..."
	rm -f $(S)/Dict.*
	rm -r -f $(L)
	rm -f -r $(O)
	rm -r -f $(B)
	rm -r -f htmldoc
	@echo "Done."
