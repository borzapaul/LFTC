#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define SAFEALLOC(var,Type) if ((var=(Type*)malloc(sizeof(Type)))==NULL)err("not enough memory");

int line;

char *pCrtCh;

enum {ID,END,CT_INT,ASSIGN,DOUBLE,ELSE,FOR,IF,INT,RETURN,STRUCT,VOID,WHILE,SEMICOLON,EQUAL,BREAK,CHAR,CT_REAL,CT_CHAR,CT_STRING,COMMA,LPAR,RPAR,LBRACKET,RBRACKET,LACC,RACC,ADD,SUB,MUL,DIV,DOT,AND,OR,NOT,ASSIGN,EQUAL,NOTEQ,LESS,LESSEQ,GREATER,GREATEREQ}; // codurile AL

typedef struct _Token
{
    int code; // codul (numele)
    union
    {
        char *text;// folosit pentru ID, CT_STRING (alocat dinamic)
        long int i; // folosit pentru CT_INT, CT_CHAR
        double r; // folosit pentru CT_REAL
    };
    int line;// linia din fisierul de intrare
    struct _Token *next; // inlantuire la urmatorul AL
} Token;

Token *tokens,*lastToken;

Token *addTk(int code)
{
    Token *tk;
    SAFEALLOC(tk,Token);
    tk->code=code;
    tk->line=line;
    tk->next=NULL;
    if(lastToken)
    {
        lastToken->next=tk;
    }
    else
    {
        tokens=tk;
    }
    lastToken=tk;
    return tk;
}

void err(const char *fmt,...)
{
    va_list va;
    va_start(va,fmt);
    fprintf(stderr,"error: ");
    vfprintf(stderr,fmt,va);
    fputc('\n',stderr);
    va_end(va);
    exit(-1);
}

void tkerr(const Token *tk,const char *fmt,...)
{
    va_list va;
    va_start(va,fmt);
    fprintf(stderr,"error in line %d: ",tk->line);
    vfprintf(stderr,fmt,va);
    fputc('\n',stderr);
    va_end(va);
    exit(-1);
}

int getNextToken()
{
    int state=0,nCh;
    char ch;
    const char *pStartCh;
    Token *tk;
    while(1)  // bucla infinita
    {
        ch=*pCrtCh;
        switch(state)
        {
        case 0: // testare tranzitii posibile din starea 0
            if (isalpha(ch)||ch=='_')
            {
                pStartCh=pCrtCh; // memoreaza inceputul ID-ului
                pCrtCh++;// consuma caracterul
                state=1;// trece la noua stare
            }
            else if
            (ch=='=')
            {
                pCrtCh++;
                state=3;
            }
            else if(ch==' '||ch=='\r'||ch=='\t')
            {
                pCrtCh++;// consuma caracterul si ramane in starea 0
            }
            else if(ch=='\n')  // tratat separat pentru a actualiza linia curenta
            {
                line++;
                pCrtCh++;
            }
            else if(ch==0) // sfarsit de sir
            {
                addTk(END);
                return END;
            }
            else tkerr(addTk(END),"caracter invalid");
            break;
        case 1:
            if (isalnum(ch)||ch=='_')pCrtCh++;
            else state=2;
            break;
        case 2:
            nCh=pCrtCh-pStartCh;//lungimea cuvantului gasit
	//teste cuvinte cheie
            if(nCh==5&&!memcmp(pStartCh,"break",5))
		tk=addTk(BREAK);
            else if(nCh==4&&!memcmp(pStartCh,"char",4))
		tk=addTk(CHAR);
	    else if(nCh==6&&!memcmp(pStartCh,"double",6))
		tk=addTk(DOUBLE);
	    else if(nCh==4&&!memcmp(pStartCh,"else",4))
		tk=addTk(ELSE);
	    else if(nCh==3&&!memcmp(pStartCh,"for",3))
		tk=addTk(FOR);
	    else if(nCh==2&&!memcmp(pStartCh,"if",2))
		tk=addTk(IF);
	    else if(nCh==3&&!memcmp(pStartCh,"int",3))
		tk=addTk(INT);
	    else if(nCh==6&&!memcmp(pStartCh,"return",6))
		tk=addTk(RETURN);
	    else if(nCh==6&&!memcmp(pStartCh,"struct",6))
		tk=addTk(STRUCT);
	    else if(nCh==4&&!memcmp(pStartCh,"void",4))
		tk=addTk(VOID);
	    else if(nCh==5&&!memcmp(pStartCh,"while",5))
		tk=addTk(WHILE);
	    else if(nCh==3&&!memcmp(pStartCh,"end",3))
		tk=addTk(END);
		//... toate cuvintele cheie ...
            else   // daca nu este un cuvant cheie, atunci e un ID
            {
                tk=addTk(ID);
                tk->text=createString(pStartCh,pCrtCh);
            }
            return tk->code;
        case 3:
            if(ch=='=')
            {
                pCrtCh++;
                state=4;
            }
            else
                state=5;
            break;
        case 4:
            addTk(EQUAL);
            return EQUAL;
        case 5:
            addTk(ASSIGN);
            return ASSIGN;
        }
    }
}


