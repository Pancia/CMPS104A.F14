
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton interface for Bison's Yacc-like parsers in C
   
      Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.
   
   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     ROOT = 258,
     TOK_IDENT = 259,
     TOK_NUMBER = 260,
     TOK_EQEQ = 261,
     TOK_BRKKRB = 262,
     TOK_NOTEQ = 263,
     TOK_LSTEQ = 264,
     TOK_GRTEQ = 265,
     TOK_VOID = 266,
     TOK_BOOL = 267,
     TOK_CHAR = 268,
     TOK_INT = 269,
     TOK_STRING = 270,
     TOK_STRUCT = 271,
     TOK_IF = 272,
     TOK_ELSE = 273,
     TOK_WHILE = 274,
     TOK_RETURN = 275,
     TOK_FALSE = 276,
     TOK_TRUE = 277,
     TOK_NIL = 278,
     TOK_ORD = 279,
     TOK_CHR = 280,
     TOK_NEW = 281,
     TOK_CHARCONST = 282,
     TOK_STRCONST = 283,
     TOK_BADIDENT = 284,
     TOK_BADSTRCONST = 285,
     TOK_BADCHARCONST = 286,
     NEG = 289,
     POS = 290
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef int YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE yylval;


