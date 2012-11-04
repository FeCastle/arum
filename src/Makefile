obj-m := HwCtr.o
KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

SRC	= Arum.C Events.C Arch.C CpuId.C ResourceUsage.C Utilities.C Counters.C $(SRY:.y=.C) $(SRL:.l=.C)
HDR	= $(SRC:.C=.h) defs.h
OBJ	= $(SRC:.C=.o)
ARCHIVE = libarum.a
EXE	= Arum

RM	= /bin/rm
MV	= /bin/mv
YACC	= /usr/bin/yacc
LEX	= /usr/bin/lex
SED	= /usr/bin/sed
ECHO    = echo

EXA	= $(EX1) $(EX2) $(EX3) $(EX4) $(EX5) $(EX6)

all: Interrupts module $(ARCHIVE) $(EXE)

module: 
	make -C $(KDIR) M=$(PWD) modules

clean:  rmexe rmobj rmmod rmint

##CXXFLAGS=-O3 $(INC)
CXXFLAGS=-O3
.C.o:
	$(CXX) $(CXXFLAGS) -c $<

$(ARCHIVE): $(OBJ)
	ar -r $(ARCHIVE) $(OBJ)
	
$(EXE):	$(OBJ)
	$(CXX) -o $(EXE) $(CXXFLAGS) $(OBJ)
	# $(RM) -rf $(EXA)
	# for i in $(EXA) ; do /bin/ln $(EXE) $$i ; done

$(OBJ):	$(HDR)

Interrupts: force_it
	$(ECHO) looking into Interrupts : $(MAKE) $(MFLAGS)
	cd Interrupts; $(MAKE) $(MFLAGS)

rmexe:
	$(RM) -rf $(EXE)

rmobj:
	$(RM) -rf $(OBJ) $(SRY:.y=.C) $(SRY:.y=.h) $(SRL:.l=.C)
	$(RM) -f *.a

rmmod:
	$(RM) -rf HwCtr.ko HwCtr.mod.c HwCtr.mod.o HwCtr.o .HwCtr.ko.cmd .HwCtr.mod.o.cmd .HwCtr.o.cmd .tmp_versions/

rmint:
	cd Interrupts; $(MAKE) clean

force_it: 
	true
