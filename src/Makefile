obj-m := HwCtr.o
KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

# SRY	= Interrupts.tab.y
# SRL	= Interrupts.lex.l
##SRC	= Main.C CpuId.C ResourceUsage.C Utilities.C $(SRY:.y=.C) $(SRL:.l=.C)
## Don't build "Main".  The "Main" is in Arum.C 
SRC	= Arum.C CpuId.C Events.C Arch.C ResourceUsage.C Utilities.C Counters.C $(SRY:.y=.C) $(SRL:.l=.C)
HDR	= $(SRC:.C=.h) defs.h
OBJ	= $(SRC:.C=.o)
EXE	= Arum
ARCHIVE = libarum.a

RM	= /bin/rm
MV	= /bin/mv
YACC	= /usr/bin/yacc
LEX	= /usr/bin/lex
SED	= /usr/bin/sed
ECHO    = echo

EXA	= $(EX1) $(EX2) $(EX3) $(EX4) $(EX5) $(EX6)

all: Interrupts $(ARCHIVE) $(EXE)

module: 
	 make -C $(KDIR) M=$(PWD) modules

clean:  rmexe rmobj rmmod rmint

## POSSIBLE DEBUG FLAGS
##    -DDEBUG_ARCH  - Adds printf to Arch module
##    -DDEBUG_CPU   - Adds printf to CpuId module
##    -DDEBUG_COUNT - Adds printf to Counters module
DEBUG_FLAGS = -g 

##CXXFLAGS=-O3 $(INC)
CXXFLAGS=-O3 $(DEBUG_FLAGS) -I../bprobe/include

.C.o:
	$(CXX) $(CXXFLAGS) -c $<

.l.C:
	$(LEX) $<
	$(SED) -e s/yy/$(<:.lex.l=)_/g -e s/lex.yy.c/$</g lex.yy.c > $(<:.l=).C
	$(RM)  lex.yy.c

.y.C:
	$(YACC) $<
	$(SED) -e s/yy/$(<:.tab.y=)_/g -e s/y.tab.c/$</g y.tab.c > $(<:.y=).C
	$(RM)  y.tab.c

.y.h:
	$(YACC) -d $<
	$(SED) -e s/yy/$(<:.tab.y=)_/g -e s/y.tab.c/$</g y.tab.h > $(<:.y=).h
	$(RM)  y.tab.c y.tab.h

$(EXE):	$(ARCHIVE)
	$(CXX) -o $(EXE) $(CXXFLAGS)-rdynamic -Wl,-rpath=../bprobe/lib $(ARCHIVE) -L../bprobe/lib -lbinpro -Wl,--whole-archive -liberty -Wl,--no-whole-archive

$(ARCHIVE): $(OBJ)
	ar -ruv $(ARCHIVE) $(OBJ)

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
