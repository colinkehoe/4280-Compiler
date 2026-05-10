#include "semantics.h"
#include <iostream>
#include <algorithm>
#include <cstdlib>

SymbolTable globalTable;

// check if the variable already exists
bool verify_symbol(std::string var_name)
{
	auto it = std::find(globalTable.variables.begin(), globalTable.variables.end(), var_name);
	return it != globalTable.variables.end();
}

// insert the variable into the table
void insert_symbol(std::string var_name)
{
	if (verify_symbol(var_name))
	{
		std::cerr << "SEMANTIC ERROR: Redefining a variable '" << var_name << "'" << std::endl;
		exit(1);
	}

	globalTable.variables.push_back(var_name);
}

// traverse the tree and check semantics
void check_semantics(Node* node)
{
	if (node == nullptr)
		return;
		
	// process B, declarations. ($ t1 or * t1)
	if (node->label == "B") {
		if (node->child1 && node->child2) {
			std::string operation = node->child1->label;
			std::string var_name = node->child2->label;

			// $ is declaration. * is some operation
			if (operation == "$") {
				insert_symbol(var_name);
			}
			else if (operation == "*") {
				if (!verify_symbol(var_name)) {
					std::cerr << "SEMANTIC ERROR: Using an undefined variable '" << var_name << "'" << std::endl;
					std::exit(1);
				}
			}		
		}
	}

	//process C (- t1 J)
	else if (node->label == "C") {
		if (node->child2) {
			std::string var_name = node->child2->label;
			if (!verify_symbol(var_name)) {
				std::cerr << "SEMANTIC ERROR: Using an undefined variable '" << var_name << "'" << std::endl;
				std::exit(1);
			}
		}
	}

	// process H (t1 | t2)
	// note: t1 token is a variable. t2 is not.
	else if (node->label == "H") {
		if (node->child1) {
			std::string var_name = node->child1->label;
			if (!var_name.empty() && (var_name[0] == '"' || var_name[0] == '#')) {
				if (!verify_symbol(var_name)) {
					std::cerr << "SEMANTIC ERROR: Using an undefined variable '" << var_name << "'" << std::endl;
					std::exit(1);
				}
			}
		}
	}

	//check all children
	check_semantics(node->child1);
	check_semantics(node->child2);
	check_semantics(node->child3);
	check_semantics(node->child4);
	check_semantics(node->child5);
	check_semantics(node->child6);
}

void print_symbol_table()
{
	std::cout << "Symbol Table:" << std::endl;
	for (const std::string& var : globalTable.variables)
		std::cout << var << std::endl;
}
