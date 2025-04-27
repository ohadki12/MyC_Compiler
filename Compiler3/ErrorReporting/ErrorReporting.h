#pragma once
#include<stdio.h>
#include"../Lexer/Lexer.h"


// define colors for printing

// red for errors
#define Red "\033[0;31m"

// black for normal printing
#define White  "\033[0;37m"

// yelow for warnings
#define Yellow "\033[0;33m"

// purpule for printing types
#define Purple "\033[0;35m"

// cyan for printing file name
#define Cyan "\033[0;36m"


typedef enum ErrorPhase
{
    ERROR_ON_LEXER_PHASE,
    ERROR_ON_PARSER_PHASE,
    ERROR_ON_SEMANTIC_PHASE

}ErrorPhaseType;

// Function declarations
void ErrorReporting_Switch_To_Red();
void ErrorReporting_Switch_To_White();
void ErrorReporting_Switch_To_Yellow();
void ErrroReporting_Switch_To_Purpule();
void ErrorReporting_PrintSourceCodeLine(int line);
void ErrorReporting_PrintErrorPhase(ErrorPhaseType type);
void ErrorReporting_ReportError_Base(ErrorPhaseType errorPhaseType, int line, int char_in_line);

void ErrorReporting_ReportError(char*);