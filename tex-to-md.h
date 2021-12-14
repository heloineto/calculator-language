#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

typedef enum NodeType {
    NT_DOCUMENT, NT_SETTINGS, NT_CLASS, NT_PACKAGE, NT_IDENTIFICATION,
    NT_MAIN, NT_BEGIN, NT_END, NT_BODYLIST, NT_CHAPTER, NT_SUBSECTION,
    NT_SECTION, NT_BODY, NT_TEXT, NT_TEXTSTYLE, NT_LIST, NT_NUMBEREDLIST,
    NT_ITEMLIST, NT_ITENS
} NodeType;

enum TextStyle { TS_BOLD, TS_ITALIC, TS_UNDERLINE };

typedef struct ASTNode {
    NodeType nodeType;
    struct ASTNode* n1;
    struct ASTNode* n2;
    struct ASTNode* n3;
    struct ASTNode* n4;
} ASTNode;

typedef struct Class {
    NodeType nodeType;
    char* content1;
    char* content2;
} Class;

typedef struct Identification {
    NodeType nodeType;
    char* title;
    char* author;
} Identification;

typedef struct Itens {
    NodeType nodeType;
    char* content;
    ASTNode* next;
} Itens;

typedef struct Package {
    NodeType nodeType;
    char* content1;
    char* content2;
    struct Package* next;
} Package;

typedef struct Text {
    NodeType nodeType;
    char* content;
    struct Text* next;
} Text;

typedef struct StructTextStyle {
    NodeType nodeType;
    char* content;
    enum TextStyle textStyle;
} TextStyle;

typedef struct TextSubdivision {
    NodeType nodeType;
    char* content;
    ASTNode* n1;
    ASTNode* n2;
} TextSubdivision;

//* AST (Abstract Syntax Tree) *//
void evalAST(ASTNode*);
void freeAST(ASTNode*);
ASTNode* newAST(NodeType nodeType, ASTNode* n1, ASTNode* n2, ASTNode* n3, ASTNode* n4);
ASTNode* newClass(NodeType nodeType, char* content1, char* content2);
ASTNode* newIdentification(NodeType nodeType, char* n1, char* n2);
ASTNode* newItems(NodeType nodeType, char* content, ASTNode* next);
ASTNode* newPackage(NodeType nodeType, char* content1, char* content2, ASTNode* next);
ASTNode* newText(NodeType nodeType, char* content, ASTNode* next);
ASTNode* newTextStyle(NodeType nodeType, char* content, enum TextStyle textStyle);
ASTNode* newTextSubdivision(NodeType nodeType, char* content, ASTNode* n1, ASTNode* n2);

//* Flex *//
int yylineno;
FILE* yyin;
int yylex();
void yyerror(const char* s);
int yyparse();

//* Utils *//
void copyStr(char** dest, char* src, bool removeBrackets);
char* numberToStr(long long int value);
FILE* getFilePtr(char* inFileName);
char* outFileName;
FILE* outFilePtr;
int currChapter;
int currSection;
int currSubSection;