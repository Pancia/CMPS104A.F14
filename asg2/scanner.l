%{
#include "auxlib.h"
#include "lyutils.h"

#define YY_USER_ACTION  { scanner_useraction (); }
#define IGNORE(THING)   { }

%}

%option 8bit
%option debug
%option nodefault
%option nounput
%option noyywrap
%option verbose
%option warn

LETTER          [A-Za-z_]
DIGIT           [0-9]
NUMBER          ({DIGIT}+)
IDENT           ({LETTER}({LETTER}|{DIGIT})*)
EQEQ            (==)
BRKKRB          (\[\]) 
NOTEQ 	        (!=)  
LSTEQ 	        (<=)  
GRTEQ 	        (>=)
VOID 		(void)
BOOL 		(bool) 
CHAR  		(char) 
INT  		(int) 
STRING  	(string) 
STRUCT  	(struct) 
IF  		(if) 
ELSE  		(else) 
WHILE  		(while) 
RETURN 		(return) 
FALSE  		(false) 
TRUE  		(true) 
NIL  		(null) 
ORD  		(ord) 
CHR  		(chr) 
NEW 		(new)
CHARCONST 	('([^\\'\n]|\\[\\'"0nt])')
STRCONST 	(\"([^\\"\n]|\\[\\'"0nt])*\")
BADIDENT 	({DIGIT}+{IDENT})

/* TODO
//invalid character and string constants (missing a final quote or
//a character following an escape).  Make sure the scanner report
//does not show any jamming states.
*/

%%

"#".*           { scanner_include(); }
[ \t]+          { IGNORE (white space) }
\n              { scanner_newline(); }

{VOID} 		{ return yylval_token (VOID); }	
{BOOL} 		{ return yylval_token (BOOL); } 
{CHAR}  	{ return yylval_token (CHAR); }
{INT}  		{ return yylval_token (INT); }
{STRING}  	{ return yylval_token (STRING); }
{STRUCT}  	{ return yylval_token (STRUCT); }
{IF}  		{ return yylval_token (IF); }
{ELSE}  	{ return yylval_token (ELSE); }
{WHILE}  	{ return yylval_token (WHILE); }
{RETURN} 	{ return yylval_token (RETURN); }
{FALSE}  	{ return yylval_token (FALSE); }
{TRUE}  	{ return yylval_token (TRUE); }
{NIL}   	{ return yylval_token (NIL); }
{ORD}  		{ return yylval_token (ORD); }
{CHR}  		{ return yylval_token (CHR); }
{NEW} 		{ return yylval_token (NEW); }

{CHARCONST} 	{ return yylval_token (CHARCONST); }
{STRCONST} 	{ return yylval_token (STRCONST); }

{NUMBER}        { return yylval_token (NUMBER); }

{IDENT}         { return yylval_token (IDENT); }

{EQEQ}          { return yylval_token (EQEQ); }
{BRKKRB}        { return yylval_token (BRKKRB); }
{NOTEQ}         { return yylval_token (NOTEQ); }
{LSTEQ}         { return yylval_token (LSTEQ); }
{GRTEQ}         { return yylval_token (GRTEQ); }

"="             { return yylval_token ('='); }
"+"             { return yylval_token ('+'); }
"-"             { return yylval_token ('-'); }
"*"             { return yylval_token ('*'); }
"/"             { return yylval_token ('/'); }
"^"             { return yylval_token ('^'); }
"("             { return yylval_token ('('); }
")"             { return yylval_token (')'); }
";"             { return yylval_token (';'); }

"."             { return yylval_token ('.'); }
","             { return yylval_token (','); }
"%"             { return yylval_token ('%'); }
"!"             { return yylval_token ('!'); }
"["             { return yylval_token ('['); }
"]"             { return yylval_token (']'); }
"{"             { return yylval_token ('{'); }
"}"             { return yylval_token ('}'); }

{BADIDENT} 	{ scanner_badtoken (yytext);
		  return yylval_token (BADIDENT); }

.               { scanner_badchar (*yytext); }

%%

