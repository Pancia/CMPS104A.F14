#include <iostream>

#include <assert.h>
#include <inttypes.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "stringset.h"
#include "lyutils.h"

astree* new_astree(int symbol, int filenr, int linenr, int offset,
                   const char* lexinfo) {
    astree* tree = new astree();
    tree->symbol = symbol;
    tree->filenr = filenr;
    tree->linenr = linenr;
    tree->offset = offset;
    tree->attributes = 0;
    tree->block_number = 0;
    tree->node = NULL;
    tree->lexinfo = intern_stringset(lexinfo);
    char symbol_name[64];
    memset(&symbol_name[0], 0, sizeof(symbol_name));
    string yytname = get_yytname(tree->symbol);
    if(yytname == "$undefined") {
        char s[16];
        memset(&s[0], 0, sizeof(s));
        sprintf(s, "\'%s\'", tree->lexinfo->c_str());
        strcat(symbol_name, s);
    } else {
        strcat(symbol_name, get_yytname(tree->symbol));
    }
    char buf[128];
    memset(&buf[0], 0, sizeof(buf));
    sprintf(buf, "  %2d  %02d.%03d  %3d  %-16s (%s)\n",
            tree->filenr, tree->linenr, tree->offset,
            tree->symbol, symbol_name,
            tree->lexinfo->c_str());
    tok_file << buf;
    DEBUGF('f', "astree %p->{%d:%d.%d: %s: \"%s\"}\n",
            tree, tree->filenr, tree->linenr, tree->offset,
            get_yytname(tree->symbol), tree->lexinfo->c_str());
    return tree;
}

astree* adopt1(astree* root, astree* child) {
    root->children.push_back(child);
    DEBUGF('a', "%p(%s) adopting %p(%s)\n",
            root, root->lexinfo->c_str(),
            child, child->lexinfo->c_str());
    return root;
}

astree* adopt2(astree* root, astree* left, astree* right) {
    adopt1(root, left);
    adopt1(root, right);
    return root;
}

astree* adopt3(astree* root, astree* left, astree* middle,
               astree* right) {
    adopt1(root, left);
    adopt1(root, middle);
    adopt1(root, right);
    return root;
}

astree* adopt4(astree* root, astree* c1, astree* c2,
               astree* c3, astree* c4) {
    adopt1(root, c1);
    adopt1(root, c2);
    adopt1(root, c3);
    adopt1(root, c4);
    return root;
}

astree* adopt1sym(astree* root, astree* child, int symbol) {
    root = adopt1(root, child);
    root->symbol = symbol;
    return root;
}

astree* adopt2sym(astree* root, astree* child1, astree* child2,
                  int symbol) {
    root = adopt1(root, child1);
    root = adopt1(root, child2);
    root->symbol = symbol;
    return root;
}

astree* upd_tree_symbol(astree* tree, int symbol) {
    tree->symbol = symbol;
    return tree;
}

static void dump_node(FILE* outfile, astree* node) {
    fprintf(outfile, "%p->{%s(%d) %ld:%ld.%03ld \"%s\" [",
            node, get_yytname(node->symbol), node->symbol,
            node->filenr, node->linenr, node->offset,
            node->lexinfo->c_str());
    bool need_space = false;
    for(size_t child = 0; child < node->children.size(); ++child) {
        if(need_space) fprintf(outfile, " ");
        need_space = true;
        fprintf(outfile, "%p", node->children.at(child));
    }
    fprintf(outfile, "]}");
}

static void dump_astree_rec(FILE* outfile, astree* root, int depth) {
    if(root == NULL) return;
    fprintf(outfile, "%*s%s ", depth * 3, "", root->lexinfo->c_str());
    dump_node(outfile, root);
    fprintf(outfile, "\n");
    for(size_t child = 0; child < root->children.size(); ++child) {
        dump_astree_rec(outfile, root->children[child], depth + 1);
    }
}

void dump_astree(FILE* outfile, astree* root) {
    dump_astree_rec(outfile, root, 0);
    fflush(NULL);
}

static void write_node(ofstream& out, astree* node) {
    out << get_yytname(node->symbol) << " "
        << node->filenr << ":" << node->linenr << "." << node->offset;
    bool need_space = false;
    //wtf is this for?
    /*for(size_t child = 0; child < node->children.size(); ++child) {
        if(need_space)
            out << " ";
        need_space = true;
    }*/
}

static void write_astree_rec(ofstream& out, astree* root, int depth) {
    if(root == NULL) return;
    out << std::string(depth * 3, ' ') << root->lexinfo->c_str() << " ";
    write_node(out, root);
    out << endl;
    for(size_t child = 0; child < root->children.size(); ++child) {
        write_astree_rec(out, root->children[child], depth + 1);
    }
}

void write_astree(ofstream& out, astree* root) {
    write_astree_rec(out, root, 0);
}

void yyprint(FILE* outfile, unsigned short toknum, astree* yyvaluep) {
    DEBUGF('f', "toknum = %d, yyvaluep = %p\n", toknum, yyvaluep);
    if(is_defined_token(toknum)) {
        dump_node(outfile, yyvaluep);
    }else {
        fprintf(outfile, "%s(%d)\n", get_yytname(toknum), toknum);
    }
    fflush(NULL);
}

void free_ast(astree* root) {
    while(not root->children.empty()) {
        astree* child = root->children.back();
        root->children.pop_back();
        free_ast(child);
    }
    DEBUGF('f', "free [%X]-> %d:%d.%d: %s: \"%s\")\n",
            (uintptr_t) root, root->filenr, root->linenr, root->offset,
            get_yytname(root->symbol), root->lexinfo->c_str());
    delete root;
}

void free_ast2(astree* tree1, astree* tree2) {
    free_ast(tree1);
    free_ast(tree2);
}

void free_ast3(astree* tree1, astree* tree2, astree* tree3) {
    free_ast(tree1);
    free_ast(tree2);
    free_ast(tree3);
}

