#include <iostream>
#include "testTree.h"

void tree_print(Node* root, int depth) {
    if (root == nullptr) return;

    // 4 spaces per level indentation
    for (int i = 0; i < depth; i++) {
        std::cout << "    ";
    }

    // Print just the label (terminals are now their own nodes)
    std::cout << root->label << std::endl;

    tree_print(root->child1, depth + 1);
    tree_print(root->child2, depth + 1);
    tree_print(root->child3, depth + 1);
    tree_print(root->child4, depth + 1);
    tree_print(root->child5, depth + 1);
    tree_print(root->child6, depth + 1);
}
