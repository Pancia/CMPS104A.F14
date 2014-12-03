#ifndef __ASTREE_H__
#define __ASTREE_H__

#include <string>
#include <vector>

#include "auxlib.h"

struct astree;
#include "symbol_table.h"

using namespace std;

struct astree {
    int symbol;
    size_t filenr;
    size_t linenr;
    size_t offset;
    const string* lexinfo;
    vector<astree*> children;
    attr_bitset attributes;
    int block_number;
    symbol_table* node;
    const string* struct_name;
};

astree* new_astree(int symbol, int filenr, int linenr, int offset,
                    const char* lexinfo);
astree* adopt1(astree* root, astree* child);
astree* adopt2(astree* root, astree* left, astree* right);
astree* adopt3(astree* root, astree* left, astree* middle,
               astree* right);
astree* adopt4(astree* root, astree* c1, astree* c2, astree* c3,
               astree* c4);
astree* adopt1sym(astree* root, astree* child, int symbol);
astree* adopt2sym(astree* root, astree* child1, astree* child2,
                  int symbol);
astree* upd_tree_symbol(astree* tree, int symbol);
void dump_astree(FILE* outfile, astree* root);
void write_astree(ofstream& out, astree* root);
void yyprint(FILE* outfile, unsigned short toknum, astree* yyvaluep);
void free_ast(astree* tree);
void free_ast2(astree* tree1, astree* tree2);
void free_ast3(astree* tree1, astree* tree2, astree* tree3);

#endif

