#include "tex-to-md.h"

int main(int argc, char** argv) {
  if (argc != 2) {
    printf("1 argument expected, got %d\n", argc);
    return 1;
  }

  yyin = fopen(argv[1], "r");

  if (!yyin) {
    printf("Error: failed to read file %s\n", argv[1]);
    return 1;
  }

  outFilePtr = getFilePtr(argv[1]);

  currChapter = 0;
  currSection = 1;
  currSubSection = 1;

  return yyparse();
}

//* AST (Abstract Syntax Tree) *//
void evalAST(ASTNode* a) {
  if (!a) return;

  switch (a->nodeType) {
  case NT_DOCUMENT:
    evalAST(a->n1);
    freeAST(a->n1);
    evalAST(a->n2);
    freeAST(a->n2);
    evalAST(a->n3);
    freeAST(a->n3);
    break;
  case NT_SETTINGS:
    evalAST(a->n1);
    evalAST(a->n2);
    break;
  case NT_CLASS:
    fputs("[//]: # \"", outFilePtr);

    Class* class = (Class*)a;

    fputs("\\documentclass[", outFilePtr);
    fputs(class->content1, outFilePtr);
    fputs("]", outFilePtr);

    fputs("{", outFilePtr);
    fputs(class->content2, outFilePtr);
    fputs("}", outFilePtr);

    fputs("}\"", outFilePtr);

    fputs("\n\n", outFilePtr);

    break;
  case NT_PACKAGE:
    fputs("[//]: # \"\n", outFilePtr);

    Package* package = (Package*)a;

    while (package != NULL) {

      fputs("\\package[", outFilePtr);
      fputs(package->content1, outFilePtr);
      fputs("]", outFilePtr);

      if (package->content2) {
        fputs("{", outFilePtr);
        fputs(package->content2, outFilePtr);
        fputs("}", outFilePtr);
      }
      fputs("\n\n", outFilePtr);

      package = (Package*)package->next;
    }

    fputs("}\"", outFilePtr);

    fputs("\n\n", outFilePtr);

    break;

  case NT_IDENTIFICATION:
    fputs("# ", outFilePtr);

    Identification* id = (Identification*)a;

    fputs(id->title, outFilePtr);
    fputs("\n\n", outFilePtr);

    if (id->author) {
      fputs(id->author, outFilePtr);
      fputs("\n\n", outFilePtr);
      fputs("\n\n", outFilePtr);
    }

    break;

  case NT_MAIN:
    evalAST(a->n3);
    break;

  case NT_BODYLIST:
    evalAST(a->n1);
    if (a->n2)
      evalAST(a->n2);

    break;

  case NT_CHAPTER:
    currSection = 1;
    currSubSection = 1;
    struct TextSubdivision* chap = (struct TextSubdivision*)a;

    fputs("## ", outFilePtr);
    fputs(chap->content, outFilePtr);

    fputs("\n\n", outFilePtr);

    currChapter++;

    break;
  case NT_SECTION:
    currSubSection = 1;

    struct TextSubdivision* sec = (struct TextSubdivision*)a;

    fputs("\n#### **", outFilePtr);
    fputs(numberToStr(currChapter), outFilePtr);
    fputs(".", outFilePtr);
    fputs(numberToStr(currSection), outFilePtr);
    fputs("\t", outFilePtr);
    fputs(sec->content, outFilePtr);
    fputs("**\n", outFilePtr);
    fputs("\n\n", outFilePtr);

    currSection++;

    break;
  case NT_SUBSECTION:
    fputs("\n##### **", outFilePtr);
    struct TextSubdivision* subsec = (struct TextSubdivision*)a;

    fputs(numberToStr(currChapter), outFilePtr);
    fputs(".", outFilePtr);
    fputs(numberToStr(currSection), outFilePtr);
    fputs(".", outFilePtr);
    fputs(numberToStr(currSubSection), outFilePtr);
    fputs("\t", outFilePtr);
    fputs(subsec->content, outFilePtr);
    fputs("**\n", outFilePtr);

    fputs("\n\n", outFilePtr);

    currSubSection++;

    break;
  case NT_BODY:
    evalAST(a->n1);
    break;

  case NT_TEXT:;
    Text* txt = (Text*)a;

    while (txt != NULL) {
      fputs(txt->content, outFilePtr);
      txt = (Text*)txt->next;
      fputs(" ", outFilePtr);
    }

    fputs("\n\n", outFilePtr);

    break;

  case NT_TEXTSTYLE:;
    struct StructTextStyle* txtst = (struct StructTextStyle*)a;

    switch (txtst->textStyle) {
    case TS_BOLD:
      fputs("**", outFilePtr);
      fputs(txtst->content, outFilePtr);
      fputs("**", outFilePtr);
      fputs(" ", outFilePtr);
      break;
    case TS_ITALIC:
      fputs("*", outFilePtr);
      fputs(txtst->content, outFilePtr);
      fputs("*", outFilePtr);
      fputs(" ", outFilePtr);
      break;
    case TS_UNDERLINE:
      fputs("<ins>", outFilePtr);
      fputs(txtst->content, outFilePtr);
      fputs("</ins>", outFilePtr);
      fputs(" ", outFilePtr);
      break;
    default:
      break;
    }

    break;

  case NT_LIST:
    evalAST(a->n1);
    break;

  case NT_NUMBEREDLIST:;
    Itens* nlist = ((Itens*)a->n1);

    while (nlist != NULL) {
      fputs("1. ", outFilePtr);
      fputs(nlist->content, outFilePtr);
      fputs("\n", outFilePtr);
      nlist = (Itens*)nlist->next;
    }

    fputs("\n\n", outFilePtr);
    break;

  case NT_ITEMLIST:;
    Itens* ilist = ((Itens*)a->n1);

    while (ilist != NULL) {
      fputs("* ", outFilePtr);
      fputs(ilist->content, outFilePtr);
      fputs("\n", outFilePtr);
      ilist = (Itens*)ilist->next;
    }

    fputs("\n\n", outFilePtr);
    break;

  case NT_ITENS:
    break;

  default:
    printf("Erro: Unknown node type on node %d\n", a->nodeType);
    break;
  }
}

void freeAST(ASTNode* a) {
  if (!a) return;

  switch (a->nodeType) {
  case NT_DOCUMENT:
    freeAST(a->n1);
    freeAST(a->n2);
    freeAST(a->n3);
    break;
  case NT_SETTINGS:
    freeAST(a->n1);
    freeAST(a->n2);
    break;
  case NT_CLASS:;
    Class* class = (Class*)a;

    if (class->content1) {
      free(class->content1);
      class->content1 = NULL;
    }
    if (class->content2) {
      free(class->content2);
      class->content2 = NULL;
    }

    break;
  case NT_PACKAGE:;
    Package* package = (Package*)a;

    if (package->content1) {
      free(package->content1);
      package->content1 = NULL;
    }
    if (package->content2) {
      free(package->content2);
      package->content2 = NULL;
    }

    evalAST((ASTNode*)package->next);
    package->next = NULL;

    break;
  case NT_IDENTIFICATION:;
    Identification* id = (Identification*)a;

    if (id->title) {
      free(id->title);
      id->title = NULL;
    }
    if (id->author) {
      free(id->author);
      id->author = NULL;
    }

    break;
  case NT_MAIN:
    freeAST(a->n1);
    freeAST(a->n2);
    freeAST(a->n3);
    break;
  case NT_BEGIN:
    break;
  case NT_END:
    break;
  case NT_BODYLIST:
    freeAST(a->n1);

    if (a->n2)
      freeAST(a->n2);

    break;
  case NT_CHAPTER:;
    struct TextSubdivision* chapter = (struct TextSubdivision*)a;

    if (chapter->content) {
      free(chapter->content);
      chapter->content = NULL;
    }

    if (chapter->n1) {
      freeAST(chapter->n1);
      freeAST(chapter->n2);
    }
    break;
  case NT_SUBSECTION:;
    struct TextSubdivision* subsection = (struct TextSubdivision*)a;

    if (subsection->content) {
      free(subsection->content);
      subsection->content = NULL;
    }

    freeAST(subsection->n1);

    if (subsection->n2) {
      freeAST(subsection->n2);
    }
    break;
  case NT_SECTION:;
    struct TextSubdivision* section = (struct TextSubdivision*)a;

    if (section->content) {
      free(section->content);
      section->content = NULL;
    }

    freeAST(section->n1);

    if (section->n2) {
      freeAST(section->n2);
    }
    break;
  case NT_BODY:
    freeAST(a->n1);
    freeAST(a->n2);
    break;
  case NT_TEXT:;
    Text* text = (Text*)a;

    if (text->content) {
      free(text->content);
      text->content = NULL;
    }

    freeAST((ASTNode*)text->next);
    break;
  case NT_TEXTSTYLE:;
    struct StructTextStyle* textStyle = (struct StructTextStyle*)a;

    if (textStyle->content) {
      free(textStyle->content);
      textStyle->content = NULL;
    }
    break;
  case NT_LIST:
    freeAST(a->n1);
    break;
  case NT_NUMBEREDLIST:
    freeAST(a->n1);
    break;
  case NT_ITEMLIST:
    freeAST(a->n1);
    break;
  case NT_ITENS:;
    Itens* itens = (Itens*)a;

    if (itens->content) {
      free(itens->content);
      itens->content = NULL;
    }

    if (itens->next) {
      freeAST(itens->next);
      itens->next = NULL;
    }
    break;
  default:
    break;
  }

  if (a) free(a);
}

ASTNode* newAST(NodeType nodeType, ASTNode* n1, ASTNode* n2, ASTNode* n3, ASTNode* n4) {
  ASTNode* a = malloc(sizeof(ASTNode));

  if (!a) {
    printf("Error: memory allocation returned NULL pointer\n");
    exit(0);
  }

  a->nodeType = nodeType;
  a->n1 = n1;
  a->n2 = n2;
  a->n3 = n3;
  a->n4 = n4;

  return a;
}

ASTNode* newClass(NodeType nodeType, char* content1, char* content2) {
  Class* a = malloc(sizeof(Class));

  if (!a) {
    printf("Error: memory allocation returned NULL pointer\n");
    exit(0);
  }

  a->nodeType = nodeType;
  a->content1 = content1;
  a->content2 = content2;

  return ((ASTNode*)a);
}

ASTNode* newIdentification(NodeType nodeType, char* title, char* author) {
  Identification* a = malloc(sizeof(Identification));

  if (!a) {
    printf("Error: memory allocation returned NULL pointer\n");
    exit(0);
  }

  a->nodeType = nodeType;
  a->title = title;
  a->author = author;

  return ((ASTNode*)a);
}

ASTNode* newItems(NodeType nodeType, char* content, ASTNode* next) {
  Itens* a = malloc(sizeof(Itens));

  if (!a) {
    printf("Error: memory allocation returned NULL pointer\n");
    exit(0);
  }

  a->nodeType = nodeType;
  a->content = content;
  a->next = next;

  return ((ASTNode*)a);
}

ASTNode* newPackage(NodeType nodeType, char* content1, char* content2, ASTNode* next) {
  Package* a = malloc(sizeof(Package));

  if (!a) {
    printf("Error: memory allocation returned NULL pointer\n");
    exit(0);
  }

  a->nodeType = nodeType;
  a->content1 = content1;
  a->content2 = content2;
  a->next = (Package*)next;

  return ((ASTNode*)a);
}

ASTNode* newText(NodeType nodeType, char* content, ASTNode* next) {
  Text* a = malloc(sizeof(Text));

  if (!a) {
    printf("Error: memory allocation returned NULL pointer\n");
    exit(0);
  }

  a->nodeType = nodeType;
  a->content = content;
  a->next = (Text*)next;

  return ((ASTNode*)a);
}

ASTNode* newTextStyle(NodeType nodeType, char* content, enum TextStyle textStyle) {
  struct StructTextStyle* a = malloc(sizeof(struct StructTextStyle));

  if (!a) {
    printf("Error: memory allocation returned NULL pointer\n");
    exit(0);
  }

  a->nodeType = nodeType;
  a->content = content;
  a->textStyle = textStyle;

  return ((ASTNode*)a);
}

ASTNode* newTextSubdivision(NodeType nodeType, char* content, ASTNode* n1, ASTNode* n2) {
  struct TextSubdivision* a = malloc(sizeof(struct TextSubdivision));

  if (!a) {
    printf("Error: memory allocation returned NULL pointer\n");
    exit(0);
  }

  a->nodeType = nodeType;
  a->content = content;
  a->n1 = n1;
  a->n2 = n2;

  return ((ASTNode*)a);
}

//* Utils *//
void copyStr(char** dest, char* src, bool removeBrackets) {
  const int n = removeBrackets ? strlen(src) - 2 : strlen(src);
  *dest = (char*)malloc((sizeof(char) * n) + 1);

  strncpy(*dest, &src[(removeBrackets ? 1 : 0)], n);
  (*dest)[n] = '\0';
}

char* numberToStr(long long int number) {
  char* str = (char*)malloc(sizeof(char) * (ceil(log10(number)) + 1));
  sprintf(str, "%llu", number);

  return str;
}

FILE* getFilePtr(char* inFileName) {
  outFileName = (char*)malloc(sizeof(char) * strlen(inFileName));

  /* Change extension (.tex -> .md) */
  strcpy(outFileName, inFileName);
  char* lastExt = strrchr(outFileName, '.');
  if (lastExt) *lastExt = '\0';
  strcat(outFileName, ".md");

  /* Clear file contents */
  outFilePtr = fopen(outFileName, "w");
  fclose(outFilePtr);

  return fopen(outFileName, "a");
}

//* Flex *//
void yyerror(char const* s) {
  fprintf(stderr, "%s\n", s);
}
