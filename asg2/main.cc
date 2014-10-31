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

int scan_opts(int argc, char** argv) {
    int option;
    opterr = 0;
    for(;;) {
        option = getopt(argc, argv, "ly@:");
        if (option == EOF) break;
        switch (option) {
            case '@': set_debugflags(optarg);                      break;
            case 'l': yy_flex_debug = 1;                           break;
            case 'y': yydebug = 1;                                 break;
            default:  errprintf("%:bad option (%c)\n", optopt);    break;
        }
    }
    if (optind > argc) {
        errprintf("Usage: %s [-ly] [filename]\n", get_execname());
        exit(get_exitstatus());
    }
    return optind;
}

const string CPP = "/usr/bin/cpp";

void scan(char* filename) {
    DEBUGF ('m', "filename=%s", filename);
    for (;;) {
	int token = yylex();
	if (token == YYEOF)
	    return;
	DEBUGF ('m', "token=%d", token);   	 
    }
}

/*
   # 16 "foobar.oc"
      2  16.003  264  TOK_KW_RETURN   (return)
      2  16.010   61  '='             (=)
      2  20.008  258  TOK_IDENT       (hello)
      2  20.010  271  TOK_LIT_INT     (1234)
      2  25.002  123  '{'             ({)
      2  26.008  272  TOK_LIT_STRING  ("beep")
*/

int main(int argc, char** argv) {
    set_execname (argv[0]);
    int new_argc = scan_opts(argc, argv);
    
    for (int argi = new_argc; argi < argc; ++argi) {
        char* filename = argv[argi];
        string command = CPP + " " + filename;
        DEBUGF('f', "command=\"%s\"\n", command.c_str());
        yyin = popen(command.c_str(), "r");
        if (yyin == NULL) {
            syserrprintf(command.c_str());
        } else {
            scan(filename);
        }
    }
    //dump_stringset(cout);
    
    return get_exitstatus();
}
