#include"Token.h"
void printTokenTypeName(int token) {
    // Static array of token names, matching the updated TokenType enum
    static const char* tokenNames[] = {
        "IDENTIFIER_TOKEN",
        "OPEN_PARENTHESES_TOKEN",
        "CLOSE_PARENTHESES_TOKEN",
        "OPEN_BRACE_TOKEN"
        ,"CLOSE_BRACE_TOKEN",
         "COMMA_TOKEN",
         "SEMICOLON_TOKEN",
        "GETCHAR_TOKEN",
         "PUTCHAR_TOKEN",
         "INT_TOKEN",
         "CHAR_TOKEN",
         "VOID_TOKEN",
         "ASSIGNMENT_TOKEN",
        "RETURN_TOKEN",
        "IF_TOKEN",
        "ELSE_TOKEN", 
        "WHILE_TOKEN",
        "LOGICAL_OR_TOKEN",
        "LOGICAL_AND_TOKEN",
        "EQUAL_TO_TOKEN",
        "NOT_EQUAL_TO_TOKEN",
          "LESS_THEN_TOKEN",
          "GREATER_THEN_TOKEN",  
          "LESS_THEN_OR_EQUAL_TO_TOKEN",
        "GREATER_THEN_OR_EQUAL_TO_TOKEN",
        "PLUS_TOKEN",
        "MINUS_TOKEN"
        ,"MULTIPLY_TOKEN"
        ,"DIVIDE_TOKEN",
        "MODULO_TOKEN",
        "LOGICAL_NOT_TOKEN",
        "DIGIT_TOKEN",
        "CHAR_LITERAL_TOKEN",
          "DOLLAR_TOKEN",  
          "ERROR_TOKEN", 
          "END_OF_TOKENS_TOKEN"
    };

    // Ensure the token is within the valid range of the enum values
    if (token >= 0 && token < sizeof(tokenNames) / sizeof(tokenNames[0])) {
        printf("The token name is: %s   ", tokenNames[token]);
    } else {
        printf("Unknown token\n");
    }
}

void printTokenArray(Token *tokenarray){
    // helper function who prints all the tokens in a visible and ease way
     for(int i = 0; tokenarray[i].type != END_OF_TOKENS_TOKEN; i++){
        
        printTokenTypeName(tokenarray[i].type);

        


        if(tokenarray[i].type == DIGIT_TOKEN){
            printf("DIGIT: %d\n", tokenarray[i].value.integer);
        }
        else if(tokenarray[i].type == IDENTIFIER_TOKEN){
            printf("IDENTIFIER TEXT: %s\n", tokenarray[i].value.identifer);
        }
        else if(tokenarray[i].type == CHAR_LITERAL_TOKEN){
            printf("TOKEN CHAR: '%c'\n", tokenarray[i].value.ch);
        }
        else{
            printf("\n");
        }

        printf("\nthe token line: %d\n", tokenarray[i].SourceCodeLine);
        printf("the token char in line: %d\n\n\n", tokenarray[i].CharacterInSourceCodeLine);
    }
}
