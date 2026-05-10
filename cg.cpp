#include "cg.h"
#include "semantics.h" 
#include <iostream>
#include <cctype>
#include <vector>
#include <map>

// import the global symbol table from the static semantics phase
extern SymbolTable globalTable;

// counters for generating unique target labels and temporary memory variables
int label_count = 0;
int temp_count = 0;

// containers to hold dynamically generated memory requirements
std::map<std::string, std::string> constants; // stores t2 integers
std::vector<std::string> temps;				  // stores temporary variables used for calculations


// generates a unique branch label for loops and conditionals
std::string new_label() {
    return "L" + std::to_string(label_count++);
}

// generates a unique temporary variable and tracks it for allocation and destruction
std::string new_temp() {
    std::string t = "V" + std::to_string(temp_count++);
    temps.push_back(t);
    return t;
}

// registers a constant integer value as a temporary variable in memory
std::string new_const(std::string val) {
    std::string t = "V" + std::to_string(temp_count++);
    constants[t] = val;
    return t;
}

// parses t2 tokens (int) based on the grammar's letter-count rule.
// 1 letter = positive, >1 letter = negative
std::string convert_t2(std::string t2) {
    int letters = 0;
    std::string digits = "";
    for (char c : t2) {
        if (isalpha(c)) letters++;
        else if (isdigit(c)) digits += c;
    }
    if (letters > 1) return "-" + digits; 
    return digits;                        
}

// sanitizes t1 tokens (identifiers) to meet with the vm's naming rules.
// the vm requries variables to start with a letter. this strips the '#' or '"'
// and prepends a distinct letter to prevent naming collision.
std::string sanitize_id(std::string t1) {
	std::string clean = "";
	
	// assign a unique prefix based on the original symbol
	if (t1[0] == '"') {
		clean = "Q";
	} else if (t1[0] == '#') {
		clean = "H";
	} else {
		clean = "M";
	}

	// append the digits
	for (char c : t1) {
		if (isdigit(c)) clean += c;
	}
	return clean;
}


/* tree traversal and code gen */


// processes h nodes (values). determines if a token is a variable (t1) or a constant (t2).
std::string process_H(Node* h_node) {
    std::string token = h_node->child1->label;
    if (token[0] == '"' || token[0] == '#') {
        return sanitize_id(token); // Apply sanitization to variable usages
    } else {
        return new_const(convert_t2(token)); 
    }
}


// processes d nodes (math ops). operates directly on the accumulator.
void process_D(Node* d_node, std::ofstream& out) {
    if (d_node->child1->label == "ε") return;
    
    std::string var = process_H(d_node->child2);
    std::string op = d_node->child1->child1->label;
    
    if (op == "(") out << "ADD " << var << "\n";
    else if (op == ")") out << "SUB " << var << "\n";
    
    process_D(d_node->child3, out);
}

// processes j nodes (expressions). always leaves the final result in the accumulator.
void process_J(Node* j_node, std::ofstream& out) {
    std::string var = process_H(j_node->child2);
    out << "LOAD " << var << "\n";
    process_D(j_node->child3, out);
}

// processes b nodes (memory alloc/io). maps '$' to READ and '*' to WRITE
void process_B(Node* b_node, std::ofstream& out) {
    std::string op = b_node->child1->label;
    std::string var = sanitize_id(b_node->child2->label); // Sanitize B declarations
    if (op == "$") {
        out << "READ " << var << "\n";
    } else if (op == "*") {
        out << "WRITE " << var << "\n";
    }
}

// processes c nodes (assignments). evaluates right-hand side, then stores value from acc into left hand side.
void process_C(Node* c_node, std::ofstream& out) {
    std::string var = sanitize_id(c_node->child2->label); // Sanitize C assignments
    process_J(c_node->child3, out); 
    out << "STORE " << var << "\n";
}

// forward declarations for mutually recursive blocks
void process_S(Node* s_node, std::ofstream& out);
void process_K(Node* k_node, std::ofstream& out);

// processes a nodes (statements). acts as a router to specific settlement logic
void process_A(Node* a_node, std::ofstream& out) {
    std::string child_type = a_node->child1->label;
    
    if (child_type == "B") {
        process_B(a_node->child1, out);
    } 
    else if (child_type == "C") {
        process_C(a_node->child1, out);
    } 

	// g blocks handle control flow: if (%) and while (&)
    else if (child_type == "G") {
        std::string g_op = a_node->child1->child1->label; 
        std::string f_op = a_node->child3->child1->label; 
        
        std::string start_label = "";
        std::string end_label = new_label();
        
		// mark top of loop if this is a WHILE
        if (g_op == "&") {
            start_label = new_label();
            out << start_label << ": NOOP\n";
        }
        
		// evaluate rhs expression and store it in a temp var
        process_J(a_node->child4, out);
        std::string rhs_temp = new_temp();
        out << "STORE " << rhs_temp << "\n";
        
		// load lhs into the accumulator and subtract rhs to test
        std::string lhs_var = process_H(a_node->child2);
        out << "LOAD " << lhs_var << "\n";
        
        out << "SUB " << rhs_temp << "\n";
        
		// if the condition is false, jump to to the end label and skip the block.
        // branching using the VM's specific instruction set
        if (f_op == ",") { // Target is > 0. Jump on <= 0.
            out << "BRZNEG " << end_label << "\n";
        } else if (f_op == ".") { // Target is < 0. Jump on >= 0.
            out << "BRZPOS " << end_label << "\n";
        } else if (f_op == "+") { // Target is == 0. Jump on != 0.
            out << "BRPOS " << end_label << "\n";
            out << "BRNEG " << end_label << "\n";
        }
        
		// execute the nested block
        process_S(a_node->child6, out);
        
		// if while loop, jump back to the top to re-eval
        if (g_op == "&") {
            out << "BR " << start_label << "\n";
        }
        
		// exit point
        out << end_label << ": NOOP\n";
    }
}

// processes k nodes (sequence handling). handles sequential statement blocks
void process_K(Node* k_node, std::ofstream& out) {
    if (k_node->child1->label == "ε") return;
    process_S(k_node->child1, out);
    process_K(k_node->child2, out);
}

// processes s nodes (program/block root)
void process_S(Node* s_node, std::ofstream& out) {
    process_A(s_node->child1, out);
    process_K(s_node->child2, out);
}

// main code gen function
void generate_code(Node* root, std::string filename) {
    std::ofstream out(filename);
    if (!out.is_open()) {
        std::cerr << "ERROR: Could not open output file " << filename << std::endl;
        exit(1);
    }
    
	// traverse tree and generate code
    process_S(root, out);
    out << "STOP\n"; // end virtmach
    
    // Memory allocation block
    for (const std::string& v : globalTable.variables) {
        // Sanitize variables extracted from the symbol table for the bottom definitions
        out << sanitize_id(v) << " 0\n";
    }
    for (const auto& pair : constants) {
        out << pair.first << " " << pair.second << "\n";
    }
    for (const std::string& t : temps) {
        out << t << " 0\n";
    }
    
    out.close();
}
