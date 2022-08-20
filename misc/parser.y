%{
	#include <stdio.h>
  #include <iostream>
  #include "./inc/asmManager.h"

	int yylex(void);
	void yyerror(const char*);

  extern "C" FILE* yyin;

  #define YYDEBUG 1
%}

%output "parser.c"
%defines "parser.h"

%define parse.trace

%code requires {
  typedef struct operandStruct{
    char* symbol;
    int literal;
    char* reg;
    int type;
  } operandInfo;
}

%union
{
  int num;
  char* ident;
  operandInfo opInfo;
}

%token T_NEWLINE
%token T_COMMA
%token T_COMMENT
%token T_COLON
%token T_DOLLAR
%token T_PERCENT
%token T_ASTERISK
%token T_SQ_BRACKET_L
%token T_SQ_BRACKET_R
%token T_PLUS

%token T_DIR_GLOB
%token T_DIR_EXT
%token T_DIR_SEC
%token T_DIR_WORD
%token T_DIR_SKIP
%token T_DIR_END

%token T_CMD_HALT
%token T_CMD_INT
%token T_CMD_IRET
%token T_CMD_CALL
%token T_CMD_RET
%token T_CMD_JMP
%token T_CMD_JEQ
%token T_CMD_JNE
%token T_CMD_JGT
%token T_CMD_PUSH
%token T_CMD_POP
%token T_CMD_XCHG
%token T_CMD_ADD
%token T_CMD_SUB
%token T_CMD_MUL
%token T_CMD_DIV
%token T_CMD_CMP
%token T_CMD_NOT
%token T_CMD_AND
%token T_CMD_OR
%token T_CMD_XOR
%token T_CMD_TEST
%token T_CMD_SHL
%token T_CMD_SHR
%token T_CMD_LDR
%token T_CMD_STR

%token <num> T_NUM
%token <ident> T_IDENT
%token <ident> T_REG

%type <opInfo> operand

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
  | T_CMD_HALT
  { ASMManager.ProcessEnd(); }
  | T_CMD_INT T_REG
  { ASMManager.ProcessEnd(); }
  | T_CMD_IRET
  { ASMManager.ProcessEnd(); }
  | T_CMD_CALL operand
  { ASMManager.ProcessEnd(); }
  | T_CMD_RET
  { ASMManager.ProcessEnd(); }
  | T_CMD_JMP operand
  { ASMManager.ProcessEnd(); }
  | T_CMD_JEQ operand
  { ASMManager.ProcessEnd(); }
  | T_CMD_JNE operand
  { ASMManager.ProcessEnd(); }
  | T_CMD_JGT operand
  { ASMManager.ProcessEnd(); }
  | T_CMD_PUSH T_REG
  { ASMManager.ProcessEnd(); }
  | T_CMD_POP T_REG
  { ASMManager.ProcessEnd(); }
  | T_CMD_XCHG T_REG T_COMMA T_REG
  { ASMManager.ProcessEnd(); }
  | T_CMD_ADD T_REG T_COMMA T_REG
  { ASMManager.ProcessEnd(); }
  | T_CMD_SUB T_REG T_COMMA T_REG
  { ASMManager.ProcessEnd(); }
  | T_CMD_MUL T_REG T_COMMA T_REG
  { ASMManager.ProcessEnd(); }
  | T_CMD_DIV T_REG T_COMMA T_REG
  { ASMManager.ProcessEnd(); }
  | T_CMD_CMP T_REG T_COMMA T_REG
  { ASMManager.ProcessEnd(); }
  | T_CMD_NOT T_REG
  { ASMManager.ProcessEnd(); }
  | T_CMD_AND T_REG T_COMMA T_REG
  { ASMManager.ProcessEnd(); }
  | T_CMD_OR T_REG T_COMMA T_REG
  { ASMManager.ProcessEnd(); }
  | T_CMD_XOR T_REG T_COMMA T_REG
  { ASMManager.ProcessEnd(); }
  | T_CMD_TEST T_REG T_COMMA T_REG
  { ASMManager.ProcessEnd(); }
  | T_CMD_SHL T_REG T_COMMA T_REG
  { ASMManager.ProcessEnd(); }
  | T_CMD_SHR T_REG T_COMMA T_REG
  { ASMManager.ProcessEnd(); }
  | T_CMD_LDR T_REG T_COMMA operand
  { ASMManager.ProcessEnd(); }
  | T_CMD_STR T_REG T_COMMA operand
  { ASMManager.ProcessEnd(); }
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
  ;
operand
  : T_NUM
  { $$.literal = $1; $$.type = OP_SYNTAX::noPrefixLiteral; }
  | T_IDENT
  { $$.symbol = $1; $$.type = OP_SYNTAX::noPrefixSymbol; }
  | T_DOLLAR T_NUM
  { $$.literal = $2; $$.type = OP_SYNTAX::dollarLiteral; }
  | T_DOLLAR T_IDENT
  { $$.symbol = $2; $$.type = OP_SYNTAX::dollarSymbol; }
  | T_PERCENT T_IDENT
  { $$.symbol = $2; $$.type = OP_SYNTAX::percentSymbol; }
  | T_REG
  { $$.reg = $1; $$.type = OP_SYNTAX::noPrefixReg; }
  | T_SQ_BRACKET_L T_REG T_SQ_BRACKET_R
  { $$.reg = $2; $$.type = OP_SYNTAX::bracketReg; }
  | T_SQ_BRACKET_L T_REG T_PLUS T_NUM T_SQ_BRACKET_R
  { $$.reg = $2; $$.literal = $4; $$.type = OP_SYNTAX::bracketRegLiteral; }
  | T_SQ_BRACKET_L T_REG T_PLUS T_IDENT T_SQ_BRACKET_R
  { $$.reg = $2; $$.symbol = $4; $$.type = OP_SYNTAX::bracketRegSymbol; }
  | T_ASTERISK T_NUM
  { $$.literal = $2; $$.type = OP_SYNTAX::asteriskLiteral; }
  | T_ASTERISK T_IDENT
  { $$.symbol = $2; $$.type = OP_SYNTAX::asteriskSymbol; }
  | T_ASTERISK T_REG
  { $$.reg = $2; $$.type = OP_SYNTAX::asteriskReg; }
  | T_ASTERISK T_SQ_BRACKET_L T_REG T_SQ_BRACKET_R
  { $$.reg = $3; $$.type = OP_SYNTAX::asteriskBracketReg; }
  | T_ASTERISK T_SQ_BRACKET_L T_REG T_PLUS T_NUM T_SQ_BRACKET_R
  { $$.reg = $3; $$.literal = $5; $$.type = OP_SYNTAX::asteriskBracketRegLiteral; }
  | T_ASTERISK T_SQ_BRACKET_L T_REG T_PLUS T_IDENT T_SQ_BRACKET_R
  { $$.reg = $3; $$.symbol = $5; $$.type = OP_SYNTAX::asteriskBracketRegSymbol; }
  ;
%%
    char* symbol;
    int literal;
    char* reg;
    int type; 