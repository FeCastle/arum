SRC	= EventTest.C CounterTest.C HwCtrTest.C Loop.C Permutation.C Binary.C FunctionCall.C Primes.C ArumTestSpec.C
HDR	= $(SRC:.C=.h) defs.h
OBJ	= $(SRC:.C=.o)

INCDIR	= ../src
ARCHIVE	= ../src/libarum.a

##LIB	= timer
INC	= -I $(INCDIR)
### These need the archive library (libarum.a) ###
ARUMEXE	= EventTest CounterTest HwCtrTest 
### These do not need to be linked with archive ###
TESTEXE = Loop Permutation Binary FunctionCall Primes ArumTestSpec

RM	= /bin/rm
MV	= /bin/mv
YACC	= /usr/bin/yacc
LEX	= /usr/bin/lex
SED	= /usr/bin/sed
ECHO    = echo

all:  $(ARUMEXE) $(TESTEXE)

clean:  rmexe rmobj

DEBUGFLAGS=-g
CXXFLAGS=-O3 $(INC)
## CXXFLAGS=-O3
.C.o:
	$(CXX) $(CXXFLAGS) $(DEBUGFLAGS) -c $<

$(ARUMEXE): $(OBJ)
	$(CXX) -o $@ $(CXXFLAGS) $(DEBUGFLAGS) $@.o $(ARCHIVE)

$(TESTEXE): $(OBJ)
	$(CXX) -o $@ $(CXXFLAGS) $(DEBUGFLAGS) $@.o 

$(OBJ):	$(SRC)

rmexe:
	$(RM) -rf $(ARUMEXE) $(TESTEXE)

rmobj:
	$(RM) -rf $(OBJ) 

force_it: 
	true
