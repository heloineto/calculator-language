/* Declaracoes para uma calculadora avancada */
/* interface com o lexer */
extern int yylineno;
int yylex();
int yyparse();
void yyerror(char* s, ...);

/* tab. de simbolos */
struct symbol { /* um nome de variavel */
    char* name;
    double value;
    struct ast* func; /* stmt para funcao */
    struct symlist* syms; /* lista de argumentos */
};

/* tab. de simbolos de tamaho fixo */
#define NHASH 9997
struct symbol symtab[NHASH];


struct symbol* lookup(char*);

/* lista de simbolos, para uma lista de argumentos */
struct symlist {
    struct symbol* sym;
    struct symlist* next;
};


struct symlist* newsymlist(struct symbol* sym, struct symlist* next);
void symlistfree(struct symlist* sl);


/**
 * tipos de nos
 * + - * /
 * 0-7 operadores de comparacao , 04 igual , 02 menor que , 01 maior que
 * L expressao ou lista de comandos
 * I comando IF
 * W comando WHILE
 * N symbol de referencia
 * = atribuicao
 * S lista de simbolos
 * F chamada de funcao pre-definida
 * C chamada de funcao d e f . p/usuario
 *
 * O comando FOR
 */


enum bifs { /* funcoes pre-definidas */
    B_sqrt = 1,
    B_exp,
    B_log,
    B_print
};
/* nos na AST * /
/* todos tem o "nodetype” inicial em comum */

struct ast {
    int nodetype;
    struct ast* l;
    struct ast* r;
};

struct fncall { /* funcoes pre-definida */
    int nodetype; /* tipo F */
    struct ast* l;
    enum bifs functype;
};

struct ufncall { /* funcoes usuario */
    int nodetype; /* tipo C */
    struct ast* l; /* lista de argumentos */
    struct symbol* s;
};

struct flow {
    int nodetype; /* tipo I ou W */
    struct ast* cond; /* condicao */
    struct ast* tl; /* ramo ”then” ou lista ”do” */
    struct ast* el; /* ramo opcional ”else” */
};

struct forLoop {
    int nodetype; /* tipo O*/
    struct ast* init; /* representa a inicialização da variável de controle */
    struct ast* cond; /* condicao */
    struct ast* listAndInc; /* lista de comandos seguida do incremento */
};

struct numval {
    int nodetype; /* tipo K */
    double number;
};

struct symref {
    int nodetype; /* tipo N */
    struct symbol* s;
};

struct symasgn {
    int nodetype; /* tipo = */
    struct symbol* s;
    struct ast* v; /* valor a ser atribuido */
};

/* construcao de uma AST */

struct ast* newast(int nodetype, struct ast* l, struct ast* r);
struct ast* newcmp(int cmptype, struct ast* l, struct ast* r);
struct ast* newfunc(int functype, struct ast* l);
struct ast* newcall(struct symbol* s, struct ast* l);
struct ast* newref(struct symbol* s);
struct ast* newasgn(struct symbol* s, struct ast* v);
struct ast* newnum(double d);
struct ast* newflow(int nodetype, struct ast* cond, struct ast* tl, struct ast* tr);
struct ast* newfor(int nodetype, struct ast* init, struct ast* cond, struct ast* listAndInc);

/* definicao de uma funcao */
void dodef(struct symbol* name, struct symlist* syms, struct ast* stmts);

/* avaliacao de uma AST */
double eval(struct ast*);

/* deletar e liberar uma AST */
void treefree(struct ast*);