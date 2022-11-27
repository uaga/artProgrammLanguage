/*	lexer V5
  * lex file
  *	recognize tokens
  */

%option noyywrap nodefault yylineno
%{
#include "v5.h"
#include "v5.tab.h"
%}

%%
 /* single character ops */
"+" |
"-" |
"*" |
"/" |
"&" |
"|" |
"!" |
"," |
";" |
"=" |
"{" |
"}" |
"(" |
")"	{ return yytext[0]; }

 /* comparison ops */
">"	{ yylval.fn = 1; return CMP; }
"<"	{ yylval.fn = 2; return CMP; }
"!="	{ yylval.fn = 3; return CMP; }
"=="	{ yylval.fn = 4; return CMP; }
">="	{ yylval.fn = 5; return CMP; }
"<="	{ yylval.fn = 6; return CMP; }

 /* keywords */
"if"	      { return IF; }
"else"	   { return ELSE; }
"while"	   { return WHILE; }
"print"	   { return PRINT; }
"printf"    { return PRINTF; }

[a-zA-Z]+("_"?[a-zA-Z0-9]*)*	{ yylval.s = newrecord(yytext); return NAME; }
"-"?[0-9]+"."?[0-9]*	      { yylval.d = atof(yytext); return NUMBER; }

 /* ignored text */
"//".*"//"	/* comments */
[ \t]		/* white space */
"\n"

 /* if there was no token found */
.	{ yyerror("Mystery character %c\n", *yytext); }
%%
