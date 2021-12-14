%{
    #include "tex-to-md.h"
%}

%union {
    ASTNode *a;
    char *string;
};

%token <string> NOME CONTEUDO ITEM
%token CLASSE PACOTE AUTOR TITULO
%token DOCUMENT _BEGIN _END CHAPTER SECTION SUBSECTION
%token PAR BF UNDERLINE IT ENUMERATE ITEMIZE NUMBER NEWLINE

%type <a> documentoLatex identificacao configuracao class package main begin end corpoLista chapter section subsection corpo text textStyle lists numberedList itemList itens

%%

documentoLatex: configuracao identificacao main { $$ = newAST(NT_DOCUMENT, $1, $2, $3, NULL); evalAST($$); }
| configuracao main { $$ = newAST(NT_DOCUMENT, $1, $2, NULL, NULL); evalAST($$); }
;

configuracao: class package { $$ = newAST(NT_SETTINGS, $1, $2, NULL, NULL); }
| class { $$ = newAST(NT_SETTINGS, $1, NULL, NULL, NULL); }
;

identificacao: TITULO CONTEUDO AUTOR CONTEUDO { $$ = newIdentification(NT_IDENTIFICATION, $2, $4); } 
| TITULO NOME AUTOR CONTEUDO { $$ = newIdentification(NT_IDENTIFICATION, $2, $4); }
| TITULO CONTEUDO AUTOR NOME { $$ = newIdentification(NT_IDENTIFICATION, $2, $4); }
| TITULO NOME AUTOR NOME { $$ = newIdentification(NT_IDENTIFICATION, $2, $4); }
| TITULO CONTEUDO { $$ = newIdentification(NT_IDENTIFICATION, $2, NULL); } 
| TITULO NOME { $$ = newIdentification(NT_IDENTIFICATION, $2, NULL); }
;

begin: _BEGIN DOCUMENT { $$ = newAST(NT_BEGIN, NULL, NULL, NULL, NULL); }
;

end: _END DOCUMENT { $$ = newAST(NT_END, NULL, NULL, NULL, NULL); }
;

package: PACOTE NOME { $$ = newPackage(NT_PACKAGE, $2, NULL, NULL); }
| PACOTE NOME NOME { $$ = newPackage(NT_PACKAGE, $2, $3, NULL); } 
| PACOTE NOME package { $$ = newPackage(NT_PACKAGE, $2, NULL, $3); }
| PACOTE NOME NOME package { $$ = newPackage(NT_PACKAGE, $2, $3, $4); }
;

main: begin end { $$ = newAST(NT_MAIN, $1, $2, NULL, NULL); }
| begin corpoLista end { $$ = newAST(NT_MAIN, $1, $3, $2, NULL); }
;

corpoLista: chapter corpoLista { $$ = newAST(NT_BODYLIST, $1, $2, NULL, NULL); }
| chapter { $$ = newAST(NT_BODYLIST, $1, NULL, NULL, NULL); }
| section corpoLista { $$ = newAST(NT_BODYLIST, $1, $2, NULL, NULL); }
| section { $$ = newAST(NT_BODYLIST, $1, NULL, NULL, NULL); }
| subsection corpoLista { $$ = newAST(NT_BODYLIST, $1, $2, NULL, NULL); }
| subsection { $$ = newAST(NT_BODYLIST, $1, NULL, NULL, NULL); }
| corpo corpoLista { $$ = newAST(NT_BODYLIST, $1, $2, NULL, NULL); }
| corpo { $$ = newAST(NT_BODYLIST, $1, NULL, NULL, NULL); }
;

chapter: CHAPTER CONTEUDO { $$ = newTextSubdivision(NT_CHAPTER, $2, NULL, NULL); }
| CHAPTER NOME { $$ = newTextSubdivision(NT_CHAPTER, $2, NULL, NULL); }
;

class: CLASSE NOME NOME { $$ = newClass(NT_CLASS, $2, $3); }
;

section: SECTION CONTEUDO { $$ = newTextSubdivision(NT_SECTION, $2, NULL, NULL); }
| SECTION NOME { $$ = newTextSubdivision(NT_SECTION, $2, NULL, NULL); }
;

subsection: SUBSECTION CONTEUDO { $$ = newTextSubdivision(NT_SUBSECTION, $2, NULL, NULL); } 
| SUBSECTION NOME { $$ = newTextSubdivision(NT_SUBSECTION, $2, NULL, NULL);} 
;

corpo: text { $$ = newAST(NT_BODY, $1, NULL, NULL, NULL); } 
| textStyle { $$ = newAST(NT_BODY, $1, NULL, NULL, NULL); } 
| lists { $$ = newAST(NT_BODY, $1, NULL, NULL, NULL); }
;

text: NOME text { $$ = newText(NT_TEXT, $1, $2); }
| NOME PAR { $$ = newText(NT_TEXT, $1, NULL); }
| NOME NEWLINE { $$ = newText(NT_TEXT, $1, NULL); }
| NOME { $$ = newText(NT_TEXT, $1, NULL); }
| PAR { $$ = newText(NT_TEXT, " ", NULL); }
| NEWLINE { $$ = newText(NT_TEXT, "\n", NULL); }
;

textStyle: BF NOME { $$ = newTextStyle(NT_TEXTSTYLE, $2, TS_BOLD); } 
| BF CONTEUDO { $$ = newTextStyle(NT_TEXTSTYLE, $2, TS_BOLD); } 
| UNDERLINE NOME { $$ = newTextStyle(NT_TEXTSTYLE, $2, TS_UNDERLINE); } 
| UNDERLINE CONTEUDO { $$ = newTextStyle(NT_TEXTSTYLE, $2, TS_UNDERLINE); } 
| IT NOME { $$ = newTextStyle(NT_TEXTSTYLE, $2, TS_ITALIC); }
| IT CONTEUDO { $$ = newTextStyle(NT_TEXTSTYLE, $2, TS_ITALIC); }
;

lists: numberedList { $$ = newAST(NT_LIST, $1, NULL, NULL, NULL); } 
| itemList { $$ = newAST(NT_LIST, $1, NULL, NULL, NULL); }
;

numberedList: _BEGIN ENUMERATE itens _END ENUMERATE { $$ = newAST(NT_NUMBEREDLIST, $3, NULL, NULL, NULL); }
;

itemList: _BEGIN ITEMIZE itens _END ITEMIZE { $$ = newAST(NT_ITEMLIST, $3, NULL, NULL, NULL); }
;

itens: ITEM { $$ = newItems(NT_ITENS, $1, NULL); } 
| ITEM itens { $$ = newItems(NT_ITENS, $1, $2); }
;
