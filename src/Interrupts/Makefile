YACC	= /usr/bin/yacc
LEX	= /usr/bin/lex
SED	= sed
IntTargets = Interrupts.lex.C Interrupts.tab.h Interrupts.tab.C

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

all: Interrupts

clean:
	rm -f $(IntTargets)

Interrupts: $(IntTargets)

Interrupts.lex.C:	Interrupts.lex.l
Interrupts.tab.h:	Interrupts.tab.y
Interrupts.tab.C:	Interrupts.tab.y
