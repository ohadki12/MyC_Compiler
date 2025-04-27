#pragma once
#include"../../Token/Token.h"
#include<string.h>
#include<ctype.h>

// Lexer_dfa.h contents
#define NUMBER_OF_STATES 92
#define NUMBER_OF_CHARACTERS 95

// Enum for token types - Already defined in Token.h, so no need to repeat.

// Act structure to define the transition states
typedef struct {
    int nextState;
} Act;

// StateType structure for defining state types
typedef struct {
    enum { CONTINUE_STATE, FINAL_STATE, ERROR_STATE } stateType;
} StateType;

// Lexer_dfa structure for the DFA matrix and state types
typedef struct {
    Act ActMatrix[NUMBER_OF_STATES][NUMBER_OF_CHARACTERS];
    StateType state_type[NUMBER_OF_STATES];
} Lexer_dfa;

// Function prototypes for Lexer_dfa
Terminals getTokenType(int FinalStage);
Lexer_dfa* init_dfa();
int getIndexFromSymbol(char symbol);
char getSymbolFromIndex(int index);
void set_dfa_matrix_row(Lexer_dfa* dfa, int state, int idenState, int delState, int errState);
void set_dfa_keyword_token(Lexer_dfa* dfa, int states[], char symbols[]);
void set_dfa_state_symbol(Lexer_dfa* dfa, int state, char symbol, Act act);
void set_dfa_int(Lexer_dfa* dfa);
void set_dfa_char(Lexer_dfa* dfa);
void set_dfa_if(Lexer_dfa* dfa);
void set_dfa_return(Lexer_dfa* dfa);
void set_dfa_getchar(Lexer_dfa* dfa);
void set_dfa_putchar(Lexer_dfa* dfa);
void set_dfa_else(Lexer_dfa* dfa);
void set_dfa_void(Lexer_dfa* dfa);
void set_dfa_while(Lexer_dfa* dfa);
void set_dfa_semicolon(Lexer_dfa* dfa);
void set_dfa_operators(Lexer_dfa* dfa);
void set_dfa_brackets(Lexer_dfa* dfa);
void set_dfa_comama(Lexer_dfa* dfa);
void set_dfa_comparisons(Lexer_dfa* dfa);
void set_dfa_digit_literal(Lexer_dfa* dfa);
void set_dfa_char_literal(Lexer_dfa* dfa);
void set_dfa_logical_and(Lexer_dfa* dfa);
void set_dfa_logical_or(Lexer_dfa* dfa);
void set_dfa_stages_types(Lexer_dfa* dfa);
void init_lexer_dfa(Lexer_dfa* dfa);
Act getAct(Lexer_dfa *dfa, int curState, char curChar);
StateType getStateType(Lexer_dfa *dfa, int curState);
int toStepBack(int fState);