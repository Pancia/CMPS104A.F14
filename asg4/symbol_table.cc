#include "symbol_table.h"
#include <assert.h>

using namespace std;

template <typename T>
void output(ofstream& out, T arg) {
    out << arg;
    out.flush();
}

vector<symbol_table*> symbol_stack(16, nullptr);
symbol_table* struct_stack = new symbol_table();

int blocknr = 0;

string get_attributes(attr_bitset attr, const string* field_name,
                      const string* struct_name) {
    string out;
    if (attr[ATTR_struct] == 1 && struct_name != nullptr) {
        out += "struct \"";
        out += *struct_name;
        out += "\" ";
    } else if (attr[ATTR_struct] == 1) {
        out += "struct ";
    }
    if (attr[ATTR_field] == 1 && field_name != nullptr) {
        out += "field {";
        out += *field_name;
        out += "} ";
    } else if (attr[ATTR_field] == 1) {
        out += "field ";
    }
    if (attr[ATTR_void] == 1) {
        out += "void ";
    }
    if (attr[ATTR_bool] == 1) {
        out += "bool ";
    }
    if (attr[ATTR_char] == 1) {
        out += "char ";
    }
    if (attr[ATTR_int] == 1) {
        out += "int ";
    }
    if (attr[ATTR_null] == 1) {
        out += "null ";
    }
    if (attr[ATTR_string] == 1) {
        out += "string ";
    }
    if (attr[ATTR_array] == 1) {
        out += "array ";
    }
    if (attr[ATTR_function] == 1) {
        out += "function ";
    }
    if (attr[ATTR_variable] == 1) {
        out += "variable ";
    }
    if (attr[ATTR_typeid] == 1) {
        out += "typeid ";
    }
    if (attr[ATTR_param] == 1) {
        out += "param ";
    }
    if (attr[ATTR_lval] == 1) {
        out += "lval ";
    }
    if (attr[ATTR_const] == 1) {
        out += "const ";
    }
    if (attr[ATTR_vreg] == 1) {
        out += "vreg ";
    }
    if (attr[ATTR_vaddr] == 1) {
        out += "vaddr ";
    }
    return out;
}

void write_attributes(ofstream& out, attr_bitset attr,
                      const string* field_name, const string* struct_name) {
    output(out, get_attributes(attr, field_name, struct_name));
}

void write_symbol(ofstream& out, astree* node, symbol_table* sym_table,
                  const string* s) {
    const auto& auto_sym = sym_table->find(s);
    assert(auto_sym != sym_table->end());
    symbol* sym = auto_sym->second;
    out << "(" << sym->filenr  << "."
        << sym->linenr  << "."
        << sym->offset  << ") {"
        << sym->blocknr << "} ";
    const string* struct_name = nullptr;
    if (sym->attributes[ATTR_struct]) {
        if (sym->attributes[ATTR_function] || sym->attributes[ATTR_array]) {
            if (node->symbol == TOK_BRKKRB) {
                struct_name = node->children[0]->lexinfo;
            }
            else {

            }
        } else {
            const auto& auto_struct = struct_stack->find(node->lexinfo);
            assert(auto_struct != struct_stack->end());
            assert(node->lexinfo != nullptr);
            struct_name = node->lexinfo;
        }
    }
    write_attributes(out, sym->attributes, nullptr, struct_name);
}

void write_node(ofstream& out, astree* node, int depth){
    if (node == nullptr)
        return;
    out << std::string(depth * 3, ' ') << node->lexinfo->c_str()
        << " ("
        << node->filenr << ":"
        << node->linenr << "."
        << node->offset
        << ")"
        << " {" << node->block_number << "} ";
    write_symbol(out, node, node->node, node->lexinfo);
    out << endl;
}

void write_struct_field(ofstream& out, astree* node, int depth,
                        const string* name) {
    out << std::string(depth * 3, ' ')
        << node->children[0]->lexinfo->c_str()
        << " ("
        << node->filenr << ":"
        << node->linenr << "."
        << node->offset
        << ") ";
    write_attributes(out, node->attributes, name, nullptr);
    out << endl;
}

void write_struct(ofstream& out, astree* node, int depth) {
    out << std::string(depth * 3, ' ') << *node->children[0]->lexinfo
        << " ("
        << node->children[0]->filenr << ":"
        << node->children[0]->linenr << "."
        << node->children[0]->offset
        << ")"
        << " {" << node->children[0]->block_number << "} ";
    write_attributes(out, node->attributes, nullptr, node->children[0]->lexinfo);
    out << endl;

    for(size_t child = 1; child < node->children.size(); ++child) {
        write_struct_field(out, node->children[child], depth+1,
                           node->children[0]->lexinfo);
    }
}

void write_tree(ofstream& out, astree* node, int depth) {
    switch (node->symbol) {
        case TOK_STRUCT:    if (!node->attributes[ATTR_array]) {
                                write_struct(out, node, depth);
                                break;
                            }
        default:            write_node(out, node, depth);
                            if (node->children.empty()) break;
                            for (astree* child: node->children) {
                                assert(child != nullptr);
                                write_tree(out, child, depth+1);
                            }
    }
}

attr_bitset get_node_attr(astree* node) {
    attr_bitset attr = node->attributes;

    //if:   node->lexinfo is defined in struct_stack.
    //then: set it to ATTR_struct, and return.
    const auto& auto_node = struct_stack->find(node->lexinfo);
    if (auto_node != struct_stack->end()) {
        attr.set(ATTR_struct);
        return attr;
    }

    switch (node->symbol) {
        case TOK_VOID:      attr.set(ATTR_void);
                            break;
        case TOK_BOOL:      attr.set(ATTR_bool);
                            break;
        case TOK_CHAR:      attr.set(ATTR_char);
                            break;
        case TOK_INT:       attr.set(ATTR_int);
                            break;
        case TOK_NIL:       attr.set(ATTR_null);
                            attr.set(ATTR_const);
                            break;
        case TOK_STRING:    attr.set(ATTR_string);
                            break;
        case TOK_STRUCT:    attr.set(ATTR_struct);
                            break;
        case TOK_BRKKRB:    attr.set(ATTR_array);
                            break;
        case TOK_TYPEID:    attr.set(ATTR_typeid);
                            break;
        case TOK_FUNCTION:  attr.set(ATTR_function);
                            break;
        case TOK_STRCONST:  attr.set(ATTR_string);
                            attr.set(ATTR_const);
                            break;
        case TOK_CHARCONST: attr.set(ATTR_char);
                            break;
        case TOK_FIELD:     attr.set(ATTR_field);
                            break;
        case TOK_IDENT:     attr.set(ATTR_lval);
                            break;
        case TOK_DECLID:    attr.set(ATTR_lval);
                            break;
        case TOK_NUMBER:    attr.set(ATTR_int);
                            attr.set(ATTR_const);
                            break;
        case TOK_TRUE:
        case TOK_FALSE:     attr.set(ATTR_bool);
                            attr.set(ATTR_const);
                            break;
    }
    return attr;
}

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

void print_symbol_table(ostream& out, symbol_table foo) {
    out << "{";
    for (const auto& i: foo) {
        out << *i.first << ": " << i.second << ", ";
    }
    out << "}" << endl;
}

void parse_array(astree* node) {
    node->lexinfo = node->children[1]->lexinfo;

    node->attributes |= node->attributes;
    node->attributes |= node->children[0]->attributes;

    const auto& auto_sym = node->node->find(node->lexinfo);
    assert(auto_sym != node->node->end());
    symbol* sym = auto_sym->second;
    sym->attributes |= node->attributes;
}

void parse_node(astree* node) {
    if (node == nullptr) return;

    if (symbol_stack[blocknr] == nullptr) {
        vector<symbol_table*>::iterator it;
        it = symbol_stack.begin() + blocknr;
        symbol_stack.insert(it, new symbol_table());
    }

    symbol* s = new_symbol(node, blocknr);

    symbol_stack[blocknr]->insert(symbol_entry(node->lexinfo, s));
    node->node = symbol_stack[blocknr];
    node->block_number = s->blocknr;
    node->attributes = s->attributes;

    switch (node->symbol) {
        case TOK_BRKKRB:    parse_array(node);
                            break;
    }
}

void parse_struct_child(astree* node, const string* name,
                        symbol_table* fields) {
    if (node == nullptr) return;

    symbol* s = new_symbol(node, 0);
    s->attributes.set(ATTR_field);

    //TODO only insert if its an lval
    fields->insert(symbol_entry(node->lexinfo, s));
    node->node = fields;
    node->attributes = s->attributes;
    astree* first_child;
    try {
        first_child = node->children.at(0);
    } catch (out_of_range& e) {
        return;
    }
    parse_struct_child(first_child, name, fields);
}

void parse_struct(astree* node) {
    const string* struct_name = node->children[0]->lexinfo;
    const auto& auto_node = struct_stack->find(struct_name);
    symbol* sym;
    if (auto_node != struct_stack->end()) {
        sym = auto_node->second;
    } else {
        sym = new_symbol(node, 0);
        sym->fields = new symbol_table();
    }
    node->block_number = 0;
    node->attributes.set(ATTR_struct);
    node->node = struct_stack;
    for(size_t child = 1; child < node->children.size(); ++child) {
        parse_struct_child(node->children[child], struct_name,
                           sym->fields);
    }
    struct_stack->insert(symbol_entry(struct_name, sym));
}

void parse_function(astree* node) {
    assert(node != nullptr);

    //set node's attributes to that of its first child
    assert(node->children[0] != nullptr);
    node->attributes |= node->children[0]->attributes;

    //set function's paramlist to all be ATTR_param
    for (size_t i = 0; i < node->children[1]->children.size(); i++) {
        astree* child = node->children[1]->children[i];

        child->children[0]->attributes |= child->attributes;
        child->children[0]->attributes.set(ATTR_param);

        assert (child->node != nullptr);
        const auto& auto_node = child->node->find(child->children[0]->lexinfo);
        assert (auto_node != node->node->end());
        auto_node->second->attributes = child->children[0]->attributes;
    }
}

void parse_tree(astree* node) {
    vector<symbol_table*>::iterator it;
    switch (node->symbol) {
        case TOK_STRUCT:    parse_struct(node);
                            return;
        case TOK_BLOCK:     blocknr++;
                            break;
    }
    for (size_t child = 0; child < node->children.size(); ++child) {
        parse_tree(node->children[child]);
    }
    switch (node->symbol) {
        case TOK_FUNCTION:  parse_function(node);
                            parse_node(node);
                            break;
        case TOK_BLOCK:     it = symbol_stack.begin() + blocknr;
                            symbol_stack.insert(it, nullptr);
                            blocknr--;
        default:            parse_node(node);
                            break;
    }
}
