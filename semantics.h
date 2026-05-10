#ifndef SEMANTICS_H
#define SEMANTICS_H

#include <string>
#include <vector>
#include "parser.h"

//structure that will hold the global scope symbol table
struct SymbolTable {
	std::vector<std::string> variables;
};

void check_semantics(Node* node);
void insert_symbol(std::string var_name);
bool verify_symbol(std::string var_name);
void print_symbol_table();

#endif
