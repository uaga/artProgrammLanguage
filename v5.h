/*
 *	declarations for V5
 */

/* symbol table */
struct symbol{
   char *name;
   double value;
};

#define NHASH 100
struct symbol symtab[NHASH];
struct symbol *newrecord(char *name);
double retnum(struct symbol *name);

/*
 * node types
 * '+' '-' '*' '/'
 * 1-6 comparison ops
 * P print function
 * I IF statement
 * W WHILE statement
 * N symbol ref
 * C constant
 * '=' assignment
 * L statement list
 */

/* 
 * nodes in the Abstract Syntax Tree
 * all have common initial nodetype
 */
struct origin{
   struct ast *b;
};

struct ast{
   int nodetype;
   struct ast *l;
   struct ast *r;
};

struct symprint{  /* type 'P' */
   int nodetype;
   int t;
   struct symbol *s;
};

struct symasgn{   /* type '=' */
   int nodetype;
   struct symbol *n;
   struct ast *v;
};

struct symref{    /* type 'N' */
   int nodetype;
   struct symbol *s;
};

struct numval{    /* type 'C' */
   int nodetype;
   double value;
};

struct flow{      /* type 'I' */
   int nodetype;
   struct ast *cond; /* condition */
   struct ast *tl;   /* if try list */
   struct ast *el;   /* optional else list */
};

/* build an AST */
struct ast *newast(char nodetype, struct ast *l, struct ast *r);
void newbranch(struct ast *b);
struct ast *newasgn(struct symbol *n, struct ast *v);
struct ast *newref(struct symbol *s);
struct ast *newnum(double value);
struct ast *newprint(int t, struct symbol *s);
struct ast *newflow(int nodetype, struct ast *cond, struct ast *tl, struct ast *el);
struct ast *newcmp(int cmptype, struct ast *l, struct ast *r);

/* evaluate an AST for orig */
void eval_orig();

/* evaluate an AST for expr */
double eval_expr(struct ast *);

/* delete and free an AST */
void treefree(struct ast *);

/* interface to the lexer */
extern int yylineno;
void yyerror(char *s, ...);