#if !defined( Interrupts_lex_h )
#define Interrupts_lex_h

extern "C" int Interrupts_wrap ( void );
extern char Interrupts_text[];
extern int Interrupts_lineno;
int Interrupts_lex();

#endif
