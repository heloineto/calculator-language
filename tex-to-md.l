%option noyywrap nodefault yylineno
%{
    #include "tex-to-md.h"    
    #include "tex-to-md.tab.h"    
%}

letter  [a-zA-Z]
digit   [0-9]
space   " "
special_chars [\.@\-"<"">""("")"","\''"!""?"";"":"]
characters {digit}|{letter}|{special_chars}

%%

"%".* /* Ignore comments */

\\author { return AUTOR; } 
\\begin { return _BEGIN; }
\\bf|\\textbf { return BF; }
\\chapter { return CHAPTER; }
\\documentclass { return CLASSE; }
\\end { return _END; }
\\it|\\textit { return IT; }
\\par { return PAR;}
\\section { return SECTION; }
\\subsection { return SUBSECTION; }
\\title { return TITULO; }
\\underline { return UNDERLINE; }
\\usepackage { return PACOTE; }
\\newline|\\\\ { return NEWLINE; }

"{"document"}" { return DOCUMENT; }
"{"enumerate"}" { return ENUMERATE; }
"{"itemize"}" { return ITEMIZE; }

\\item" ".* {
    copyStr(&yylval.string, &yytext[6], false);
    return ITEM;
}

("{"|"[")(["\\"\n]|{characters})+("}"|"]") {
    copyStr(&yylval.string, yytext, true);
    return NOME;
}

"{"([" ""\\"\n]|{characters})+"}" {
    copyStr(&yylval.string, yytext, true);
    return CONTEUDO;
}

{characters}+ {
    copyStr(&yylval.string, yytext, false);
    return NOME;
}

\t|\n|{space}|\\maketitle {}

. { printf("Unknown character %c at line %d \n", *yytext, yylineno); }

%%