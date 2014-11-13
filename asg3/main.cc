#include <iostream>
#include <string>
#include <vector>

#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "auxlib.h"
#include "lyutils.h"
#include "stringset.h"

using namespace std;
ofstream tok_file;

int scan_opts(int argc, char** argv) {
    int option;
    opterr = 0;
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

const string CPP = "/usr/bin/cpp";

void scan(char* filename) {
    string delimiter =".";
    size_t pos = 0;
    string str_fname = filename;
    pos = str_fname.find(delimiter);
    string fname = str_fname.substr(0, pos);
    DEBUGF('m', "filename=%s", filename);
    tok_file.open(fname + ".tok", ios::out);
    assert(tok_file.is_open());
    for(;;) {
        int token = yylex();
        if(token == YYEOF)
            return;
        DEBUGF('m', "token=%d", token);
    }
    tok_file.close();
}

/*
# 16 "foobar.oc"
2  16.003  264  TOK_KW_RETURN  (return)
2  16.010   61  '='            (=)
2  20.008  258  TOK_IDENT      (hello)
2  20.010  271  TOK_LIT_INT    (1234)
2  25.002  123  '{'            ({)
2  26.008  272  TOK_LIT_STRING ("beep")
*/

int main(int argc, char** argv) {
    set_execname(argv[0]);
    int parsecode = 0;
    int new_argc = scan_opts(argc, argv);
    scanner_setecho (want_echo());
    parsecode = yyparse();

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
        string delimiter =".";
        size_t pos = 0;
        string str_fname = filename;
        pos = str_fname.find(delimiter);
        string fname = str_fname.substr(0, pos);
        str_file.open(fname + ".str", ios::out);
        dump_stringset(str_file);
        str_file.close();

        if (parsecode) {
            errprintf ("%:parse failed (%d)\n", parsecode);
        } else {
            DEBUGSTMT ('a', dump_astree (stderr, yyparse_astree); );
            emit_sm_code (yyparse_astree);
        }
    }
    return get_exitstatus();
}
