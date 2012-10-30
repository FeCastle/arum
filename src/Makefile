obj-m := HwCtr.o

# SRY	= Interrupts.tab.y
# SRL	= Interrupts.lex.l
##SRC	= Main.C CpuId.C ResourceUsage.C Utilities.C $(SRY:.y=.C) $(SRL:.l=.C)
## Don't build "Main".  The "Main" is in Arum.C 
SRC	= Arum.C CpuId.C Events.C Arch.C ResourceUsage.C Utilities.C Counters.C $(SRY:.y=.C) $(SRL:.l=.C)
HDR	= $(SRC:.C=.h) defs.h
OBJ	= $(SRC:.C=.o)
##LIBDIR	= ../lib
##INCDIR	= ../include
##LIB	= timer
##INC	= -I $(INCDIR)
EXE	= Arum

RM	= /bin/rm
MV	= /bin/mv
YACC	= /usr/bin/yacc
LEX	= /usr/bin/lex
SED	= /usr/bin/sed

EXA	= $(EX1) $(EX2) $(EX3) $(EX4) $(EX5) $(EX6)

all: $(EXE)

clean:  rmexe rmobj

#	$(RM) $(EXE)
#	$(RM) *.o
#	$(RM) Interrupts.lex.C
#	$(RM) Interrupts.tab.h
#	$(RM) Interrupts.tab.C

## POSSIBLE DEBUG FLAGS
##    -DDEBUG_CPU  - Adds printf to CpuId module
##    -DDEBUG_COUNT - Adds printf to Counters module
DEBUG_FLAGS = -DDEBUG_CPU

##CXXFLAGS=-O3 $(INC)
CXXFLAGS=-O3 $(DEBUG_FLAGS)

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

##$(EXE):	$(OBJ)
##	$(CXX) -o $(EXE) $(CXXFLAGS) $(OBJ) -L $(LIBDIR) -l $(LIB)
##	$(RM) -rf $(EXA)
##	for i in $(EXA) ; do /bin/ln $(EXE) $$i ; done

$(EXE):	$(OBJ)
	$(CXX) -o $(EXE) $(CXXFLAGS) $(OBJ)
	# $(RM) -rf $(EXA)
	# for i in $(EXA) ; do /bin/ln $(EXE) $$i ; done

$(OBJ):	$(HDR)

rmexe:
	$(RM) -rf $(EXE)

rmobj:
	$(RM) -rf $(OBJ) $(SRY:.y=.C) $(SRY:.y=.h) $(SRL:.l=.C)

rmmod:
	$(RM) -rf HwCtr.ko HwCtr.mod.c HwCtr.mod.o HwCtr.o .HwCtr.ko.cmd .HwCtr.mod.o.cmd .HwCtr.o.cmd .tmp_versions/

Interrupts.lex.C:	Interrupts.lex.l
Interrupts.tab.h:	Interrupts.tab.y
Interrupts.tab.C:	Interrupts.tab.y
