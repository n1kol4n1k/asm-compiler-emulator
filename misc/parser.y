%{
	#include <stdio.h>
  #include <iostream>
  #include "./inc/asmManager.h"

	int yylex(void);
	void yyerror(const char*);

  extern "C" FILE* yyin;

  #define YYDEBUG 1

  bool isLine = false;
%}

%output "parser.c"
%defines "parser.h"

%define parse.trace

%union
{
  int num;
  char *ident;
}

%token T_NEWLINE
%token T_COMMA
%token T_COMMENT
%token T_COLON
%token T_DIR_GLOB
%token T_DIR_EXT
%token T_DIR_SEC
%token T_DIR_WORD
%token T_DIR_SKIP
%token T_DIR_END

%token <num> T_NUM
%token <ident> T_IDENT

%start program

%%
program 
  : 
  | line_compl
  | line_compl T_NEWLINE program
  ;
line_compl
  : line
  | line T_COMMENT
  | T_COMMENT
  | T_IDENT T_COLON line
  { ASMManager.ProcessSameLineLabel($1); }
  | T_IDENT T_COLON
  { ASMManager.ProcessLabel($1);}
  ;
line
  : T_DIR_GLOB sym_list
  { ASMManager.ProcessGlobal(); }
  | T_DIR_EXT sym_list
  { ASMManager.ProcessExtern(); }
  | T_DIR_SEC T_IDENT
  { ASMManager.ProcessSection($2); }
  | T_DIR_WORD sym_lit_list
  { ASMManager.ProcessWord(); }
  | T_DIR_SKIP T_NUM
  { ASMManager.ProcessSkip($2); }
  | T_DIR_END
  { ASMManager.ProcessEnd(); YYACCEPT; }
  ;
sym_list
  : T_IDENT
  { ASMManager.RegisterSymArg($1); }
  | T_IDENT T_COMMA sym_list
  { ASMManager.RegisterSymArg($1); }
  ;
sym_lit_list
  : T_IDENT
  { ASMManager.RegisterSymArg($1); }
  | T_NUM
  { ASMManager.RegisterNumArg($1); }
  | T_IDENT T_COMMA sym_lit_list
  { ASMManager.RegisterSymArg($1); }
  | T_NUM T_COMMA sym_lit_list
  { ASMManager.RegisterNumArg($1); }
%%