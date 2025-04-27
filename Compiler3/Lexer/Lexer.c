#include"Lexer.h"

// create global storage struct that will be alive untill program exit
LexerSourceCodeStorage* SourceCodeStorage;

// create a gloval storage that will keep the name of the souce code
char* SOURCE_CODE_FILE;

char* Lexer_GetTokenName(Token* token) {
    // Static array of token names, matching the updated TokenType enum
    char* tokenNames[] = {
        "identifier",
        "(",
        ")",
        "{",
        "}",
        ",",
        ";",
        "getchar",
        "putchar",
        "int",
        "char",
        "void",
        "=",
        "return",
        "if",
        "else", 
        "while",
        "||",
        "&&",
        "==",
        "!=",
        "<",
        ">",  
        "<=",
        ">=",
        "+",
        "-",
        "*",
        "/",
        "%",
        "!",
        "int_literal",
        "char_literal",
        "$",  
        "ERROR_TOKEN", // No corresponding name provided, keeping original
        "END_OF_TOKENS_TOKEN" // No corresponding name provided, keeping original
    };
    
    //if(token->type > 36) return NULL;
    return tokenNames[token->type];
}
char* getNextLine(FILE* fp) {
    // creating a temporery buffer to hold the line data
    char* buffer = (char*)malloc(sizeof(char) * MAX_LINE_LENGTH + 2);
    char* temp = NULL;
    if(buffer == NULL){
        printf("MALLOC ERROR");
        exit(-1);
    }
    if (fp == NULL || fgets(buffer, MAX_LINE_LENGTH, fp) == NULL) {
        return NULL; // Return NULL if end of file or error
    }

    // add the buffer with delimiter at the end
    temp = buffer;
    while (*temp != '\0') temp++;
    *(temp) = ' ';
    *(temp + 1) = 0;
    
    return buffer; // Return the line read
}
void addNewToken(Token** tokens,int *tokensLen, Terminals TokenType, void* value, int TokenLine, int TokenCharacterInLine){
    //printf("\n\n\ni M BEING PRINTED\n\n\n");
    if(*tokens == NULL && *tokensLen == 0){
        *tokens = (Token*)malloc(sizeof(Token) * 1);
        if(*tokens == NULL){
            printf("MALLOC ERROR!");
            exit(1);
        }
        *tokensLen = 1;
        
    }
    else if(*tokens != NULL && *tokensLen != 0){
        // increasing the size of the array
        (*tokensLen)++;
        *tokens = (Token*)realloc(*tokens, sizeof(Token) * *tokensLen);
        if(*tokens == NULL){
            printf("MALLOC ERROR!");
            exit(1);
        }
    }

    // setting the token type
    (*tokens)[*tokensLen - 1].type = TokenType;
    //printf("Creating new token element\n");
    // if type identifier, number or character, add their value
    if(TokenType == IDENTIFIER_TOKEN){
        //printf("this is identifier, with the data: %s", (char*)(value));
        (*tokens)[*tokensLen - 1].value.identifer = (char*)(value);
    }
    else if(TokenType == DIGIT_TOKEN){
        (*tokens)[*tokensLen - 1].value.integer = *(int*)(value);
        free(value);
    }
    else if(TokenType == CHAR_LITERAL_TOKEN){
    
        (*tokens)[*tokensLen - 1].value.ch = *(char*)(value);
        free(value);
    }

    // setting the token error values
    (*tokens)[*tokensLen - 1].SourceCodeLine = TokenLine;
    (*tokens)[*tokensLen - 1].CharacterInSourceCodeLine = TokenCharacterInLine;
    

}

void* extractNumberFromStream(char* stream, size_t stream_size) {
    if (stream_size == 0) {
        return NULL;
    }

    int endIndex = 0;

    while (!isdigit(stream[endIndex])) {
        endIndex--;
    }

    int startIndex = endIndex - 1;
    while (isdigit(stream[startIndex]))
    {
        startIndex--;
    }
    startIndex++;
    
    int token_length = endIndex - startIndex + 1;

    //printf("[DEBUG] Number starts at index %d, length: %d\n", startIndex, token_length);

    char* numStr = malloc(token_length + 1);

    if (!numStr) {
        perror("[ERROR] malloc failed");
        exit(EXIT_FAILURE);
    }
    strncpy(numStr, stream + startIndex, token_length);
    numStr[token_length] = '\0';

    //printf("[DEBUG] Extracted numeric string: %s\n", numStr);

    long number = strtol(numStr, NULL, 10);
    free(numStr);

    int* numPtr = malloc(sizeof(int));
    if (!numPtr) {
        perror("[ERROR] malloc failed");
        exit(EXIT_FAILURE);
    }

    *numPtr = (int)number;
    //printf("[DEBUG] Converted number: %d\n", *numPtr);

    if(stream_size < startIndex * -1){
        perror("[ERROR]");
        exit(EXIT_FAILURE);
    }

    return numPtr;
}

void* extractIdentifierFromStream(char* stream, size_t stream_size) {
    stream--;

    if (stream_size == 0) {
        //printf("[ERROR] Stream size is zero. Returning NULL.\n");
        return NULL;
    }
    int endIndex = 0;
    
    while (!isalnum(stream[endIndex]) && stream[endIndex] != '_')
    {
        endIndex--;
    }
    int startIndex = endIndex - 1;

    while (isalnum(stream[startIndex]) || stream[startIndex] == '_')
    {
        startIndex--;
    }
    startIndex++;
    int token_length = endIndex - startIndex + 1;

    char* identStr = malloc(token_length + 1);

    if (!identStr) {
        perror("[ERROR] malloc failed");
        exit(EXIT_FAILURE);
    }

    strncpy(identStr, stream + startIndex, token_length);
    identStr[token_length] = '\0';

    if(stream_size < startIndex * - 1){
        perror("[ERROR]");
        exit(EXIT_FAILURE);
    }

    return identStr;
}
void* extractCharacterFromStream(char* stream, size_t stream_size) {
    if (stream == NULL || stream_size < 3) {
        //printf("[ERROR] Stream is NULL or too short to contain a valid character literal.\n");
        return NULL;
    }

    for (size_t i = 0; i <= stream_size - 3; i++) {
        char* candidate = stream - i;

        if (candidate[-2] == '\'' && candidate[0] == '\'' && candidate[-1] != '\'') {
            char* ch = malloc(sizeof(char));
            if (!ch) {
                perror("[ERROR] malloc failed");
                exit(EXIT_FAILURE);
            }

            *ch = candidate[-1];

            return ch;
        }
    }
    return NULL;
}
void* getValueForTokenType(char* stream, Terminals type, int streamSize){
    if(type != IDENTIFIER_TOKEN && type != DIGIT_TOKEN && type != CHAR_LITERAL_TOKEN){
        return NULL;
    }
    if(type == DIGIT_TOKEN){
        return extractNumberFromStream(stream, streamSize);
    }
    if(type == IDENTIFIER_TOKEN){
        return extractIdentifierFromStream(stream, streamSize);
    }
    if(type == CHAR_LITERAL_TOKEN){
        //printf("am i here?");
        return extractCharacterFromStream(stream, streamSize);
    }
}

void stepStreamUntilNewToken(char** stream) {
    // This function steps the stream until it reaches a delimiter character.
    // It updates *stream to point to the first character after the delimiter.
    //printf("we are currently at point: %s\n", *stream);

    // Define the delimiter characters
    char* delis = "(),;{}<>=!&*+-/%|' ";
    int c  = 0;

    // Loop through the stream until we find a delimiter
    while (**stream != '\0') {
        // Check if the current character is a delimiter
        for (int j = 0; delis[j] != '\0'; j++) {
            if (**stream == delis[j]) {
                
                return;
            }
        }
        
        (*stream)++;
        c++;
    }
    
}
char* Lexer_PrepareErrorMessage(int val, char ch){
    // define local variale
    char buffer[100];

    // this function creates the different error messages in the lexical analysis
    if(val == 90){
        return _strdup("'|' is not a valid operator; did you mean '||'?");
       
    }
    else if(val == 89){
        return _strdup("'&' is not a valid operator; did you mean '&&'?");
        
    }
    else if(val == 26){
        return _strdup("invalid identifier; identifiers can't begin with a digit");
       
    }
    else if(val == 29){
        snprintf(buffer, sizeof(buffer), "unrecognized character: %c", ch);
        return _strdup(buffer);
        
    }
    else if(val == 25){
        return _strdup("invalid numeric literal; decimal points are not allowed");
    }
}

Token* getTokens(FILE* fp){
    // define variables
    int Tokenslen = 0;
    int currentState = 0;

    int currentLine = 1;
    int currentCharacterInLine = 0;
    int lengthOfCurrentToken = 0;

    char* buffer = NULL;
    char* tempBuffer = NULL;
    StateType currentStateType;
    int currentTokentype;
    void* currentTokenValue;
    Act currentAct;

    // in panic mode?
    int panicMode = 0;

    // this variable hold the pointer into the error message
    char* errorMsg;

    // initiate the global storage
    SourceCodeStorage = lexer_CreateNewCodeStorage();

    // create the dfa
    Lexer_dfa *dfa = init_dfa();
    init_lexer_dfa(dfa);

    // create the array of tokens
    Token *tokenArray = NULL;
    
    // looping until we reed all data
    while (!feof(fp))
    {   
        // getting the chatractes int he current line
        buffer = getNextLine(fp);

        // inserting the current line into the storage
        lexer_AddNewLineIntoStorage(buffer);

        // setting the stream pointer that we use to iterate the chracters in the currnent line
        tempBuffer = buffer;

        // setting the character counter in the current line to be zero
        currentCharacterInLine = 0;
        
        //printf("the char is: %c\n", *tempBuffer);
        while (tempBuffer != NULL && *tempBuffer != '\0')
        {
            //printf("the ascii of the current char is: %d   ", *tempBuffer);
            // checking if current character is lower then ascii 32
            if(*tempBuffer < getSymbolFromIndex(0) || *tempBuffer > getSymbolFromIndex(NUMBER_OF_CHARACTERS - 1)){

                //printf("ma i need to be here?\n");
                //continue;
                *tempBuffer = ' ';
            } 
            else{
                //printf("\n");
            }
            //printf("CURRENT STATE: %d  current char: %c, ascii value: %d  \n", currentState, *tempBuffer, *tempBuffer);
            //printf("_%s_",tempBuffer);
            //printf("\n");
        
            // checking if the current state isn't final 
            currentStateType = getStateType(dfa, currentState);
            //printf("the sttype is: %d\n", currentStateType.stateType);
            // checking the type of the current state
            if(currentStateType.stateType == CONTINUE_STATE){                

                // getting the act in the current index
                currentAct = getAct(dfa, currentState, *tempBuffer);
                currentState = currentAct.nextState;
                //printf("NEXT STATE: %d\n\n", currentState);
                // stepping the str pointer
                tempBuffer++;
                if(currentState == -1){printf("how am i here?"); exit(1);}

                // checking we are not on space character
                if(*tempBuffer > 32){
                    //printf("increasing the counter with the current char is: %c in ascii: %d\n", *tempBuffer, *tempBuffer);
                    // increasing the token length counter
                    lengthOfCurrentToken++;
                }
                
            }
            else if(currentStateType.stateType == FINAL_STATE){
                //printf("starting to create new token in the array");
                // creating new token
                currentTokentype = getTokenType(currentState);
                
                // getting the value of that token, if not identifer, digit, or charcter, it will be null
                currentTokenValue = getValueForTokenType(tempBuffer,currentTokentype, currentCharacterInLine);
                if(currentLine == - 1){
                    printf("COMPILER ERROR");
                    exit(EXIT_FAILURE);
                }
                if(currentCharacterInLine == -1){
                    printf("COMPILER ERROR");
                    exit(EXIT_FAILURE);
                }

                //int typebla = getTokenType(currentState);
                //if(typebla < 0 || typebla > 36) exit(1);

                // creating a new token in the dynamic array with his attributes
                addNewToken(&tokenArray, &Tokenslen, getTokenType(currentState),
                                                 currentTokenValue, currentLine, 
                                                 currentCharacterInLine - lengthOfCurrentToken); //// -lengthOfCurrentToken
                
                // chrcking if to step back
                if(toStepBack(currentState)){
                    //printf("\nstepped back\n");
                    tempBuffer -= 1;
                }
               
                // reseting the state to start state
                currentState = 0;

                // reseting the current token length
                lengthOfCurrentToken = 0;
            }
            else if(currentStateType.stateType == ERROR_STATE){
                //printf("i am here, this is very wierd!");
                // handling lexical error while keeping the lexer working
                //printf("detected an error state in state: %d", currentState);

                // setting panic mode to 1
                panicMode = 1;

                // outputing the base error
                ErrorReporting_ReportError_Base(ERROR_ON_LEXER_PHASE, currentLine, currentCharacterInLine - lengthOfCurrentToken);

                // preparing the error message
                errorMsg = Lexer_PrepareErrorMessage(currentState, *(buffer + currentCharacterInLine - lengthOfCurrentToken));

                // outputing the error msg
                ErrorReporting_ReportError(errorMsg);

                // freeing the error message from the heap
                free(errorMsg);
                
                // reseting the current state back to the inital position
                currentState = 0;

                // reseting the counter of the token length
                lengthOfCurrentToken = 0;

                //printf("calling the maniac function!\n");
                // skipping processing the errored token
                stepStreamUntilNewToken(&tempBuffer);
            }

            // increasing the counter of chatacter in line
            currentCharacterInLine++;
        }

        // freeing the bufer
        if(buffer != NULL) free(buffer);
        
        // STEPPING THE LINE COUTNER
        currentLine++;
    }
    
    // adding another token indicating end of tokesn
    addNewToken(&tokenArray, &Tokenslen, END_OF_TOKENS_TOKEN, NULL, -1, -1);

    if(panicMode){
        free(dfa);
        free(tokenArray);

        return NULL;
    }
    //////tokenArray[Tokenslen - 1].type = END_OF_TOKENS_TOKEN;
    //printf("freeing the dfa memory at: %d\n", dfa);
    free(dfa);
    
    return tokenArray;
}


void lexer_AddNewLineIntoStorage(char* line){
    // this function save the current line into the following line in the storage

    if(SourceCodeStorage == NULL){
        printf("COMPILER ERROR");
        exit(EXIT_FAILURE);
    }
    if(SourceCodeStorage->number_of_lines == 0){
        SourceCodeStorage->lines = (char**)malloc(sizeof(char*) * 1);
        if(SourceCodeStorage->lines == NULL){
            printf("MALLOC ERROR");
            exit(EXIT_FAILURE);
        }
        SourceCodeStorage->number_of_lines = 1;
    }
    else{
        // increasing the memory size
        SourceCodeStorage->lines = (char**)realloc(SourceCodeStorage->lines, sizeof(char*) * ++SourceCodeStorage->number_of_lines);
    }
    
    // saving the new line
    SourceCodeStorage->lines[SourceCodeStorage->number_of_lines - 1] = _strdup(line);
    
}
LexerSourceCodeStorage* lexer_CreateNewCodeStorage(){
    // this function creates a new struct in type LexerSourceCodeStorage and return its memory address

    // define variables
    LexerSourceCodeStorage* storage = (LexerSourceCodeStorage*)malloc(sizeof(LexerSourceCodeStorage));
    if(storage == NULL){
        printf("MALLOC ERROR");
        exit(EXIT_FAILURE);
    }

    storage->number_of_lines = 0;
    storage->lines = NULL;

    return storage;
}
char* getSourceCodeLine(int line){
    // this function responsible for returning the corresponding souce code line

    // checking that we arent accessing a non existing line
    if(line > SourceCodeStorage->number_of_lines || line <= 0){
        return NULL;
    }

    // returning the wanted line that was stored in the global struct
    return SourceCodeStorage->lines[line - 1];
}
void SetsourceCodeFileName(char* fname){
    SOURCE_CODE_FILE = _strdup(fname);
}
char* getSourceCodeFileName(){
    return SOURCE_CODE_FILE;
}