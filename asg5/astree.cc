#include <iostream>

#include <assert.h>
#include <inttypes.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "stringset.h"
#include "lyutils.h"

/*FUNCTION: new_astree
PURPOSE: Makes a new astree* with the parameters that are passed in
*/
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

/*FUNCTION: adopt1
PURPOSE: root adopts child.
*/
astree* adopt1(astree* root, astree* child) {
    root->children.push_back(child);
    DEBUGF('a', "%p(%s) adopting %p(%s)\n",
            root, root->lexinfo->c_str(),
            child, child->lexinfo->c_str());
    return root;
}

/*FUNCTION: adopt2
PURPOSE: root adopts left and right
*/
astree* adopt2(astree* root, astree* left, astree* right) {
    adopt1(root, left);
    adopt1(root, right);
    return root;
}

/*FUNCTION: adopt3
PURPOSE: root adopts left, middle, and right
*/
astree* adopt3(astree* root, astree* left, astree* middle,
               astree* right) {
    adopt1(root, left);
    adopt1(root, middle);
    adopt1(root, right);
    return root;
}

/*FUNCTION: adopt4
PURPOSE: root adopts c1, c2, c3, and c4
*/
astree* adopt4(astree* root, astree* c1, astree* c2,
               astree* c3, astree* c4) {
    adopt1(root, c1);
    adopt1(root, c2);
    adopt1(root, c3);
    adopt1(root, c4);
    return root;
}

/*FUNCTION: adopt1sym
PURPOSE: root adopts child and sets root's symbol to symbol.
*/
astree* adopt1sym(astree* root, astree* child, int symbol) {
    root = adopt1(root, child);
    root->symbol = symbol;
    return root;
}

/*FUNCTION: adopt2sym
PURPOSE: root adopts child1 and child2 and sets root's symbol to symbol.
*/
astree* adopt2sym(astree* root, astree* child1, astree* child2,
                  int symbol) {
    root = adopt1(root, child1);
    root = adopt1(root, child2);
    root->symbol = symbol;
    return root;
}

/*FUNCTION: upd_tree_symbol
PURPOSE: tree->symbol is updated to symbol
*/
astree* upd_tree_symbol(astree* tree, int symbol) {
    tree->symbol = symbol;
    return tree;
}

/*FUNCTION: dump_node
PURPOSE: Prints out the node
*/
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

/*FUNCTION: upd_astree_rec
PURPOSE: prints out the astree recursively
*/
static void dump_astree_rec(FILE* outfile, astree* root, int depth) {
    if(root == NULL) return;
    fprintf(outfile, "%*s%s ", depth * 3, "", root->lexinfo->c_str());
    dump_node(outfile, root);
    fprintf(outfile, "\n");
    for(size_t child = 0; child < root->children.size(); ++child) {
        dump_astree_rec(outfile, root->children[child], depth + 1);
    }
}

/*FUNCTION: dump_astree
PURPOSE: prints out the astree
*/
void dump_astree(FILE* outfile, astree* root) {
    dump_astree_rec(outfile, root, 0);
    fflush(NULL);
}

/*FUNCTION: print_symbol_table
PURPOSE: prints the symbol table
*/
void print_symbol_table(ofstream& out, symbol_table foo) {
    out << "{";
    for (const auto& i: foo) {
        out << *i.first << ": " << i.second << ", ";
    }
    out << "}" << endl;
}

const string* STRUCT_NAME = nullptr;

/*FUNCTION: write_node
PURPOSE: writes the node to the ofstream& out
*/
static void write_node(ofstream& out, astree* node, int depth) {
    if (node->node == nullptr)
        return;
    //print_symbol_table(out, *node->node);
    const auto& auto_node = node->node->find(node->lexinfo);
    out << std::string(depth * 3, ' ');
    out << get_yytname(node->symbol);
    if (node->symbol == TOK_STRUCT) {
        out << " " << node->children[0]->lexinfo->c_str() << " ";
    } else {
        out << " " << node->lexinfo->c_str() << " ";
    }
    out << "(" << node->filenr << ":"
        << node->linenr << "."
        << node->offset << ") "
        << "{" << node->block_number << "} ";

    const string* field_name = nullptr;
    if (node->attributes[ATTR_field]) {
        assert(STRUCT_NAME != nullptr);
        field_name = STRUCT_NAME;
    }
    write_attributes(out, node->attributes, field_name, STRUCT_NAME);

    if (auto_node != node->node->end()) {
        out << "(" << auto_node->second->filenr << ":"
            << auto_node->second->linenr << "."
            << auto_node->second->offset << ") ";
    }
    out << endl;
}

/*FUNCTION: write_astree_rec
PURPOSE: writes astree recursively
*/
static void write_astree_rec(ofstream& out, astree* root, int depth) {
    if (root == NULL) return;
    if (root->symbol == TOK_STRUCT) {STRUCT_NAME = root->children[0]->lexinfo;}
    write_node(out, root, depth);
    for (size_t child = 0; child < root->children.size(); ++child) {
        write_astree_rec(out, root->children[child], depth + 1);
    }
    if (root->symbol == TOK_STRUCT) {STRUCT_NAME = nullptr;}
}

/*FUNCTION: write_astree
PURPOSE: writes the astree
*/
void write_astree(ofstream& out, astree* root) {
    write_astree_rec(out, root, 0);
}

/*FUNCTION: yyprint
PURPOSE: print the token or node
*/
void yyprint(FILE* outfile, unsigned short toknum, astree* yyvaluep) {
    DEBUGF('f', "toknum = %d, yyvaluep = %p\n", toknum, yyvaluep);
    if (is_defined_token(toknum)) {
        dump_node(outfile, yyvaluep);
    } else {
        fprintf(outfile, "%s(%d)\n", get_yytname(toknum), toknum);
    }
    fflush(NULL);
}

/*FUNCTION: free_ast
PURPOSE: frees the astree root
*/
void free_ast(astree* root) {
    while (not root->children.empty()) {
        astree* child = root->children.back();
        root->children.pop_back();
        free_ast(child);
    }
    DEBUGF('f', "free [%X]-> %d:%d.%d: %s: \"%s\")\n",
            (uintptr_t) root, root->filenr, root->linenr, root->offset,
            get_yytname(root->symbol), root->lexinfo->c_str());
    delete root;
}

/*FUNCTION: free_ast2
PURPOSE: Frees 2 astrees
*/
void free_ast2(astree* tree1, astree* tree2) {
    free_ast(tree1);
    free_ast(tree2);
}

/*FUNCTION: free_ast3
PURPOSE: frees 3 astrees
*/
void free_ast3(astree* tree1, astree* tree2, astree* tree3) {
    free_ast(tree1);
    free_ast(tree2);
    free_ast(tree3);
}

