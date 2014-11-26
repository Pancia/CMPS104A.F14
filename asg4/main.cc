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
#include "stringset.h"

using namespace std;
ofstream tok_file;

int scan_opts(int argc, char** argv) {
    int option;
    opterr = 0;;;
    for(;;) {
        option = getopt(argc, argv, "ly@:");
        if(option == EOF) break;
        switch(option) {
            case '@': set_debugflags(optarg);
                      break;
            case 'l': yy_flex_debug = 1;
                      break;
            case 'y': yydebug = 1;
                      break;
            default:  errprintf("%:bad option(%c)\n", optopt);
                      break;
        }
    }
    if(optind > argc) {
        errprintf("Usage: %s [-ly] [filename]\n", get_execname());
        exit(get_exitstatus());
    }
    return optind;
}

string make_filename(char* filename, string ending){
    string delimiter =".";
    size_t pos = 0;
    string str_fname = filename;
    pos = str_fname.find(delimiter);
    string fname = str_fname.substr(0, pos);
    DEBUGF('m', "filename=%s", filename);
    return (fname + ending);
}

const string CPP = "/usr/bin/cpp";

void scan(char* filename) {
    tok_file.open(make_filename(filename, ".tok"), ios::out);
    assert(tok_file.is_open());
    for(;;) {
        int token = yylex();
        if(token == YYEOF)
            return;
        DEBUGF('m', "token=%d", token);
    }
    tok_file.close();
}

string yyin_cpp_popen(char* filename) {
    string yyin_cpp_command;

    yyin_cpp_command = CPP;
    yyin_cpp_command += " ";
    yyin_cpp_command += filename;
    yyin = popen(yyin_cpp_command.c_str(), "r");
    if(yyin == nullptr) {
        syserrprintf(yyin_cpp_command.c_str());
        exit(get_exitstatus());
    }

    return yyin_cpp_command;
}

void yyin_cpp_pclose(string filename) {
    int pclose_rc = pclose(yyin);
    eprint_status(filename.c_str(), pclose_rc);
    if(pclose_rc != 0) set_exitstatus(EXIT_FAILURE);
}

void write_attributes(ofstream& out, attr_bitset attr) {
    out << "attr@{";
    if (attr[ATTR_void] == 1) {
        out << "void ";
    }
    if (attr[ATTR_bool] == 1) {
        out << "bool ";
    }
    if (attr[ATTR_char] == 1) {
        out << "char ";
    }
    if (attr[ATTR_int] == 1) {
        out << "int ";
    }
    if (attr[ATTR_null] == 1) {
        out << "null ";
    }
    if (attr[ATTR_string] == 1) {
        out << "string ";
    }
    if (attr[ATTR_struct] == 1) {
        out << "struct ";
    }
    if (attr[ATTR_array] == 1) {
        out << "array ";
    }
    if (attr[ATTR_function] == 1) {
        out << "function ";
    }
    if (attr[ATTR_variable] == 1) {
        out << "variable ";
    }
    if (attr[ATTR_field] == 1) {
        out << "field ";
    }
    if (attr[ATTR_typeid] == 1) {
        out << "typeid ";
    }
    if (attr[ATTR_param] == 1) {
        out << "param ";
    }
    if (attr[ATTR_lval] == 1) {
        out << "lval ";
    }
    if (attr[ATTR_const] == 1) {
        out << "const ";
    }
    if (attr[ATTR_vreg] == 1) {
        out << "vreg ";
    }
    if (attr[ATTR_vaddr] == 1) {
        out << "vaddr ";
    }
    out << "}";
}

void write_symbol(ofstream& out, symbol_table* sym_table, const string* s) {
    const auto& node = sym_table->find(s);
    assert(node != sym_table->end());
    out << "sym@{"
        << ":fnr " << node->second->filenr << ", "
        << ":lnr " << node->second->linenr << ", "
        << ":off " << node->second->offset << ", "
        << ":bnr" << node->second->blocknr << ", "
        << ":atr ";
    write_attributes(out, node->second->attributes);
    out << "}";
}

void write_node (ofstream& out, astree* node, int depth){
    if (node == nullptr) return;
    out << std::string(depth * 3, ' ') << node->lexinfo->c_str() << " ("
        << node->filenr << ":" << node->linenr << "." << node->offset
        << ") {" << node->block_number << "} ";
    write_symbol(out, node->node, node->lexinfo);
    out << endl;
}

attr_bitset get_node_attr(astree* node) {
    attr_bitset attr;
    switch (node->symbol) {
        case TOK_VOID:     attr.set(ATTR_void);
                           break;
        case TOK_BOOL:     attr.set(ATTR_bool);
                           attr.set(ATTR_const);
                           break;
        case TOK_CHAR:     attr.set(ATTR_char);
                           attr.set(ATTR_const);
                           break;
        case TOK_INT:      attr.set(ATTR_int);
                           attr.set(ATTR_const);
                           break;
        case TOK_NIL:      attr.set(ATTR_null);
                           attr.set(ATTR_const);
                           break;
        case TOK_STRING:   attr.set(ATTR_string);
                           attr.set(ATTR_const);
                           break;
        case TOK_STRUCT:   attr.set(ATTR_struct);
                           break;
        case TOK_BRKKRB:   attr.set(ATTR_array);
                           break;
        case TOK_TYPEID:   attr.set(ATTR_typeid);
                           break;
        case TOK_FUNCTION: attr.set(ATTR_function);
                           break;
        case TOK_FIELD:    attr.set(ATTR_field);
                           break;
        case TOK_IDENT:    attr.set(ATTR_lval);
                           break;
        case TOK_DECLID:   attr.set(ATTR_lval);
                           break;
        default:
                       attr.set(ATTR_null);
                       break;
    }
    return attr;
}

vector<symbol_table*> symbol_stack(16, nullptr);
int this_block = 0;

symbol* new_symbol(astree* node) {
    symbol* s = new symbol();

    s->filenr = node->filenr;
    s->blocknr = this_block;
    s->linenr = node->linenr;
    s->attributes = get_node_attr(node);
    s->parameters = nullptr;
    s->fields = nullptr;
    s->offset = node->offset;

    return s;
}

void parse_node (ofstream& out, astree* node, int depth){
    if (node == nullptr) return;

    if (symbol_stack[this_block] == nullptr) {
        vector<symbol_table*>::iterator it = symbol_stack.begin() + this_block;
        symbol_stack.insert(it, new symbol_table());
    }

    //if (node->symbol == TOK_BLOCK)
    //    return;

    symbol* s = new_symbol(node);

    symbol_stack[this_block]->insert(symbol_entry(node->lexinfo, s));
    node->node = symbol_stack[this_block];
    node->block_number = s->blocknr;

    write_node(out, node, depth);
}

void parse_tree(ofstream& out, astree* node, int depth){
    parse_node (out, node, depth);
    if (node->symbol == TOK_BLOCK) {
        this_block++;
        out << "BLOCK++ " << this_block << endl;
    }
    for(size_t child = 0; child < node->children.size(); ++child) {
        parse_tree(out, node->children[child], depth+1);
    }
    if (node->symbol == TOK_BLOCK) {
        //symbol_stack[this_block] = nullptr;
        this_block--;
        out << "BLOCK--" << this_block << endl;
    }
}

int main(int argc, char** argv) {
    set_execname(argv[0]);
    int parsecode = 0;
    int new_argc = scan_opts(argc, argv);
    //scanner_setecho(want_echo());

    for(int argi = new_argc; argi < argc; ++argi) {
        char* filename = argv[argi];
        string command = CPP + " " + filename;
        DEBUGF('f', "command=\"%s\"\n", command.c_str());
        yyin = popen(command.c_str(), "r");
        if(yyin == nullptr) {
            syserrprintf(command.c_str());
        } else {
            scan(filename);
        }
        ofstream str_file;
        str_file.open(make_filename(filename, ".str"), ios::out);
        dump_stringset(str_file);
        str_file.close();

        yyin_cpp_popen(filename);
        parsecode = yyparse();
        yyin_cpp_pclose(filename);

        if(parsecode) {
            errprintf("%:parse failed(%d)\n", parsecode);
        } else {
            DEBUGSTMT('a', dump_astree(stderr, yyparse_astree); );
            //emit_sm_code(yyparse_astree);
            ofstream ast_file;
            ast_file.open(make_filename(filename, ".ast"), ios::out);
            write_astree(ast_file, yyparse_astree);
            ast_file.close();
        }
        ofstream sym_file;
        sym_file.open(make_filename(filename, ".sym"), ios::out);
        parse_tree(sym_file, yyparse_astree, 0);
        sym_file.close();
    }
    return get_exitstatus();
}

