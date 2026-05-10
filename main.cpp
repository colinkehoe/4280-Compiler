#include <iostream>
#include <fstream>
#include <string>
#include "scanner.h"
#include "parser.h"
#include "testTree.h"
#include "semantics.h"
#include "cg.h" 

int main(int argc, char* argv[]) {
    std::istream* input_stream = &std::cin;
    std::ifstream file_stream;
    std::string out_filename = "out.asm"; // Default

    if (argc == 2) {
        file_stream.open(argv[1]);
        if (!file_stream.is_open()) {
            std::cerr << "ERROR: File could not be opened: " << argv[1] << std::endl;
            return 1;
        }
        input_stream = &file_stream;
        out_filename = std::string(argv[1]) + ".asm"; // Set specific output name
    } else if (argc > 2) {
        std::cerr << "Usage: P5 [file]" << std::endl;
        return 1;
    }

    Node* root = parser(*input_stream);
    
    // 1. Static Semantics Check
    check_semantics(root);

    // 2. Generate Assembly Code
    generate_code(root, out_filename);

    if (file_stream.is_open()) {
        file_stream.close();
    }

    return 0;
}
