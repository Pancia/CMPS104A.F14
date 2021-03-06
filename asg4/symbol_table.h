#ifndef __SYMBOL_TABLE_H__
#define __SYMBOL_TABLE_H__

#include <iostream>
#include <bitset>
#include <string>
#include <unordered_map>
#include <vector>

using namespace std;

enum { ATTR_void, ATTR_bool, ATTR_char, ATTR_int, ATTR_null,
       ATTR_string, ATTR_struct, ATTR_array, ATTR_function,
       ATTR_variable, ATTR_field, ATTR_typeid, ATTR_param,
       ATTR_lval, ATTR_const, ATTR_vreg, ATTR_vaddr,
       ATTR_bitset_size,
};
using attr_bitset = bitset<ATTR_bitset_size>;

struct symbol;
using symbol_table = unordered_map<const string*,symbol*>;
using symbol_entry = pair<const string*,symbol*>;

struct symbol {
   attr_bitset attributes;
   symbol_table* fields;
   size_t filenr, linenr, offset;
   size_t blocknr;
   vector<symbol*>* parameters;
};

#include "lyutils.h"

void parse_tree(astree* node);
void write_attributes(ofstream& out, attr_bitset attr,
                      const string* field_name, const string* struct_name);
void write_tree(ofstream& out, astree* node, int depth);

#endif
