%{
#include <stdio.h>
#include <string.h>
#include "meterparseryacc.h"
extern YYSTYPE meterlval;
extern int meterlex();
extern FILE *meterin;


%}

%option nounput
%option noinput
%option prefix="meter"

%%


Test-End    { return _TESTEND; }
[a-zA-Z-]+ { meterlval.strg = strdup(metertext); return _STRING; }
\.         { return _DOT; }
@          { return _AT; }
:          { return _DELIMITER; }
(([0-9][0-9]*\.?[0-9]*)|([0]\.[0-9]+))([Ee][+-]?[0-9]+)? { meterlval.float_val = atof(yytext); return _FLOATNUM; }
[ \t\r\n]  ; 
%%


