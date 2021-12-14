/**
 * Lexer para uma calculadora avancada
 */

/* reconhecimento de tokens para a calculadora */

%option noyywrap nodefault yylineno
%{
    #include "calculator.h"
    #include "calculator.tab.h"
%}

/* expoente float */
EXP ([Ee][-+]?[0-9]+)

%%
"+" | /* operadores de caracter unico */
"-" | 
"*" |
"/" |
"=" |
"," |
";" |
"(" |
")" { return yytext[0]; } /* operadores de comparacao, todos sao token CMP`*/
">" { yylval.fn = 1; return CMP; }
"<" { yylval.fn = 2; return CMP; }
"<>" { yylval.fn = 3; return CMP; }
"==" { yylval.fn = 4; return CMP; }
">=" { yylval.fn = 5; return CMP; }
"<=" { yylval.fn = 6; return CMP; }

"&&" { yylval.fn = 7; return CMP; } /* ADDED */
"||" { yylval.fn = 8; return CMP; }

"if" { return IF; } /* palavras-chave */
"then" { return THEN; }
"else" { return ELSE; }
"while" { return WHILE; }
"do" { return DO; }
"let" { return LET; }

"for" { return FOR; } /* ADDED */

"sqrt" { yylval.fn = B_sqrt; return FUNC; } /* funcoes pre-definidas */
"exp" { yylval.fn = B_exp; return FUNC; }
"log" { yylval.fn = B_log; return FUNC; }
"print" { yylval.fn = B_print; return FUNC; }

[a-zA-Z][a-zA-Z0-9]* { yylval.s = lookup(yytext); return NAME; } /* nomes */

[0-9]+"."[0-9]*{EXP}? |
"."?[0-9]+{EXP}? { yylval.d = atof(yytext); return NUMBER; }

"//".*
[ \t] /* ignora espaco em branco */

\\\n { printf("c> "); } /* ignora continuacao de linha */

\n { return EOL; }

. { yyerror("Caracter desconhecido %c\n", *yytext); }
%%