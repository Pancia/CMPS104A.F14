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
    if(yyin == NULL) {
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

void write_node (ofstream& out, astree* node, int depth){
    if (node == NULL) return;
    out << std::string(depth * 3, ' ') << node->lexinfo->c_str() << " (" 
        << node->filenr << ":" << node->linenr << "." << node->offset
        << ") {" << node->block_number << "} " << node->attributes << 
        node->node << endl;
}

void parse_node (ofstream& out, astree* node, int depth){
    if (node == NULL) return;

    int next_block = 1;
    if (symbol_stack[next_block-1] == NULL) {
        symbol_stack.push_back(new symbol_table());
    }
    symbol* s = new symbol();
    s->filenr = node->filenr;
    s->blocknr = next_block-1;
    s->linenr = node->linenr;
    s->attributes = NULL;
    s->parameters = NULL;
    s->fields = NULL;
    s->offset = node->offset;

    char *q = new char[64];
    strcpy(q, node->lexinfo->c_str());
    string* str = &string(q);

    symbol_stack[next_block-1]->insert (symbol_entry(str, s));
    node->node = symbol_stack[next_block-1];

    write_node(out, node, depth);
}

void parse_tree(ofstream& out, astree* root, int depth){
    parse_node (out, root, depth);
    for(size_t child = 0; child < root->children.size(); ++child) {
        parse_tree(out, root->children[child], depth+1);
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
        if(yyin == NULL) {
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
