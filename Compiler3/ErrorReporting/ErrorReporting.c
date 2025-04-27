#include"ErrorReporting.h"



void ErrorReporting_Switch_To_Red(){
    printf(Red);
}
void ErrorReporting_Switch_To_White(){
    printf(White);
}
void ErrorReporting_Switch_To_Yellow(){
    printf(Yellow);
}
void ErrroReporting_Switch_To_Purpule(){
    printf(Purple);
}
void ErrroReporting_Switch_To_Cyan(){
    printf(Cyan);
}
void ErrorReporting_PrintSourceCodeLine(int line){
    // this function prints the wanted line in the source code

    // define local variables
    char* line_p;
    
    // getting the wanted line
    line_p = getSourceCodeLine(line);

    // printing the line
    fputs(line_p, stdout);  // using fputs to avoid formating printing, like if the character % appear, etc.
}
void ErrorReporting_PrintErrorPhase(ErrorPhaseType type){
    if(type == ERROR_ON_LEXER_PHASE){
        printf("Lexer error:");
    }
    else if(type == ERROR_ON_PARSER_PHASE){
        printf("Parser error:");
    }
    else if(type == ERROR_ON_SEMANTIC_PHASE){
        printf("Semantic error:");
    }
}
void ErrorReporting_ReportError_Base(ErrorPhaseType errorPhaseType, int line, int char_in_line){
    // going line down
    printf("\n");

    // print the phase of error
    ErrroReporting_Switch_To_Purpule();
    ErrorReporting_PrintErrorPhase(errorPhaseType);

    // printing file name
    printf(" in file: ");

    // switch to red color
    ErrorReporting_Switch_To_Red();

    printf("\"");

    // switch to blue color
    ErrroReporting_Switch_To_Cyan();

    //printf("yourfile.txt");
    printf("%s", getSourceCodeFileName());

    // switch back to red
    ErrorReporting_Switch_To_Red();

    printf("\"");

    // print the base error format
    ErrorReporting_Switch_To_Red();
    printf(" on line %d:%d: \n", line, char_in_line);

    // print the source code line
    ErrorReporting_PrintSourceCodeLine(line);

    // returning to white color
    ErrorReporting_Switch_To_White();

}

void ErrorReporting_ReportError(char* errorMsg){
    // switch to red color
    ErrorReporting_Switch_To_Red();
    
    // outputing the error message
    printf(errorMsg);
    // switching back to white
    ErrorReporting_Switch_To_White();
    
    printf("\n"); // go line down
    
}

