/*#pragma once
#include<stdio.h>
#include"../Compiler3/Token/Token.h"
//#include"../Compiler3/Parser/Parser.h"


typedef enum _treeNodeType {
    TERMINAL,
    NON_TERMINAL
} TreeNodeType;

typedef struct _TreeNode {
    TreeNodeType node_type;
    union parser {
        Non_Terminals nonTerminalType;
        Terminals TerminalType;
    } Kind;
    
    void* value; // for int char identifier terminals

    struct _TreeNode** childrens;
    int numberOfChildrens;
} TreeNode;
//Parser* parser_init();
//TreeNode* create_abstact_syntax_tree(Parser* parser, Token* tokens);
void parser_treeNode_addChild(TreeNode *, TreeNode *, int);
TreeNode* parser_treeNode_create_TreeNode();
void parser_treeNode_set_type(TreeNode*, TreeNodeType);
void parser_treeNode_set_terminal(TreeNode* , Terminals );
void parser_treeNode_set_non_terminal(TreeNode*, Non_Terminals);
*/