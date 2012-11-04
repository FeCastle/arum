SRC	= EventTest.C CounterTest.C HwCtrTest.C Loop.C Permutation.C Binary.C FunctionCall.C
HDR	= $(SRC:.C=.h) defs.h
OBJ	= $(SRC:.C=.o)

INCDIR	= ../src
ARCHIVE	= ../src/libarum.a

##LIB	= timer
INC	= -I $(INCDIR)
EXE	= EventTest CounterTest HwCtrTest Loop Permutation Binary FunctionCall

RM	= /bin/rm
MV	= /bin/mv
YACC	= /usr/bin/yacc
LEX	= /usr/bin/lex
SED	= /usr/bin/sed
ECHO    = echo

all:  $(EXE)

clean:  rmexe rmobj

DEBUGFLAGS=-g
CXXFLAGS=-O3 $(INC)
## CXXFLAGS=-O3
.C.o:
	$(CXX) $(CXXFLAGS) $(DEBUGFLAGS) -c $<

$(EXE):	$(OBJ)
	$(CXX) -o $@ $(CXXFLAGS) $(DEBUGFLAGS) $@.o $(ARCHIVE)

$(OBJ):	$(SRC)

rmexe:
	$(RM) -rf $(EXE)

rmobj:
	$(RM) -rf $(OBJ) 

force_it: 
	true
