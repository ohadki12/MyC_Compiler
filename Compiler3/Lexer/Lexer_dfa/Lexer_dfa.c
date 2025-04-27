#include"Lexer_dfa.h"
Terminals getTokenType(int FinalStage){
    switch (FinalStage)
    {
        case 16:
            return IF_TOKEN;
        case 4:
            return INT_TOKEN;
        case 14:
            return VOID_TOKEN;
        case 9:
            return  CHAR_TOKEN;
        case 22:
            return WHILE_TOKEN;
        case 36:
            return PLUS_TOKEN;
        case 37:
            return MINUS_TOKEN;
        case 38:
            return MULTIPLY_TOKEN;
        case 39:
            return DIVIDE_TOKEN;
        case 40:
            return MODULO_TOKEN;
        case 80:
            return OPEN_PARENTHESES_TOKEN;
        case 81:
            return CLOSE_PARENTHESES_TOKEN;
        case 78:
            return OPEN_BRACE_TOKEN;
        case 79:
            return CLOSE_BRACE_TOKEN;
        case 30:
            return DIGIT_TOKEN;
        case 86:
            return LOGICAL_AND_TOKEN;
        case 88:
            return LOGICAL_OR_TOKEN;
        case 84:
            return CHAR_LITERAL_TOKEN;
        case 77:
            return SEMICOLON_TOKEN;
        case 45:
            return ASSIGNMENT_TOKEN;
        case 52:
            return EQUAL_TO_TOKEN;
        case 46:
            return LOGICAL_NOT_TOKEN;
        case 51:
            return NOT_EQUAL_TO_TOKEN;
        case 47:
            return GREATER_THEN_TOKEN;
        case 50:
            return GREATER_THEN_OR_EQUAL_TO_TOKEN;
        case 48:
            return LESS_THEN_TOKEN;
        case 49:
            return LESS_THEN_OR_EQUAL_TO_TOKEN;
        case 91:
        case 25:
        case 26:
        case 29:
        case 90:
        case 89:
            return ERROR_TOKEN;
        case 59:
            return RETURN_TOKEN;
        case 28:
            return IDENTIFIER_TOKEN;
        case 35:
            return ELSE_TOKEN;
        case 67:
            return GETCHAR_TOKEN;
        case 75:
            return PUTCHAR_TOKEN;
        case 76:
            return COMMA_TOKEN;
    }
    return -1;
}
Lexer_dfa* init_dfa(){
    // creates a new lexer dfa object
    // define varivales
    int x, y;
    Act act;

    // allocating memeory for the dfa
    Lexer_dfa *dfa = (Lexer_dfa*)malloc(sizeof(Lexer_dfa));
    if(dfa == NULL){
        printf("malloc error!");
        exit(1);
    }
    //printf("allocated memory for dfa with pointer: %d", dfa);

    act.nextState = - 1;

    // intiating all indexes in the matrix to -1, indicate a spot unintallize
    for(y = 0; y < NUMBER_OF_STATES; y++){
        for(x = 0; x < NUMBER_OF_CHARACTERS; x++){
            dfa->ActMatrix[y][x] = act;
        }
    }

    // returning the lexer dfa object
    return dfa;
}
int getIndexFromSymbol(char symbol){
    // returning the index of a given symbol in the matrix
    return symbol - 32;
}
char getSymbolFromIndex(int index){
    // retutn the char that is in the given index
    return index + 32;
}

void set_dfa_matrix_row(Lexer_dfa* dfa, int state, int idenState, int delState, int errState){
    // classifying the characters 
    char identifiers[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_";
    char delimiters[] = "(),;{}<>=!*+-/%|&' ";
    char errors[] =  "@#$^`~?.:#\"[]^\\";

    // initiating integer for loop
    int i;

    // setting the identifiers, identifier state is in q27
    for(i = 0; i < sizeof(identifiers) / sizeof(identifiers[0]) - 1; i++){
        // checking that we are not overriding an existing act
        if(dfa->ActMatrix[state][getIndexFromSymbol(identifiers[i])].nextState == - 1){
            set_dfa_state_symbol(dfa, state, identifiers[i], (Act){idenState});
        }
    }

    // setting the delimiters, which is identifier final state in q28
    for(i = 0; i < sizeof(delimiters) / sizeof(delimiters[0]) - 1; i++){
        // checking that we are not overriding an existing act
        if(dfa->ActMatrix[state][getIndexFromSymbol(delimiters[i])].nextState == - 1){
            set_dfa_state_symbol(dfa, state, delimiters[i], (Act){delState});
        }
    }

    // setting the errors state, going into invaild char error final state in q29
    for(i = 0; i < sizeof(errors) / sizeof(errors[0]) - 1; i++){
        // checking that we are not overriding an existing act
        if(dfa->ActMatrix[state][getIndexFromSymbol(errors[i])].nextState == - 1){
            set_dfa_state_symbol(dfa, state, errors[i], (Act){errState});
        }
        
    }
}
void set_dfa_keyword_token(Lexer_dfa* dfa, int states[], char symbols[]){
    // setting a token path in the matrix
    // define variables
    int i;
    
    // initiating the states acoording to the symbols

    set_dfa_state_symbol(dfa, states[0], symbols[0], (Act){states[1]});

    for(i = 1; symbols[i] != '\0'; i++){
        //q27 -> identifier state
        //q28 -> identifier final state
        //q29 -> unknown symbol final state error
        set_dfa_matrix_row(dfa, states[i], 27, 28, 29);

        set_dfa_state_symbol(dfa, states[i], symbols[i], (Act){states[i + 1]});
    }
    set_dfa_matrix_row(dfa, states[i], 27, states[i + 1], 29);
}
void set_dfa_state_symbol(Lexer_dfa* dfa, int state, char symbol, Act act){
    // intiating a symbol in a state to do the given act
    dfa->ActMatrix[state][getIndexFromSymbol(symbol)] = act;
}
void set_dfa_int(Lexer_dfa* dfa){
    // setting int token...
    set_dfa_keyword_token(dfa, (int[]){0,1,2, 3, 4}, "int");

}
void set_dfa_identifier(Lexer_dfa* dfa){
    set_dfa_matrix_row(dfa, 27, 27, 28, 29);
}
void set_dfa_char(Lexer_dfa* dfa){
    // setting char token...
    set_dfa_keyword_token(dfa, (int[]){0,5,6,7,8,9}, "char");

}
void set_dfa_if(Lexer_dfa* dfa){
    // setting if token...
    set_dfa_keyword_token(dfa, (int[]){0,1, 15, 16}, "if");

}
void set_dfa_return(Lexer_dfa* dfa){
    // setting return token
    set_dfa_keyword_token(dfa, (int[]){0,53,54,55,56,57,58,59}, "return");

    
}
void set_dfa_getchar(Lexer_dfa *dfa){
    // setting the getchar token keyword
    set_dfa_keyword_token(dfa, (int[]){0, 60, 61, 62, 63, 64, 65, 66, 67}, "getchar");
}
void set_dfa_putchar(Lexer_dfa* dfa){
    // setting the putchar token keyword 
    set_dfa_keyword_token(dfa, (int[]){0, 68, 69, 70, 71, 72, 73, 74, 75}, "putchar");
} 
void set_dfa_else(Lexer_dfa* dfa){
    // setting the else token
    set_dfa_keyword_token(dfa, (int[]){0,31,32,33,34,35}, "else");
}
void set_dfa_void(Lexer_dfa* dfa){
    // setting the void token
    set_dfa_keyword_token(dfa, (int[]){0,10, 11, 12, 13, 14}, "void");

}
void set_dfa_while(Lexer_dfa* dfa){
    // setting the while token in the dfa matrix
    set_dfa_keyword_token(dfa, (int[]){0,17,18,19,20,21, 22}, "while");

}
void set_dfa_semicolon(Lexer_dfa* dfa){
    // setting the token for ; token
    
    set_dfa_state_symbol(dfa, 0, ';', (Act){77});
}
void set_dfa_operators(Lexer_dfa* dfa){
    // settting the tokens for operators
    
    set_dfa_state_symbol(dfa, 0, '+', (Act){36});

    set_dfa_state_symbol(dfa, 0, '-', (Act){37});

    set_dfa_state_symbol(dfa, 0, '*', (Act){38});

    set_dfa_state_symbol(dfa, 0, '/', (Act){39});

    set_dfa_state_symbol(dfa, 0, '%', (Act){40});
}
void set_dfa_brackets(Lexer_dfa* dfa){
    // set brackets


    set_dfa_state_symbol(dfa, 0, '{', (Act){78});

    set_dfa_state_symbol(dfa, 0, '}', (Act){79});

    set_dfa_state_symbol(dfa, 0, ')', (Act){81});

    set_dfa_state_symbol(dfa, 0, '(', (Act){80});

}
void set_dfa_comama(Lexer_dfa* dfa){
    //setting the comma token in the dfa matrix
    set_dfa_state_symbol(dfa, 0, ',', (Act){76});

}
void set_dfa_comparisons(Lexer_dfa* dfa){
    // set comperisions

    set_dfa_state_symbol(dfa, 0, '=', (Act){41});
    set_dfa_matrix_row(dfa,41, 45, 45, 45);
    set_dfa_state_symbol(dfa, 41, '=', (Act){52});

    set_dfa_state_symbol(dfa, 0, '!', (Act){42});
    set_dfa_matrix_row(dfa,42, 46, 46, 46);
    set_dfa_state_symbol(dfa, 42, '=', (Act){51});

    set_dfa_state_symbol(dfa, 0, '>', (Act){43});
    set_dfa_matrix_row(dfa,43, 47, 47, 47);
    set_dfa_state_symbol(dfa, 43, '=', (Act){50});

    set_dfa_state_symbol(dfa, 0, '<', (Act){44});
    set_dfa_matrix_row(dfa,44, 48, 48, 48);
    set_dfa_state_symbol(dfa, 44, '=', (Act){49});
}
void set_dfa_digit_literal(Lexer_dfa* dfa){
    // setting the path for identifying a digit token
    int i;

    set_dfa_matrix_row(dfa, 23,26, 30,29);
    set_dfa_matrix_row(dfa, 24, 26, 91, 29);
    for(i=0 ; i < 10; i++){
        set_dfa_state_symbol(dfa, 0, '0' + i, (Act){23});
        set_dfa_state_symbol(dfa, 23, '0' + i, (Act){23});
        set_dfa_state_symbol(dfa, 24, '0' + i, (Act){25});
    }
    set_dfa_state_symbol(dfa, 23, '.', (Act){24});

}
void set_dfa_char_literal(Lexer_dfa* dfa){
    // setting the path for a char litetal
    int i;

    // setting the ' start path
    set_dfa_state_symbol(dfa,0, '\'', (Act){82});
    set_dfa_matrix_row(dfa, 82, 83, 83, 83);
    set_dfa_state_symbol(dfa,83, '\'', (Act){84});

}
void set_dfa_logical_and(Lexer_dfa* dfa){
    set_dfa_state_symbol(dfa, 0, '&', (Act){85});
    set_dfa_matrix_row(dfa, 85, 89,89,89);
    set_dfa_state_symbol(dfa, 85, '&', (Act){86});
}
void set_dfa_logical_or(Lexer_dfa* dfa){
    set_dfa_state_symbol(dfa, 0, '|', (Act){87});
    set_dfa_matrix_row(dfa, 87, 90,90,90);
    set_dfa_state_symbol(dfa, 87, '|', (Act){88});
}
void set_dfa_stages_types(Lexer_dfa* dfa){
    int finals[] = {16, 4, 14, 9, 22, 36, 35, 37, 38, 39, 40, 80, 81, 78, 79, 
        30, 86, 88, 84, 77, 45, 52, 46, 51, 47, 50, 48, 49, 
        59, 28, 67, 75, 76};

    int errors[] = {91, 25, 26, 29, 89, 90};

    int continues[] = {0, 1, 2, 3, 5, 6, 7, 8, 10, 11, 12, 13, 15, 17, 18, 19, 
        20, 21, 23, 24, 27, 31, 32, 33, 34, 41, 42, 43, 44, 
        53, 54, 55, 56, 57, 58, 60, 61, 62, 63, 64, 65,66,68, 69, 
        70, 71, 72, 73,74, 82, 85, 87};
    

    int i;
    // setting all the finals 
    for(i = 0; i < sizeof(finals) / sizeof(finals[0]); i++){
        dfa->state_type[finals[i]].stateType = FINAL_STATE;
    }

    // setting all the erros
    for(i = 0; i < sizeof(errors) / sizeof(errors[0]); i++){
        dfa->state_type[errors[i]].stateType = ERROR_STATE;
    }

    // setting all the continues states
    for(i = 0; i < sizeof(continues) / sizeof(continues[0]); i++){
        dfa->state_type[continues[i]].stateType = CONTINUE_STATE;
    }

}
void init_lexer_dfa(Lexer_dfa* dfa){
    
    set_dfa_matrix_row(dfa, 0, 27, 28, 29);
    set_dfa_state_symbol(dfa, 0, ' ', (Act){0});
    
    // firstly, setting the types of all states
    set_dfa_stages_types(dfa);
    
    // setting the paths to a operators( + - * / %)
    set_dfa_operators(dfa);
    
    //  setting the paths to a patanteses( (, ), {, })
    set_dfa_brackets(dfa);
    
    // setting the paths for comperrisions tokens( ==, !=, <=, >=)
    set_dfa_comparisons(dfa);
    
    // setting the path for semicolon
    set_dfa_semicolon(dfa);
    
    // setting the digits leteral path
    set_dfa_digit_literal(dfa);

    // setting the char literal path
    set_dfa_char_literal(dfa);

    // setting the comma character
    set_dfa_comama(dfa);

    // setting the || and && tokens
    set_dfa_logical_and(dfa);
    set_dfa_logical_or(dfa);

    set_dfa_identifier(dfa);
    
    // setting the paths to a keywords
    set_dfa_char(dfa); // char
    set_dfa_if(dfa); // if
    set_dfa_else(dfa); // else
    set_dfa_int(dfa); // int
    set_dfa_return(dfa); // return
    set_dfa_getchar(dfa); // getchar(i/o)
    set_dfa_putchar(dfa); // putchar(i/o)
    set_dfa_while(dfa); // while
    set_dfa_void(dfa); // void 
    
}
int toStepBack(int fState){
    // return 1 if the final state ended after any DELIMITER character, else return 0
    if(fState == 4 || fState == 9 || fState == 14 || fState == 16 || fState == 22 || fState == 30 || fState == 28 || fState == 35
    || fState == 45 || fState == 46 || fState == 47|| fState == 48|| fState == 59 || fState == 67 || fState == 75){
        return 1;
    } 
    return 0;
}
Act getAct(Lexer_dfa *dfa, int curState, char curChar){
    // returing the act in the current indexes
    Act act = dfa->ActMatrix[curState][getIndexFromSymbol(curChar)];
    return act;
}
StateType getStateType(Lexer_dfa *dfa, int curState){
    // returning the state of the currat 
    StateType st = dfa->state_type[curState];
    return st;
}