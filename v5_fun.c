/*
 *	helper function for V5
 */

#   include <stdio.h>
#   include <stdlib.h>
#   include <string.h>
#   include <stdarg.h>
#   include "v5.h"

#   define FILE_IN "code.art"
#   define FILE_OUT "OUT"
#   define FILE_DATA "IN"
#   define NBRANCH 100

extern FILE *yyin;
FILE *data;

/* time in system */
char time[20];

/* symbol table */
int size_tab = 0;

/* record as programm */
struct symbol *
newrecord(char *name){
   struct symbol *sp = &symtab[0];
   int scount = 0;
   while(scount < NHASH){
      if(sp->name && !strcmp(sp->name, name)) { return sp; }
      if(!sp->name){
         sp->name = strdup(name);
         sp->value = -1;
         size_tab++;
         return sp;
      }
      if(++sp >= symtab+NHASH) { sp = symtab; }
   }
}

/* record is data file */
void
addrecord(char *name, double value){
   struct symbol *sp = &symtab[0];
   int scount = 0;
   while(scount < NHASH){
      if(sp->name && !strcmp(sp->name, name)) { sp->value = value; return; }
      if(!sp->name){
         sp->name = strdup(name);
         sp->value = value;
         size_tab++;
         return;
      }
      if(++sp >= symtab+NHASH) { sp = symtab; }
   }
}

void
tabprint(){
   printf("---------------------------------\n");
   for(int i=0; i<size_tab; i++){
      printf("%d|\t%s = %.3f\t\t|\n", i, symtab[i].name, (symtab[i]).value);
   }
   printf("---------------------------------\n");
}

double
retnum(struct symbol *name){
   if(size_tab == 0) { yyerror("table is empty | retnum"); return 0.0; }
   return name->value;
}

struct ast *
newast(char nodetype, struct ast *l, struct ast *r){
   struct ast *a = malloc(sizeof(struct ast));

   if(!a) { yyerror("out of space | newast"); exit(0); }

   a->nodetype = nodetype;
   a->l = l;
   a->r = r;
   return a;
}

int count_branch = 0;
struct origin orig[NBRANCH];

void
newbranch(struct ast *b){
   orig[count_branch].b = malloc(sizeof(struct ast *));
   orig[count_branch].b = b;
   count_branch++;
}

struct ast *
newasgn(struct symbol *n, struct ast *v){
   struct symasgn *a = malloc(sizeof(struct symasgn));

   if(!a) { yyerror("out of space | newasgn"); exit(0); }

   a->nodetype = '=';
   a->n = n;
   a->v = v;
   return (struct ast *)a;
}

struct ast *
newref(struct symbol *s){
   struct symref *a = malloc(sizeof(struct symref));

   if(!a) { yyerror("out of space | newref"); exit(0); }

   a->nodetype = 'N';
   a->s = s;
   return (struct ast *)a;
}

struct ast *
newnum(double value){
   struct numval *a = malloc(sizeof(struct numval));

   if(!a) { yyerror("out of space | newnum"); exit(0); }

   a->nodetype = 'C';
   a->value = value;
   return (struct ast *)a;
}

struct ast  *
newprint(int t, struct symbol *s){
   struct symprint *a = malloc(sizeof(struct symprint));

   if(!a) { yyerror("out of space | newprint"); exit(0); }

   a->nodetype = 'P';
   a->t = t;
   a->s = s;
   return (struct ast *)a;
}

struct ast *
newflow(int nodetype, struct ast *cond, struct ast *tl, struct ast *el){
   struct flow *a = malloc(sizeof(struct flow));

   if(!a) { yyerror("out of space | newflow"); exit(0); }

   a->nodetype = nodetype;
   a->cond = cond;
   a->tl = tl; 
   a->el = el;

   return (struct ast *)a ;
}

struct ast *
newcmp(int cmptype, struct ast *l, struct ast *r){
   struct ast *a = malloc(sizeof(struct ast));

   if(!a) { yyerror("out of space | newcmp"); exit(0); }

   a->nodetype = '0' + cmptype;
   a->l = l;
   a->r = r;
   return a;
}

void
eval(struct ast *a){
   if(!a) { yyerror("internal error, null eval | eval"); }
   switch(a->nodetype){
      case 'P':   
         if( ((struct symprint *)a)->t == 0 ) { printf("%s ", time); }
         printf("%s = %.3f\n", ((struct symprint *)a)->s->name, ((struct symprint *)a)->s->value); break;
      case '=':   ((struct symasgn *)a)->n->value = eval_expr(((struct symasgn *)a)->v); /*tabprint();*/ break;
      case 'L':   eval(a->l); if(a->r != NULL) { eval(a->r); } break;
      case 'I':   
         if( eval_expr( ((struct flow *)a)->cond ) == 1 ) { eval( ((struct flow *)a)->tl ); }
         else { if( ((struct flow *)a)->el ) {eval( ((struct flow *)a)->el );} } 
         break;
      case 'W':
         while( eval_expr( ((struct flow *)a)->cond ) != 0 ) { eval( ((struct flow *)a)->tl ); } break;
   }
}

void
eval_orig(){
   for(int i=0; i<count_branch; i++){
      eval(orig[i].b);
   }
}

double
eval_expr(struct ast *a){
   double v ;
   
   if(!a) { yyerror("internal error, null eval | eval_expr"); return 0.0; }
   switch(a->nodetype){
      /* name referense */
      case 'N':   v = retnum(((struct symref *)a)->s); break;

      /* constant */
      case 'C':   v = ((struct numval *)a)->value; break;
      
      /* expressions */
      case '+':   v = eval_expr(a->l) + eval_expr(a->r); break;
      case '-':   v = eval_expr(a->l) - eval_expr(a->r); break;
      case '*':   v = eval_expr(a->l) * eval_expr(a->r); break;
      case '/':   v = eval_expr(a->l) / eval_expr(a->r); break;

      /* comparisons */
      case '&':   v = ( (eval_expr(a->l) == 1.0) && (eval_expr(a->r) == 1.0) )? 1 : 0; break;
      case '|':   v = ( (eval_expr(a->l) == 1.0) || (eval_expr(a->r) == 1.0) )? 1 : 0; break;
      case '!':   v = ( eval_expr(a->l) == 0 )? 1 : 0; break;
      case '1':   v = (eval_expr(a->l) > eval_expr(a->r))? 1 : 0; break;
      case '2':   v = (eval_expr(a->l) < eval_expr(a->r))? 1 : 0; break;
      case '3':   v = (eval_expr(a->l) != eval_expr(a->r))? 1 : 0; break;
      case '4':   v = (eval_expr(a->l) == eval_expr(a->r))? 1 : 0; break;
      case '5':   v = (eval_expr(a->l) >= eval_expr(a->r))? 1 : 0; break;
      case '6':   v = (eval_expr(a->l) <= eval_expr(a->r))? 1 : 0; break;

      default: printf("internal error: bad node - '%c'\n", a->nodetype);
   }
   return v;
}

void
get_data(){
   char *name;
   double value;
   char str[64];
   char *str_split = malloc(sizeof(str));
   char *end = "end start value\n";
   char *spl_2 = " ";
   char *estr;
   estr = fgets(str, sizeof(str), data);
   while( strcmp(str, end) ){
      str_split = strtok(str, spl_2);
      name = str_split;
      str_split = strtok(NULL, spl_2);
      value = atof(str_split);
      addrecord(name, value);
      estr = fgets(str, sizeof(str), data);
   }
}

void
add_data(){
   char *name;
   double value;
   char str[64];
   char *str_split = malloc(sizeof(str));
   char *spl_1 = "|";
   char *spl_2 = " ";
   char *estr;
   estr = fgets(str, sizeof(str), data);
   str_split = strtok(str, spl_1);
   strcpy(time, str_split);
   str_split = strtok(NULL, spl_1);
   str_split = strtok(str_split, spl_2);
   name = str_split;
   str_split = strtok(NULL, spl_2);
   value = atof(str_split);
   addrecord(name, value);
}

int
main(){
   data = fopen(FILE_DATA, "r");
   yyin = fopen(FILE_IN, "r");
   FILE *file_out = freopen(FILE_OUT, "w+", stdout);
   yyparse();
   get_data();
   do{
      add_data();
      eval_orig();
   }while( feof(data) == 0 );
   return 0;
}

void
yyerror(char *s, ...){
   va_list ap;
   va_start(ap, s);

   fprintf(stderr, "in line %d error: ", yylineno);
   vfprintf(stderr, s, ap);
   fprintf(stderr, "\n");   
}