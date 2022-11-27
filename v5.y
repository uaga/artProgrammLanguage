/*
 *	the parser for V5
 */

%{
#   include <stdio.h>
#   include <stdlib.h>
#   include "v5.h"
%}

%union{
   struct ast *a;
   double d;
   struct symbol *s;
   int fn;
}

/* decler tokens */
%token <d> NUMBER
%token <s> NAME

%token IF ELSE WHILE PRINT PRINTF

%nonassoc <fn> CMP
%right '='
%left '+' '-'
%left '*' '/'

%type <a> stmt exp list col der minus multi name num plus ref print if if_else while cmp printf and or not

%start commands

%%
stmt: if
   | if_else
   | while
   | print
   | printf
   | exp

exp: and
   | or
   | not
   | cmp
   | plus
   | minus
   | multi
   | der
   | col
   | ref
   | num
   | name
;

list: /* nothing */  { $$ = NULL; }
   | stmt ';' list   { $$ = newast('L', $1, $3); }
;

if:      IF '(' exp ')' '{' list '}'                     { $$ = newflow('I', $3, $6, NULL); };
if_else: IF '(' exp ')' '{' list '}' ELSE '{' list '}'   { $$ = newflow('I', $3, $6, $10); };
while:   WHILE '(' exp ')' '{' list '}'                  { $$ = newflow('W', $3, $6, NULL); };
and:     exp '&' exp                   { $$ = newast('&', $1, $3); };
or:      exp '|' exp                   { $$ = newast('|', $1, $3); };
not:     '!' exp                       { $$ = newast('!', $2, NULL); };
cmp:     exp CMP exp                   { $$ = newcmp($2, $1, $3); };
plus:    exp '+' exp                   { $$ = newast('+', $1, $3); };
minus:   exp '-' exp                   { $$ = newast('-', $1, $3); };
multi:   exp '*' exp                   { $$ = newast('*', $1, $3); };
der:     exp '/' exp                   { $$ = newast('/', $1, $3); };
col:     '(' exp ')'                   { $$ = $2; };
ref:     NAME                          { $$ = newref($1); };
num:     NUMBER                        { $$ = newnum($1); };
print:   PRINT '(' NAME ')'            { $$ = newprint(1, $3); };
printf:  PRINTF '(' NAME ')'           { $$ = newprint(0, $3); };
name:    NAME '=' exp                  { $$ = newasgn($1, $3); };

command: /* nothing */
   | command stmt ';'	{ newbranch($2); }
;

commands: command;
%%