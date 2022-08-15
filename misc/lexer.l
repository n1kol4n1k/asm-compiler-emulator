%{
  #include "parser.h"
  #include <stdlib.h>
  #include <string.h>

  extern int yychar;

  bool isComment = false;
  extern bool isLine;
%}

%option noyywrap
%option yylineno
%option outfile="lexer.c" header-file="lexer.h"

%%

"#"[^\n]* { return T_COMMENT; }
\n { return T_NEWLINE; }
":" { return T_COLON; }
".global " { return T_DIR_GLOB; }
".extern " { return T_DIR_EXT; }
".section " { return T_DIR_SEC; }
".word " { return T_DIR_WORD; }
".skip " { return T_DIR_SKIP; }
".end" { return T_DIR_END; }
"," { return T_COMMA; }

[+-]?[1-9][0-9]*|0 { 
  yylval.num = atoi(yytext);
  return T_NUM; 
}
[a-zA-Z]+ {
  size_t len = strnlen(yytext, 32);
	char* buf = (char*)malloc(len + 1);
	strncpy(buf, yytext, len);
	buf[len] = '\0';
	yylval.ident = buf;
  return T_IDENT;
}

. ;

%%

int yyerror(const char* msg)
{
    printf("lexer error: %s line: %d val: %c\n", msg, yylineno, (char)yychar);
    return 0;
}