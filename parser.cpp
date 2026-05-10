#include <cstdlib>
#include <iostream>

#include "parser.h"
#include "scanner.h"

void syntax_error(const std::string& expected, const Token& found, int line) {
    std::cerr << "PARSER ERROR: Expected " << expected << " but found '"
              << found.tokenContents << "' on line " << line << std::endl;
    std::exit(1);
}

Node* parser(std::istream& input_stream) {
    int current_line = 1;
    Token current_token = get_scanner_token(input_stream, current_line);
    Node* root = s_func(input_stream, current_token, current_line);
    if (current_token.id != EOF_TK)
        syntax_error("EOF", current_token, current_line);
    return root;
}

// S -> A K '
Node* s_func(std::istream& input_stream, Token& current_token,
             int& current_line) {
    Node* node = new Node("S");

    // process non-terminals
    node->child1 = a_func(input_stream, current_token, current_line);
    node->child2 = k_func(input_stream, current_token, current_line);

    // process terminal "'"
    if (current_token.tokenContents == "'") {
        node->child3 = new Node("'");  // Terminal as a leaf node
        current_token = get_scanner_token(input_stream, current_line);
    } else {
        syntax_error("'", current_token, current_line);
    }
    return node;
}

// A -> B | C | G H F J ' S
Node* a_func(std::istream& input_stream, Token& current_token,
             int& current_line) {
    Node* node = new Node("A");
    std::string contents = current_token.tokenContents;

    // FIRST(B) = { $, * }
    if (contents == "$" || contents == "*") {
        node->child1 = b_func(input_stream, current_token, current_line);
    }
    // FIRST(C) = { - }
    else if (contents == "-") {
        node->child1 = c_func(input_stream, current_token, current_line);
    }
    // FIRST(G) = { %, & }
    else if (contents == "%" || contents == "&") {
        node->child1 = g_func(input_stream, current_token, current_line);
        node->child2 = h_func(input_stream, current_token, current_line);
        node->child3 = f_func(input_stream, current_token, current_line);
        node->child4 = j_func(input_stream, current_token, current_line);

        // process terminal "'"
        if (current_token.tokenContents == "'") {
            node->child5 = new Node("'");  // Terminal as a leaf node
            current_token = get_scanner_token(input_stream, current_line);
        }
        // handle errors
        else {
            syntax_error("'", current_token, current_line);
        }
        node->child6 = s_func(input_stream, current_token, current_line);
    } else {
        syntax_error("$, *, -, %, or &", current_token, current_line);
    }
    return node;
}

// B -> $ t1 | * t1
Node* b_func(std::istream& input_stream, Token& current_token,
             int& current_line) {
    Node* node = new Node("B");
    std::string contents = current_token.tokenContents;
    if (contents == "$" || contents == "*") {
        node->child1 = new Node(contents);
        current_token = get_scanner_token(input_stream, current_line);
        if (current_token.id == T1_TK) {
            node->child2 =
                new Node(current_token.tokenContents);  // actual t1 string
            current_token = get_scanner_token(input_stream, current_line);
        } else {
            syntax_error("t1", current_token, current_line);
        }
    } else {
        syntax_error("$ or *", current_token, current_line);
    }
    return node;
}

// C -> - t1 J
Node* c_func(std::istream& input_stream, Token& current_token,
             int& current_line) {
    Node* node = new Node("C");
    if (current_token.tokenContents == "-") {
        node->child1 = new Node("-");
        current_token = get_scanner_token(input_stream, current_line);

        // match t1 token
        if (current_token.id == T1_TK) {
            node->child2 = new Node(current_token.tokenContents);
            current_token = get_scanner_token(input_stream, current_line);
            node->child3 = j_func(input_stream, current_token, current_line);
        } else {
            syntax_error("t1", current_token, current_line);
        }
    } else {
        syntax_error("-", current_token, current_line);
    }
    return node;
}

// D -> E H D | ε
Node* d_func(std::istream& input_stream, Token& current_token,
             int& current_line) {
    Node* node = new Node("D");

    if (current_token.tokenContents == "(" ||
        current_token.tokenContents == ")") {
        node->child1 = e_func(input_stream, current_token, current_line);
        node->child2 = h_func(input_stream, current_token, current_line);
        node->child3 = d_func(input_stream, current_token, current_line);
        return node;
    } else {
        node->child1 = new Node("ε");
    }
    return node;
}

// E -> ( | )
Node* e_func(std::istream& input_stream, Token& current_token,
             int& current_line) {
    Node* node = new Node("E");
    std::string contents = current_token.tokenContents;
    if (contents == "(" || contents == ")") {
        node->child1 = new Node(contents);
        current_token = get_scanner_token(input_stream, current_line);
    } else {
        syntax_error("( or )", current_token, current_line);
    }
    return node;
}

// F -> , | . | +
Node* f_func(std::istream& input_stream, Token& current_token,
             int& current_line) {
    Node* node = new Node("F");
    std::string contents = current_token.tokenContents;
    if (contents == "," || contents == "." || contents == "+") {
        node->child1 = new Node(contents);
        current_token = get_scanner_token(input_stream, current_line);
    } else {
        syntax_error(", . or +", current_token, current_line);
    }
    return node;
}

// G -> % | &
Node* g_func(std::istream& input_stream, Token& current_token,
             int& current_line) {
    Node* node = new Node("G");
    std::string contents = current_token.tokenContents;
    if (contents == "%" || contents == "&") {
        node->child1 = new Node(contents);
        current_token = get_scanner_token(input_stream, current_line);
    } else {
        syntax_error("% or &", current_token, current_line);
    }
    return node;
}

// H -> t1 | t2
Node* h_func(std::istream& input_stream, Token& current_token,
             int& current_line) {
    Node* node = new Node("H");
    if (current_token.id == T1_TK || current_token.id == T2_TK) {
        node->child1 = new Node(current_token.tokenContents);
        current_token = get_scanner_token(input_stream, current_line);
    } else {
        syntax_error("t1 or t2", current_token, current_line);
    }
    return node;
}

// J -> ' H D
Node* j_func(std::istream& input_stream, Token& current_token,
             int& current_line) {
    Node* node = new Node("J");
    if (current_token.tokenContents == "'") {
        node->child1 = new Node("'");
        current_token = get_scanner_token(input_stream, current_line);
        node->child2 = h_func(input_stream, current_token, current_line);
        node->child3 = d_func(input_stream, current_token, current_line);
    } else {
        syntax_error("'", current_token, current_line);
    }
    return node;
}

// K -> S K | ε
Node* k_func(std::istream& input_stream, Token& current_token,
             int& current_line) {
    Node* node = new Node("K");
    std::string contents = current_token.tokenContents;
    if (contents == "$" || contents == "*" || contents == "-" ||
        contents == "%" || contents == "&") {
        node->child1 = s_func(input_stream, current_token, current_line);
        node->child2 = k_func(input_stream, current_token, current_line);
        return node;
    } else {
        node->child1 = new Node("ε");  // Explicit epsilon node
    }

    return node;
}
