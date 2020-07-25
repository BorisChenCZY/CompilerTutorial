%{
#include <string.h>
#include "util.h"
#include "tokens.h"
#include "errormsg.h"

int charPos=1;

int yywrap(void)
{
 charPos=1;
 return 1;
}


void adjust(void)
{
 EM_tokPos=charPos;
 charPos+=yyleng;
}

%}

digits [0-9]+
%Start COMMENT
%%
<INITIAL>if                    {adjust(); return IF;}
<INITIAL>for  	               {adjust(); return FOR;}
<INITIAL>var                   {adjust(); return VAR;}
<INITIAL>function              {adjust(); return FUNCTION;}
<INITIAL>array                 {adjust(); return ARRAY;}
<INITIAL>of                    {adjust(); return OF;}
<INITIAL>nil                   {adjust(); return NIL;}
<INITIAL>then                  {adjust(); return THEN;}
<INITIAL>else                  {adjust(); return ELSE;}
<INITIAL>to                    {adjust(); return TO;}
<INITIAL>break                 {adjust(); return BREAK;}
<INITIAL>let                   {adjust(); return LET;}
<INITIAL>in                    {adjust(); return IN;}
<INITIAL>end                   {adjust(); return END;}
<INITIAL>[0-9]+                {adjust(); yylval.ival = atoi(yytext); return INT;}
<INITIAL>\"(.*?)\"             {adjust(); yylval.sval = String(yytext); return STRING;} 
<INITIAL>\/\*                  {BEGIN COMMENT;}
<COMMENT>.                     {continue;}
<COMMENT>\*\/                  {BEGIN INITIAL;}
<INITIAL>\{                    {adjust(); return LBRACE;}
<INITIAL>\}                    {adjust(); return RBRACE;}
<INITIAL>\(                    {adjust(); return LPAREN;}
<INITIAL>\)                    {adjust(); return RPAREN;}
<INITIAL>\[                    {adjust(); return LBRACK;}
<INITIAL>\]                    {adjust(); return RBRACK;}
<INITIAL>\.                    {adjust(); return DOT;}
<INITIAL>\+                    {adjust(); return PLUS;}
<INITIAL>\-                    {adjust(); return MINUS;}
<INITIAL>\*                    {adjust(); return TIMES;}
<INITIAL>\/                    {adjust(); return DIVIDE;}
<INITIAL>:=                    {adjust(); return ASSIGN;}
<INITIAL>:                     {adjust(); return COLON;}
<INITIAL>type                  {adjust(); return TYPE;}
<INITIAL>=                     {adjust(); return EQ;}
<INITIAL>[A-Za-z][A-Za-z0-9_]+ {adjust(); yylval.sval = String(yytext); return ID;}
[ \t\n]                        {continue;}
.	 {adjust(); EM_error(EM_tokPos,"illegal token");}
