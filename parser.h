#ifndef PARSER_H
#define PARSER_H

#include <iostream>
#include <string>
#include "token.h"

struct Node {
    std::string label;
    
    // Explicit pointers for up to 6 children (Rule A requires 6)
    struct Node *child1, *child2, *child3, *child4, *child5, *child6;

    Node(std::string name) : label(name), 
        child1(nullptr), child2(nullptr), child3(nullptr), 
        child4(nullptr), child5(nullptr), child6(nullptr) {}
};

Node* parser(std::istream& input_stream);
Node* s_func(std::istream& input_stream, Token& current_token, int& current_line);
Node* a_func(std::istream& input_stream, Token& current_token, int& current_line);
Node* b_func(std::istream& input_stream, Token& current_token, int& current_line);
Node* c_func(std::istream& input_stream, Token& current_token, int& current_line);
Node* d_func(std::istream& input_stream, Token& current_token, int& current_line);
Node* e_func(std::istream& input_stream, Token& current_token, int& current_line);
Node* f_func(std::istream& input_stream, Token& current_token, int& current_line);
Node* g_func(std::istream& input_stream, Token& current_token, int& current_line);
Node* h_func(std::istream& input_stream, Token& current_token, int& current_line);
Node* j_func(std::istream& input_stream, Token& current_token, int& current_line);
Node* k_func(std::istream& input_stream, Token& current_token, int& current_line);

#endif
