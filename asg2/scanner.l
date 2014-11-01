%{
#include "auxlib.h"
#include "lyutils.h"

#define YY_USER_ACTION  { scanner_useraction (); }
#define IGNORE(THING)   { }

%}

%option 8bit
/*%option debug*/
%option nodefault
%option nounput
%option noyywrap
/*%option verbose*/
/*%option warn*/

TOK_BADCHARCONST    	('([^\\'\n]|\\[\\'"0nt])\n)
TOK_BADIDENT 		({TOK_DIGIT}+{TOK_IDENT})
TOK_BADSTRCONST     	(\"([^\\"\n]|\\[\\'"0nt])*\n)
TOK_BOOL 		(bool) 
TOK_BRKKRB          	(\[\]) 
TOK_CHAR  		(char) 
TOK_CHARCONST 		('([^\\'\n]|\\[\\'"0nt])')
TOK_CHR  		(chr) 
TOK_DIGIT           	[0-9]
TOK_ELSE  		(else) 
TOK_EQEQ            	(==)
TOK_FALSE  		(false) 
TOK_GRTEQ 	        (>=)
TOK_IDENT           	({TOK_LETTER}({TOK_LETTER}|{TOK_DIGIT})*)
TOK_IF  		(if) 
TOK_INT  		(int) 
TOK_LETTER          	[A-Za-z_]
TOK_LSTEQ 	        (<=)  
TOK_NEW 		(new)
TOK_NIL  		(null) 
TOK_NOTEQ 	        (!=)  
TOK_NUMBER          	({TOK_DIGIT}+)
TOK_ORD  		(ord) 
TOK_RETURN 		(return) 
TOK_STRCONST 		(\"([^\\"\n]|\\[\\'"0nt])*\")
TOK_STRING  		(string) 
TOK_STRUCT  		(struct) 
TOK_TRUE  		(true) 
TOK_VOID 		(void)
TOK_WHILE  		(while) 

%%

"#".*           	{ scanner_include(); }
[ \t]+          	{ IGNORE (white space) }
\n              	{ scanner_newline(); }

"!"             	{ return yylval_token ('!'); }
"%"             	{ return yylval_token ('%'); }
"("             	{ return yylval_token ('('); }
")"             	{ return yylval_token (')'); }
"*"             	{ return yylval_token ('*'); }
"+"             	{ return yylval_token ('+'); }
","             	{ return yylval_token (','); }
"-"             	{ return yylval_token ('-'); }
"."             	{ return yylval_token ('.'); }
"/"             	{ return yylval_token ('/'); }
";"             	{ return yylval_token (';'); }
"="             	{ return yylval_token ('='); }
"["             	{ return yylval_token ('['); }
"]"             	{ return yylval_token (']'); }
"^"             	{ return yylval_token ('^'); }
"{"             	{ return yylval_token ('{'); }
"}"             	{ return yylval_token ('}'); }

{TOK_BOOL} 		{ return yylval_token (TOK_BOOL); } 
{TOK_BRKKRB}        	{ return yylval_token (TOK_BRKKRB); }
{TOK_CHARCONST} 	{ return yylval_token (TOK_CHARCONST); }
{TOK_CHAR}  		{ return yylval_token (TOK_CHAR); }
{TOK_CHR}  		{ return yylval_token (TOK_CHR); }
{TOK_ELSE}  		{ return yylval_token (TOK_ELSE); }
{TOK_EQEQ}          	{ return yylval_token (TOK_EQEQ); }
{TOK_FALSE}  		{ return yylval_token (TOK_FALSE); }
{TOK_GRTEQ}         	{ return yylval_token (TOK_GRTEQ); }
{TOK_IDENT}         	{ return yylval_token (TOK_IDENT); }
{TOK_IF}  		{ return yylval_token (TOK_IF); }
{TOK_INT}  		{ return yylval_token (TOK_INT); }
{TOK_LSTEQ}         	{ return yylval_token (TOK_LSTEQ); }
{TOK_NEW} 		{ return yylval_token (TOK_NEW); }
{TOK_NIL}   		{ return yylval_token (TOK_NIL); }
{TOK_NOTEQ}         	{ return yylval_token (TOK_NOTEQ); }
{TOK_NUMBER}        	{ return yylval_token (TOK_NUMBER); }
{TOK_ORD}  		{ return yylval_token (TOK_ORD); }
{TOK_RETURN} 		{ return yylval_token (TOK_RETURN); }
{TOK_STRCONST} 		{ return yylval_token (TOK_STRCONST); }
{TOK_STRING}  		{ return yylval_token (TOK_STRING); }
{TOK_STRUCT}  		{ return yylval_token (TOK_STRUCT); }
{TOK_TRUE}  		{ return yylval_token (TOK_TRUE); }
{TOK_VOID} 		{ return yylval_token (TOK_VOID); }	
{TOK_WHILE}  		{ return yylval_token (TOK_WHILE); }

{TOK_BADIDENT} 		{ scanner_badtoken (yytext);
		  	  return yylval_token (TOK_BADIDENT); }
{TOK_BADSTRCONST}   	{ scanner_badtoken (yytext);
		  	  return yylval_token (TOK_BADSTRCONST); }
{TOK_BADCHARCONST}  	{ scanner_badtoken (yytext);
		  	  return yylval_token (TOK_BADCHARCONST); }
.               	{ scanner_badchar (*yytext); }

%%

