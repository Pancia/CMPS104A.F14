/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton interface for Bison's Yacc-like parsers in C

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

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
     TOK_IFELSE = 287,
     TOK_BINOP = 288,
     TOK_TYPEID = 289,
     TOK_UNOP = 290,
     TOK_FIELD = 291,
     TOK_FUNCTION = 292,
     TOK_DECLID = 293,
     TOK_RETURNVOID = 294,
     TOK_NEWSTRING = 295,
     TOK_NEWARRAY = 296,
     TOK_CALL = 297,
     TOK_INDEX = 298,
     TOK_COMP = 299,
     NEG = 302,
     POS = 303
   };
#endif
/* Tokens.  */
#define ROOT 258
#define TOK_IDENT 259
#define TOK_NUMBER 260
#define TOK_EQEQ 261
#define TOK_BRKKRB 262
#define TOK_NOTEQ 263
#define TOK_LSTEQ 264
#define TOK_GRTEQ 265
#define TOK_VOID 266
#define TOK_BOOL 267
#define TOK_CHAR 268
#define TOK_INT 269
#define TOK_STRING 270
#define TOK_STRUCT 271
#define TOK_IF 272
#define TOK_ELSE 273
#define TOK_WHILE 274
#define TOK_RETURN 275
#define TOK_FALSE 276
#define TOK_TRUE 277
#define TOK_NIL 278
#define TOK_ORD 279
#define TOK_CHR 280
#define TOK_NEW 281
#define TOK_CHARCONST 282
#define TOK_STRCONST 283
#define TOK_BADIDENT 284
#define TOK_BADSTRCONST 285
#define TOK_BADCHARCONST 286
#define TOK_IFELSE 287
#define TOK_BINOP 288
#define TOK_TYPEID 289
#define TOK_UNOP 290
#define TOK_FIELD 291
#define TOK_FUNCTION 292
#define TOK_DECLID 293
#define TOK_RETURNVOID 294
#define TOK_NEWSTRING 295
#define TOK_NEWARRAY 296
#define TOK_CALL 297
#define TOK_INDEX 298
#define TOK_COMP 299
#define NEG 302
#define POS 303




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef int YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;

