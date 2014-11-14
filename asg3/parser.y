%{
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "lyutils.h"
#include "astree.h"

#define YYDEBUG 1
#define YYERROR_VERBOSE 1
#define YYPRINT yyprint
#define YYMALLOC yycalloc

static void* yycalloc (size_t size);
%}

%debug
%defines
%error-verbose
%token-table
%verbose

/* %destructor { error_destructor ($$); } <> */

%token ROOT TOK_IDENT TOK_NUMBER TOK_EQEQ TOK_BRKKRB
%token TOK_NOTEQ TOK_LSTEQ TOK_GRTEQ TOK_VOID TOK_BOOL
%token TOK_CHAR TOK_INT TOK_STRING TOK_STRUCT TOK_IF
%token TOK_ELSE TOK_WHILE TOK_RETURN TOK_FALSE TOK_TRUE
%token TOK_NIL TOK_ORD TOK_CHR TOK_NEW TOK_CHARCONST
%token TOK_STRCONST TOK_BADIDENT TOK_BADSTRCONST TOK_BADCHARCONST
%token TOK_IFELSE TOK_BINOP TOK_TYPEID TOK_UNOP TOK_FIELD
%token TOK_FUNCTION TOK_DECLID TOK_RETURNVOID TOK_NEWSTRING
%token TOK_NEWARRAY TOK_CALL TOK_INDEX TOK_COMP

%right  '='
%left   TOK_EQEQ TOK_NOTEQ TOK_LSTEQ TOK_GRTEQ TOK_GRT TOK_LST
%left   '+' '-'
%left   '*' '/' '%'
%right  POS "u+" NEG "u-"

%start  program

%%
program   : start { yyparse_astree = $1; }
          ;

start     : structdef { $$ = $1; }
          | function  { $$ = $1; }
          | statement { $$ = $1; }
          |           { $$ = new_parseroot(); }
          ;

structdef : TOK_STRUCT TOK_IDENT '{' fielddecl ';' '}' { free_ast3 ($3, $5, $6); $$ = adopt2 ($1, $2, $4); }
          | TOK_STRUCT TOK_IDENT '{' '}' { free_ast2 ($3, $4); $$ = adopt1 ($1, $2); }
          ;

fielddecl : basetype TOK_BRKKRB TOK_IDENT { $$ = adopt2 ($2, $1, upd_tree_symbol($3, TOK_FIELD)); }
          | basetype TOK_IDENT { $$ = adopt1 ($1, upd_tree_symbol($2, TOK_FIELD)); }
          ;

function  : identdecl '(' identdecl ','  identdecl ')' block
                { free_ast3 ($2, $4, $6); $$ = adopt4 (new_custom_astree (TOK_FUNCTION, "FUNCTION", $1), $1, $3, $5, $6); }
          | identdecl '(' identdecl ')' block { free_ast2 ($2, $4); $$ = adopt3 (new_custom_astree (TOK_FUNCTION, "FUNCTION", $1), $1, $3, $5); }
          | identdecl '(' ')' block { free_ast2 ($2, $3); $$ = adopt2 (new_custom_astree (TOK_FUNCTION, "FUNCTION", $1), $1, $4); }
          ;

identdecl : basetype TOK_BRKKRB TOK_IDENT { $$ = adopt2 ($2, $1, upd_tree_symbol($3, TOK_DECLID)); }
          | basetype TOK_IDENT { $$ = adopt1 ($1, upd_tree_symbol($2, TOK_DECLID)); }
          ;

statement : block     { $$ = $1; }
          | vardecl   { $$ = $1; }
          | while     { $$ = $1; }
          | ifelse    { $$ = $1; }
          | return    { $$ = $1; }
          | expr ';'  { free_ast ($2); $$ = $1; }
          ;

basetype  : TOK_VOID { $$ = $1; }
          | TOK_BOOL { $$ = $1; }
          | TOK_CHAR { $$ = $1; }
          | TOK_INT { $$ = $1; }
          | TOK_STRING { $$ = $1; }
          | TOK_IDENT { $$ = upd_tree_symbol($1, TOK_DECLID); }
          ;

block     : '{' statement '}' { free_ast2 ($1, $3); $$ = $2; }
          | '{' '}' { free_ast2 ($1, $2); }
          | ';' { free_ast ($1); }
          ;

vardecl   : identdecl '=' expr ';' { free_ast ($4); $$ = adopt2 ($2, $1, $3); }
          ;

while     : TOK_WHILE '(' expr ')' statement { free_ast2 ($2, $4); $$ = adopt2 ($1, $3, $5); }
          ;

ifelse    : TOK_IF '(' expr ')' statement { free_ast2 ($2, $4); $$ = adopt2 ($1, $3, $5); }
          | TOK_IF '(' expr ')' statement TOK_ELSE statement
                { free_ast3 ($2, $4, $6);
                  $$ = adopt2 (adopt1sym($1, $3, TOK_IFELSE), $5, $7); }
          ;

return    : TOK_RETURN ';' { free_ast2 ($1, $2); $$ = upd_tree_symbol($1, TOK_RETURNVOID); }
          | TOK_RETURN expr ';' { free_ast ($3); $$ = adopt1 ($1, $2); }
          ;

expr      : binopexpr { $$ = $1; }
          | unopexpr { $$ = $1; }
          | allocator { $$ = $1; }
          | call { $$ = $1; }
          | '(' expr ')' { free_ast2 ($1, $3); $$ = $2; }
          | variable { $$ = $1; }
          | constant { $$ = $1; }
          ;

binopexpr : expr '=' expr { $$ = adopt2 ($2, $1, $3); }
          | expr '+' expr { $$ = adopt2 ($2, $1, $3); }
          | expr '-' expr { $$ = adopt2 ($2, $1, $3); }
          | expr '*' expr { $$ = adopt2 ($2, $1, $3); }
          | expr '/' expr { $$ = adopt2 ($2, $1, $3); }
          | expr '%' expr { $$ = adopt2 ($2, $1, $3); }
          | expr TOK_EQEQ expr { $$ = adopt2 ($2, $1, $3); }
          | expr TOK_LST expr { $$ = adopt2 ($2, $1, $3); }
          | expr TOK_GRT expr { $$ = adopt2 ($2, $1, $3); }
          | expr TOK_GRTEQ expr { $$ = adopt2 ($2, $1, $3); }
          | expr TOK_LSTEQ expr { $$ = adopt2 ($2, $1, $3); }
          | expr TOK_NOTEQ expr { $$ = adopt2 ($2, $1, $3); }
          | expr TOK_NEW expr { $$ = adopt2 ($2, $1, $3); }
          ;

unopexpr  : '+' expr { $$ = adopt1 ($1, $2); }
          | '-' expr { $$ = adopt1 ($1, $2); }
          | '!' expr { $$ = adopt1 ($1, $2); }
          | TOK_NEW expr { $$ = adopt1 ($1, $2); }
          | TOK_ORD expr { $$ = adopt1 ($1, $2); }
          | TOK_CHR expr { $$ = adopt1 ($1, $2); }
          ;

allocator : TOK_NEW TOK_IDENT '(' ')' { free_ast3 ($1, $3, $4);
                                      $$ = adopt1sym ($1, upd_tree_symbol($2, TOK_TYPEID), TOK_NEW); }
          | TOK_NEW TOK_STRING '(' expr ')'
                { free_ast ($2); free_ast3 ($1, $3, $5);
                  $$ = adopt1sym ($1, $4, TOK_NEWSTRING); }
          | TOK_NEW basetype '[' expr ']' { free_ast3 ($1, $3, $5);
                                          $$ = adopt2sym ($1, $2, $4, TOK_NEWARRAY); }
          ;

call      : TOK_IDENT '(' expr ',' expr ')' { free_ast2 ($4, $6);
                                              $$ = adopt3 (upd_tree_symbol($2, TOK_CALL), $1, $3, $5); }
          | TOK_IDENT '(' expr ')' { free_ast ($4); $$ = adopt2 (upd_tree_symbol($2, TOK_CALL), $1, $3); }
          | TOK_IDENT '(' ')' { free_ast ($3); $$ = adopt1 (upd_tree_symbol($2, TOK_CALL), $1); }
          ;

variable  : TOK_IDENT { $$ = $1; }
          | expr '[' expr ']' { free_ast2 ($2, $4); $$ = adopt2sym ($2, $1, $3, TOK_INDEX); }
          | expr '.' TOK_IDENT { $$ = adopt2 ($2, $1, $3); }
          ;

constant  : TOK_NUMBER { $$ = $1; }
          | TOK_CHARCONST { $$ = $1; }
          | TOK_STRCONST { $$ = $1; }
          | TOK_FALSE { $$ = $1; }
          | TOK_TRUE { $$ = $1; }
          | TOK_NIL { $$ = $1; }
          ;

%%

const char* get_yytname (int symbol) {
   return yytname [YYTRANSLATE (symbol)];
}

bool is_defined_token (int symbol) {
   return YYTRANSLATE (symbol) > YYUNDEFTOK;
}

static void* yycalloc (size_t size) {
   void* result = calloc (1, size);
   assert (result != NULL);
   return result;
}
