#include <iostream>
#include <string>
#include <vector>

#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "symbol_table.h"
#include "auxlib.h"
#include "lyutils.h"

using namespace std;

size_t reg_counter = 1;

void gen_oil_stuff(ofstream& out, astree* node, int depth, astree* extra);

/**
 *  We take in a node and an old_name,
 *  and sprinkle in old_name with information
 *  from node based on the project specs.
 */
string mangle_name(astree* node, string old_name) {
    string new_name;
    int symbol = node->symbol;

    if (node->block_number != 0) {
        switch (symbol) {
            case TOK_WHILE:
            case TOK_IF:
            case TOK_IFELSE:
                                new_name = old_name + "_"
                                    + to_string(node->filenr) + "_"
                                    + to_string(node->linenr) + "_"
                                    + to_string(node->offset) + ":;";
                                break;
            case TOK_NUMBER:    new_name = old_name;
                                break;
            case TOK_FUNCTION:  new_name = "__" + old_name;
                                break;
            default:            new_name = "_" + to_string(node->block_number)
                                    + "_" + *node->lexinfo;
        }
    } else {
        switch (symbol) {
            case TOK_STRUCT:    new_name = "s_" + old_name;
                                break;
                                //STRUCT FIELD
            case TOK_TYPEID:    new_name = "f_" + *node->lexinfo + "_" + old_name;
                                break;
            case TOK_WHILE:
            case TOK_IF:
            case TOK_IFELSE:
                                new_name = old_name + "_"
                                    + to_string(node->filenr) + "_"
                                    + to_string(node->linenr) + "_"
                                    + to_string(node->offset) + ":;";
                                break;
            case TOK_DECLID:    new_name = "_" + to_string(node->block_number)
                                    + "_" + *node->lexinfo;
                                break;
            default:            new_name = "__" + old_name;
                                break;
        }
    }

    return new_name;
}

/**
 *  convert_type takes in a node, and a struct_name,
 *  if the node's lexinfo is not a known primitive type,
 *  then struct_name is used and is assumed to be the name
 *      of a struct,
 *  otherwise we slightly modify the primitives to match
 *      the project specifications.
 */
string convert_type(astree* node, const string* struct_name) {
    string old_type = *node->lexinfo;
    string new_type;
    if (old_type == "int") {
        new_type = "int";
    } else if (old_type == "char") {
        new_type = "char";
    } else if (old_type == "bool") {
        new_type = "char";
    } else if (old_type == "string") {
        new_type = "char*";
    } else {
        assert(struct_name != nullptr);
        astree* a_struct = new astree();
        a_struct->symbol = TOK_STRUCT;
        a_struct->lexinfo = struct_name;
        new_type = "struct " + mangle_name(a_struct, old_type) + "*";
    }
    return new_type;
}

void gen_struct(ofstream& out, astree* child, int depth) {
    astree* struct_name = child->children[0];
    out << "struct "
        << mangle_name(child, *struct_name->lexinfo)
        << " {" << endl;

    depth++;
    for(size_t i = 1; i < child->children.size(); ++i) {
        astree* type  = child->children[i];
        astree* field = type->children[0];
        string old_name = *field->lexinfo;
        field->lexinfo = struct_name->lexinfo;
        out << string(depth * 3, ' ')
            << convert_type(type, struct_name->lexinfo) << " "
            << mangle_name(field, old_name)
            << ";" << endl;
    }

    out << "};" << endl;
}

void gen_strconst(ofstream& out, astree* node, int depth) {
    if (node->symbol == '=') {
        if (node->children[1]->symbol == TOK_STRCONST) {
            out << "char* "
                << mangle_name(node, *node->children[0]->children[0]->lexinfo)
                << " = " << *node->children[1]->lexinfo
                << endl;
        }
    } else {
        for (astree* child: node->children) {
            gen_strconst(out, child, depth);
        }
    }
}

void gen_return(ofstream& out, astree* node, int depth) {
    astree* return_val = node->children[0];
    out << string(depth * 3, ' ');
    if (return_val != nullptr) {
        if (return_val->symbol == TOK_IDENT) {
            out << "return "
                << mangle_name(return_val, *return_val->lexinfo)
                << ";" << endl;
        } else {
            out << "return "
                << *return_val->lexinfo
                << " " << mangle_name(return_val->children[0], *return_val->children[0]->lexinfo)
                << ";" << endl;
        }
    } else {
        out << "else";
    }
}

void gen_binary(ofstream& out, astree* node, int depth) {
    astree* left = node->children[0];
    astree* right = node->children[1];

    out << "char b" << reg_counter++
        << " = " << mangle_name(left, *left->lexinfo)
        << " " << *node->lexinfo
        << " " << mangle_name(right, *right->lexinfo)
        << ";" << endl;
}

void gen_conditional(ofstream& out, astree* node, int depth, astree* extra) {
    //TODO: Handle case where is node is a unary operator (ie: do switch/if-else)
    gen_binary(out, node, depth);

    out << "if (!b" << reg_counter-1
        << ") goto break_"
        << to_string(extra->filenr) << "_"
        << to_string(extra->linenr) << "_"
        << to_string(extra->offset)
        << ";" << endl;
}

void gen_while(ofstream& out, astree* node, int depth) {
    out << mangle_name(node, *node->lexinfo) << endl;

    gen_conditional(out, node->children[0], depth, node);

    gen_oil_stuff(out, node->children[1], depth, node);

    out << "goto "
        << mangle_name(node, *node->lexinfo) << endl;

    out << "break_"
        << to_string(node->filenr) << "_"
        << to_string(node->linenr) << "_"
        << to_string(node->offset)
        << ":" << endl;
}

void gen_call(ofstream& out, astree* node, int depth) {
    node->children[0]->symbol = TOK_FUNCTION;
    out << mangle_name(node->children[0], *node->children[0]->lexinfo)
        << " (";

    for (size_t i = 1; i < node->children.size(); i++) {
        astree* arg = node->children[i];
        out << mangle_name(arg, *arg->lexinfo);
        if (i+1 != node->children.size()) {
            out << ", ";
        }
    }
    out << ");" << endl;
}

void gen_expression(ofstream& out, astree* node, int depth){
    //cases caught: x=y+2, x=y+2+y+2, x=funct(fucnt(x, 2), 2+3+3+3+funct(x, y))
    
    //handle first child
    switch (node->children[0]->symbol){
        case TOK_IDENT: out << mangle_name(node->children[0], 
                                            *node->children[0]->lexinfo) 
                            << " " << *node->lexinfo << " ";
                        break;

        case TOK_STRCONST:
        case TOK_CHARCONST:
        case TOK_NUMBER:
        case TOK_TRUE:
        case TOK_FALSE: out << *node->children[0]->lexinfo << " " 
                            << *node->lexinfo << " ";
                        break;

        case TOK_CALL: out << "im a call" << endl;
                       break;
    }

    //handle second child
    switch (node->children[1]->symbol){
        case TOK_IDENT: out << mangle_name(node->children[1],
                                           *node->children[1]->lexinfo) 
                            << "; " << endl;
                        break;

        case TOK_STRCONST:
        case TOK_CHARCONST:
        case TOK_NUMBER:
        case TOK_TRUE:
        case TOK_FALSE: out << *node->children[1]->lexinfo << "; " 
                            << endl;
                        break;

        case TOK_CALL: out << "im a call" << endl;
                       break;
    }
}

string to_reg_type(string s) {
    if (s == "char") {
        return "c";
    } else if (s == "int") {
        return "i";
    } else if (s == "string") {
        return "s";
    } else {
        return "i";
    }
}

void gen_eq(ofstream& out, astree* node, int depth) {
    astree* left = node->children[0];
    astree* right = node->children[1];
    if (left->symbol != TOK_IDENT) {
        switch (right->symbol) {
            //case int x=y, char y=z
            case TOK_IDENT:         out << string(depth*3, ' ')
                                        << *left->lexinfo
                                        << " " << mangle_name(left->children[0], *left->children[0]->lexinfo)
                                        << " = " << mangle_name(right, *right->lexinfo)
                                        << ";" << endl;
                                    break;
            //case int x=2, string x="string", char x='c'
            case TOK_NUMBER:
            case TOK_CHARCONST:
            case TOK_STRCONST:
            case TOK_TRUE:
            case TOK_FALSE:         out << string(depth*3, ' ')
                                        << *left->lexinfo << " "
                                        << mangle_name(left->children[0], *left->children[0]->lexinfo)
                                        << " = " << *right->lexinfo
                                        << ";" << endl;
                                    break;
            //case int [] x = new int [];
            case TOK_NEW:           break;
            //case int x = x + 2
            //Do we have || and &&?
            case '+':
            case '-':
            case '/':
            case '*':               out << string(depth*3, ' ')
                                        << *left->lexinfo
                                        << " " << to_reg_type(*left->lexinfo) << reg_counter++
                                        << " = ";
                                    gen_expression(out, right, depth);
                                    out << string(depth*3, ' ')
                                        << *left->lexinfo << " "
                                        << mangle_name(left->children[0], *left->children[0]->lexinfo)
                                        << " = " << to_reg_type(*left->lexinfo) << reg_counter-1
                                        << ";" << endl;
                                    break;
            //count also be tok_call?
            default:                out << string(depth*3, ' ')
                                        << *left->lexinfo
                                        << " " << to_reg_type(*left->lexinfo) << reg_counter++
                                        << " = ";
                                    gen_call(out, right, depth);
                                    out << string(depth*3, ' ')
                                        << *left->lexinfo << " "
                                        << mangle_name(left->children[0], *left->children[0]->lexinfo)
                                        << " = " << to_reg_type(*left->lexinfo) << reg_counter-1
                                        << ";" << endl;
                                    break;
        }
    } else {
        switch (right->symbol) {
            //case x=y
            case TOK_IDENT:         out << string(depth*3, ' ')
                                        << mangle_name(left, *left->lexinfo)
                                        << " = " << mangle_name(right, *right->lexinfo)
                                        << ";" << endl;
                                    break;
            //case x=1, x='y', x="string"
            case TOK_NUMBER:
            case TOK_CHARCONST:
            case TOK_STRCONST:
            case TOK_TRUE:
            case TOK_FALSE:         out << string(depth*3, ' ')
                                        << mangle_name(left, *left->lexinfo)
                                        << " = " << *right->lexinfo
                                        << ";" << endl;
                                    break;
            //case x=new Object()
            case TOK_NEW:           break;
            //case x = x + 2
            //Do we have || and &&?
            case '+':
            case '-':
            case '/':
            case '*':               out << string(depth*3, ' ')
                                        << "int " //do we want to hide this better?
                                        << to_reg_type(*left->lexinfo) << reg_counter++
                                        << " = ";
                                    gen_expression(out, right, depth);
                                    out << string(depth*3, ' ')
                                        << mangle_name(left, *left->lexinfo)
                                        << " = " << to_reg_type(*left->lexinfo) << reg_counter-1
                                        << ";" << endl;
                                    break;
            default:                out << string(depth*3, ' ')
                                        << *left->lexinfo
                                        << " " << to_reg_type(*left->lexinfo) << reg_counter++
                                        << " = ";
                                    gen_call(out, right, depth);
                                    out << string(depth*3, ' ')
                                        << mangle_name(left, *left->lexinfo)
                                        << " = " << to_reg_type(*left->lexinfo) << reg_counter-1
                                        << ";" << endl;
                                    break;
        }
    }
}

void gen_oil_stuff(ofstream& out, astree* node, int depth, astree* extra) {
    switch (node->symbol) {
        case TOK_BLOCK:     for (astree* child: node->children) {
                                gen_oil_stuff(out, child, depth+1, extra);
                            }
                            break;

        case TOK_WHILE:     gen_while(out, node, depth);
                            break;

        case TOK_IF:        gen_binary(out, node->children[0], depth);
                            out << "if (!b" << reg_counter-1 << ") "
                                << "goto fi_"
                                << to_string(node->filenr) << "_"
                                << to_string(node->linenr) << "_"
                                << to_string(node->offset)
                                << ";" << endl;
                            gen_oil_stuff(out, node->children[1], depth, extra);
                            out << "fi_"
                                << to_string(node->filenr) << "_"
                                << to_string(node->linenr) << "_"
                                << to_string(node->offset)
                                << ";" << endl;
                            break;

        case '=':           gen_eq(out, node, depth);
                            break;

        case TOK_RETURN:    gen_return(out, node, depth);
                            break;

        default:            out << string(depth * 3, ' ');
                            out << get_yytname(node->symbol) << endl;
                            break;
    }
}

void gen_function(ofstream& out, astree* node, int depth) {
    //gen function type and name
    astree* return_type = node->children[0];
    astree* name = return_type->children[0];
    name->symbol = TOK_FUNCTION;
    out << convert_type(return_type, return_type->lexinfo)
        << " " << mangle_name(name, *name->lexinfo)
        << " (" << endl;

    //gen function parameter list
    depth++;
    astree* paramlist = node->children[1];
    for (size_t i = 0; i < paramlist->children.size(); i++) {
        astree* param_type = paramlist->children[i];
        astree* param = param_type->children[0];
        out << string(depth * 3, ' ')
            << convert_type(param_type, param_type->lexinfo)
            << " " << mangle_name(param, *param->lexinfo);
        if (i+1 != paramlist->children.size())
            out << "," << endl;
    }
    out << ")" << endl
        << "{" << endl;

    //gen function block
    astree* block = node->children[2];
    gen_oil_stuff(out, block, depth, nullptr);

    out << "}" << endl;
}

void gen_oil(ofstream& out, astree* root, int depth) {
    //gen all structs
    for (astree* child: root->children) {
        if (child->symbol == TOK_STRUCT)
            gen_struct(out, child, depth);
    }
    //gen all string consts
    for (astree* child: root->children) {
        gen_strconst(out, child, depth);
    }
    //gen all global/top-level var decls
    for (astree* child: root->children) {
        if (child->symbol == '=') {
            astree* type = child->children[0];
            astree* declid = type->children[0];
            if (declid->symbol == TOK_DECLID
                    && type->symbol != TOK_STRING) {
                out << convert_type(type, nullptr) << " "
                    << mangle_name(type, *declid->lexinfo)
                    << ";" << endl;
            }
        }
    }
    //gen all functions
    for (astree* child: root->children) {
        if (child->symbol == TOK_FUNCTION) {
            gen_function(out, child, depth);
        }
    }
}
