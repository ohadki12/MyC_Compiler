#include"Compiler.h"


char* filename_convert_to_extension(const char *input_filename, const char *new_extension) {
    // Validate parameters
    if (input_filename == NULL || new_extension == NULL) {
        return NULL;
    }

    // Find the last dot in the filename
    const char *dot_position = strrchr(input_filename, '.');

    // Calculate the base name length
    size_t base_len;
    if (dot_position != NULL) {
        base_len = dot_position - input_filename;
    } else {
        base_len = strlen(input_filename);
    }

    // Ensure the new extension starts with a dot
    const char *extension_with_dot = new_extension[0] == '.' ? new_extension : NULL;
    char *temp_extension = NULL;

    if (!extension_with_dot) {
        size_t ext_len = strlen(new_extension);
        temp_extension = (char*)malloc(ext_len + 2); // +1 for dot, +1 for null
        if (temp_extension == NULL) {
            return NULL;
        }
        temp_extension[0] = '.';
        strcpy(temp_extension + 1, new_extension);
        extension_with_dot = temp_extension;
    }

    // Allocate memory for new filename
    size_t new_len = base_len + strlen(extension_with_dot) + 1;
    char *output_filename = (char*)malloc(new_len);
    if (output_filename == NULL) {
        free(temp_extension);
        return NULL;
    }

    // Copy base and append new extension
    strncpy(output_filename, input_filename, base_len);
    output_filename[base_len] = '\0';
    strcat(output_filename, extension_with_dot);

    // Clean up temporary string if used
    if (temp_extension) {
        free(temp_extension);
    }

    return output_filename;
}
#include <string.h>

int Compiler_Check_Vaild_FileName(char* fname) {
    if (fname == NULL) return 0;  // Null check

    size_t len = strlen(fname);
    if (len < 4) return 0;  // Must be at least 4 chars to have ".txt"

    // Compare the last 4 characters with ".txt"
    if (strcmp(fname + len - 4, ".txt") == 0) {
        return 1;  // Valid
    } else {
        return 0;  // Invalid
    }
}


int main(int argc, char* argv[]){

    // checking goten atguments
    if(argc != 2){
        printf("EXPECTING EXACTLY ONE ARGUMENT");
        return 1;
    }

    // checking if got a valid filename
    if(!Compiler_Check_Vaild_FileName(argv[1])){
        // here, got an invalid file name
        printf("INVALID FILE NAME\n");
        return 1;
    }
    
    // creating new File object pointer
    FILE *file = fopen(argv[1], "r");
    if(file == NULL){
        printf("SOURCE CODE PATH DOESN'T EXISTS");
        return 1;
    }

    // ================================================
    // first stage, converting the text into tokens
    // the lexer holds the file name for the error handling, seting it
    SetsourceCodeFileName(argv[1]);

    // get the tokens array
    Token *tokenarray = getTokens(file);

    // if we were at panic mode in the lexer
    if(tokenarray == NULL){
        // exiting program
        exit(EXIT_SUCCESS);
    }

    // ===================================================
    // second stage, the parser
    Parser* parser = parser_init();         

    // intiating the parser structure
    init_action_table(parser);
    init_goto_table(parser);
    init_Production_Rules(parser);
    
    // creating the Abstact Syntax Tree and checking syntax of the code
    TreeNode * root = create_abstact_syntax_tree(parser, tokenarray);
    
    // freeing the parser and the token array
    VirtualFree(parser, 0, MEM_RELEASE);
    free(tokenarray);

    // checking if we were at panic mode at the parser
    if(root == NULL){
        // exiting program
        exit(EXIT_SUCCESS);
    }

    // ===================================================
    // third stage
    // creting the scope tree and checking semantic errors
    Scope* main_scope;
    process_program(root, &main_scope);
    
    // checking if we were at panic mode at the semantic phase
    if(main_scope == NULL){
        // freeing the ast tree
        freeAst(root);

        // exiting program
        exit(EXIT_SUCCESS);
    }
    
    // ============================================
    // forth stage
    // back-end, generating the code in assembly

    // creating the destination code
    char* dest_file_name_nasm;
    char* dest_file_name_obj;
    char* dest_file_name_exe;
    
    // creating the new files names
    dest_file_name_nasm = filename_convert_to_extension(argv[1], "nasm");
    dest_file_name_obj = filename_convert_to_extension(argv[1], "obj");
    dest_file_name_exe = filename_convert_to_extension(argv[1], "exe");

    // creating new file(or override if exists)
    FILE *dest_file = fopen(dest_file_name_nasm, "w");
    
    if(dest_file == NULL){
        printf("COMPILER ERROR");
        exit(EXIT_FAILURE);
    }

    // generating the nasm code into the destination file
    CodeGenerator_Generate(root, main_scope, dest_file);

    // closing the file
    fclose(dest_file);
    
    // compiler finished, freeing left over memory
    freeAst(root);
    freeScopeTree(main_scope);

    // final stage, creating the obj file and linking it with kernel32 lib
    char buffer[1000];

    // create the obj command
    sprintf(buffer, "%s -f win64 \"%s\" -o \"%s\"", NASM_FILE_PATH,  dest_file_name_nasm, dest_file_name_obj);

    // executing the command
    int nasm_status = system(buffer);
    if (nasm_status != 0) {
        printf("Error compiling NASM file\n");
        return 1;  // Exit on error
    }
    
    // creating the linking command
    sprintf(buffer, "%s %s kernel32.lib /LIBPATH:\"C:\\Program Files (x86)\\Windows Kits\\10\\Lib\\10.0.26100.0\\um\\x64\" /subsystem:console /machine:x64 /ENTRY:main /out:%s",
        LINKER_FILE_PSTH, dest_file_name_obj, dest_file_name_exe);

    // executing the command
    int gcc_status = system(buffer);
    if (gcc_status != 0) {
       printf("Error linking with microsoft linker\n");
       return 1;  // Exit on error
    
    }

    // finished!
    printf("Executable created successfully!\n");

    return 0;
}

