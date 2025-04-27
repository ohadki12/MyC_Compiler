#include"Parser/Parser.h"
#include"Token/Token.h"
#include"Lexer/Lexer.h"
#include"Semantic/Scope/Scope.h"
#include"Semantic/SymbolTable/SymbolTable.h"
#include"Semantic/Scope/ScopeTree.h"
#include"CodeGenerator/CodeGenerator.h"
#include<stdio.h>
#include<stdlib.h>
#include<windows.h>

#define KERNEL32_FILE_PATH "C:\\Program Files (x86)\\Windows Kits\\10\\Lib\\10.0.26100.0\\um\\x64\\kernel32.lib"
#define NASM_FILE_PATH "nasm"
#define LINKER_FILE_PSTH "link"


