%{
#include "Interrupts.tab.h"
#include "Interrupts.lex.h"
%}

NAME	[a-zA-Z0-9_.+-]+
STRING	["][^"\n]*["]
COLON	:
COMMA	,
WS	[ \t]*
COMMENT	^[ \t]*#[^\n]*\n
EMPTY	^[ \t]*\n
EOL	\n

%%

{NAME}		{ return NAME; }
{STRING}	{ return STRING; }
{COLON}		{ return COLON; }
{COMMA}		{ return COMMA; }
{COMMENT}	;
{EMPTY}		;
{EOL}		{ return EOL; }
{WS}		;
\r		;
.		;
%%

#include <stdio.h>
#include <string.h>

int
yywrap()
{
    return 1;
}
