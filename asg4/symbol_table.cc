#include "symbol_table.h"

using namespace std;

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
        << ":fnr " << node->second->filenr  << ", "
        << ":lnr " << node->second->linenr  << ", "
        << ":off " << node->second->offset  << ", "
        << ":bnr " << node->second->blocknr << ", "
        << ":atr ";
    write_attributes(out, node->second->attributes);
    out << "}";
}

void write_node (ofstream& out, astree* node, int depth){
    if (node == nullptr) return;
    out << std::string(depth * 3, ' ') << node->lexinfo->c_str()
        << " ("
        << node->filenr << ":"
        << node->linenr << "."
        << node->offset
        << ")"
        << " {" << node->block_number << "} ";
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
symbol_table* struct_stack = new symbol_table();

int blocknr = 0;

symbol* new_symbol(astree* node, int blocknr) {
    symbol* s = new symbol();

    //filenr/linenr/offset point to first instance
    //of the thing node represents...
    s->filenr = node->filenr;
    s->blocknr = blocknr;
    s->linenr = node->linenr;
    s->attributes = get_node_attr(node);
    s->parameters = nullptr;
    s->fields = nullptr;
    s->offset = node->offset;

    return s;
}

void parse_node (ofstream& out, astree* node, int depth){
    if (node == nullptr) return;

    if (symbol_stack[blocknr] == nullptr) {
        vector<symbol_table*>::iterator it = symbol_stack.begin() + blocknr;
        symbol_stack.insert(it, new symbol_table());
    }

    //if (node->symbol == TOK_BLOCK)
    //    return;

    symbol* s = new_symbol(node, blocknr);

    symbol_stack[blocknr]->insert(symbol_entry(node->lexinfo, s));
    node->node = symbol_stack[blocknr];
    node->block_number = s->blocknr;

    write_node(out, node, depth);
}

void parse_tree(ofstream& out, astree* node, int depth){
    //parse_node might need to be done after the for loop
    //so as to do a DFS, not a BFS...
    parse_node (out, node, depth);
    if (node->symbol == TOK_BLOCK) {
        blocknr++;
        out << "BLOCK++ " << blocknr << endl;
    }
    for(size_t child = 0; child < node->children.size(); ++child) {
        parse_tree(out, node->children[child], depth+1);
    }
    if (node->symbol == TOK_BLOCK) {
        //symbol_stack[blocknr] = nullptr;
        blocknr--;
        out << "BLOCK--" << blocknr << endl;
    }
}