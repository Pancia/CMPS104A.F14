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
            case 'l': cout << "scan_opts: l\n";                     break;
            case 'y': cout << "scan_opts: y\n";                     break;
            default:  errprintf("%:bad option (%c)\n", optopt);     break;
        }
    }
    if (optind > argc) {
        errprintf("Usage: %s [-ly] [filename]\n", get_execname());
        exit(get_exitstatus());
    }
    return optind;
}

const string CPP = "/usr/bin/cpp";
const size_t LINESIZE = 1024;

// Chomp the last character from a buffer if it is delim.
void chomp (char* string, char delim) {
    size_t len = strlen (string);
    if (len == 0) return;
    char* nlpos = string + len - 1;
    if (*nlpos == delim) *nlpos = '\0';
}

// Run cpp against the lines of the file.
void cpplines (FILE* pipe, char* filename) {
    int linenr = 1;
    char inputname[LINESIZE];
    strcpy(inputname, filename);
    for (;;) {
        char buffer[LINESIZE];
        char* fgets_rc = fgets(buffer, LINESIZE, pipe);
        if (fgets_rc == NULL) break;
        chomp (buffer, '\n');
        DEBUGF('p', "%s:line %d: [%s]\n", filename, linenr, buffer);
        // http://gcc.gnu.org/onlinedocs/cpp/Preprocessor-Output.html
        int sscanf_rc = sscanf(buffer, "# %d \"%[^\"]\"",
                                &linenr, filename);
        if (sscanf_rc == 2) {
            DEBUGF('f', "DIRECTIVE: line %d file \"%s\"\n", linenr, filename);
            continue;
        }
        char* savepos = NULL;
        char* bufptr = buffer;
        for (int tokenct = 1;; ++tokenct) {
            char* token = strtok_r(bufptr, " \t\n", &savepos);
            bufptr = NULL;
            if (token == NULL) break;
            DEBUGF('f', "token %d.%d: [%s]\n",
                        linenr, tokenct, token);
            const string* str = intern_stringset(token);
            DEBUGF('f', "intern(", token, ") returned ", str, "\n");
        }
        ++linenr;
    }
}

int main(int argc, char** argv) {
    set_execname (argv[0]);
    int new_argc = scan_opts(argc, argv);
    
    for (int argi = new_argc; argi < argc; ++argi) {
        char* filename = argv[argi];
        string command = CPP + " " + filename;
        DEBUGF('f', "command=\"%s\"\n", command.c_str());
        FILE* pipe = popen(command.c_str(), "r");
        if (pipe == NULL) {
            syserrprintf(command.c_str());
        } else {
            cpplines(pipe, filename);
            int pclose_rc = pclose (pipe);
            eprint_status(command.c_str(), pclose_rc);
        }
    }
    dump_stringset(cout);
    
    return get_exitstatus();
}
