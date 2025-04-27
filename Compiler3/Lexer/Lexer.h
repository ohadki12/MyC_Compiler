#pragma once
#include"Lexer_dfa/Lexer_dfa.h"
#include"../ErrorReporting/ErrorReporting.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

typedef struct LexerSourceCodeStorage
{
    int number_of_lines;
    char**lines;
}LexerSourceCodeStorage;


#define MAX_LINE_LENGTH 256

char* getNextLine(FILE* fp);
Token* getTokens(FILE* fp);

void lexer_AddNewLineIntoStorage(char* line);
LexerSourceCodeStorage* lexer_CreateNewCodeStorage();
char* getSourceCodeLine(int line);
char* Lexer_GetTokenName(Token* token);


void SetsourceCodeFileName(char* fname);
char* getSourceCodeFileName();