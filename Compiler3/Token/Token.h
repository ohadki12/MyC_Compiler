#pragma once
#include<stdio.h>
#include<stdlib.h>
// Token.h contents
typedef enum TokenType {
    IDENTIFIER_TOKEN,
    OPEN_PARENTHESES_TOKEN,
    CLOSE_PARENTHESES_TOKEN,
    OPEN_BRACE_TOKEN,
    CLOSE_BRACE_TOKEN,
    COMMA_TOKEN,
    SEMICOLON_TOKEN,
    GETCHAR_TOKEN, 
    PUTCHAR_TOKEN,
    INT_TOKEN,
    CHAR_TOKEN,
    VOID_TOKEN,
    ASSIGNMENT_TOKEN,
    RETURN_TOKEN,
    IF_TOKEN,
    ELSE_TOKEN, 
    WHILE_TOKEN,
    LOGICAL_OR_TOKEN,
    LOGICAL_AND_TOKEN,
    EQUAL_TO_TOKEN,
    NOT_EQUAL_TO_TOKEN,
    LESS_THEN_TOKEN,
    GREATER_THEN_TOKEN,  
    LESS_THEN_OR_EQUAL_TO_TOKEN,
    GREATER_THEN_OR_EQUAL_TO_TOKEN,
    PLUS_TOKEN,
    MINUS_TOKEN,
    MULTIPLY_TOKEN,
    DIVIDE_TOKEN,
    MODULO_TOKEN,
    LOGICAL_NOT_TOKEN,
    DIGIT_TOKEN,
    CHAR_LITERAL_TOKEN,
    DOLLAR_TOKEN,
    ERROR_TOKEN,
    END_OF_TOKENS_TOKEN
} Terminals;

typedef struct {
    // the type of the token
    enum TokenType type;

    // the values for identifer, character, or integer
    union {
        int integer;
        char *identifer;
        char ch;
    } value;

    // the line which the token appear
    // for reporting an error
    int SourceCodeLine;  // the line of the error
    int CharacterInSourceCodeLine; // the character of the error
    
} Token;

void printTokenTypeName(int token);
void printTokenArray(Token*);

