%{
#include <stdio.h>
#include <string.h>
#include "lpfparseryacc.h"
extern YYSTYPE lpflval;
extern int lpflex();
extern FILE *lpfin;

#define lpfterminate() return token::FILE_END


%}

%option nounput
%option noinput
%option prefix="lpf"

%%

time        { return _TIME; }
is        { return _IS; }
data       { return _DATA; }
value_       { return _VAL; }
timestamp_       { return _TSTAMP; }
[a-zA-Z-]+ { lpflval.strg = strdup(lpftext); return _STRING; }
@          { return _AT; }
(([0-9][0-9]*\.?[0-9]*)|([0]\.[0-9]+))([Ee][+-]?[0-9]+)? { lpflval.float_val = atof(yytext); return _FLOATNUM; }
[ \t\r\n]  ; 
%%


