#include <cctype>

#include "scanner.h"

// Helper for categorizing charactersf or the FSA table columns
int get_character_column(char ch) {
    if (isspace(ch)) return 0;
    if (ch == '"' || ch == '#') return 1;
    if (isdigit(ch)) return 2;
    if (isalpha(ch)) return 3;
    if (std::string("$%&'()*+,-./").find(ch) != std::string::npos) return 4;
    return 5;  // Error or illegal character
}

// FSA Table: Rows are states, Columns are character classes
const int FSA_TABLE[5][6] = {
    // WS #/ Dig Let Sym Err
    {0, 1, -1, 2, 3, -1},      // State 0: Start
    {-1, -1, 1, -1, -1, -1},   // State 1: T1 (digits)
    {-1, -1, 4, 2, -1, -1},    // State 2: T2 (letters)
    {-1, -1, -1, -1, -1, -1},  // State 3: T3 (special chars)
    {-1, -1, 4, -1, -1, -1}    // State 4: T2 (digits)
};

// Scanner function.
// Returns: 1 token.
Token get_scanner_token(std::istream& input_stream, int& current_line) {
    int current_state = 0;
    std::string accumulated_chars = "";
    char next_char;
    Token result_token;

    while (input_stream.get(next_char)) {
        // Empty line
        if (next_char == '\n') current_line++;

        // Skip comments
        if (current_state == 0 && next_char == '!') {
            while (input_stream.get(next_char) && next_char != '!') {
                if (next_char == '\n') current_line++;
            }
            continue;  // skip to next iteration of while loop
        }

        int col = get_character_column(next_char);
        if (current_state == 0 && col == 0) continue;  // skip whitespace

        int next_state = (col == 5) ? -1 : FSA_TABLE[current_state][col];

        if (next_state == -1) {
            // Handle merged tokens by putting back the lookahead character.
            // Additionally, check if we can finish the current token.
            if (current_state == 1 || current_state == 3 ||
                current_state == 4) {
                input_stream.putback(next_char);  // return char to the stream
                if (next_char == '\n') current_line--;
                break;
            }

            // Scanner error for invalid characters
            result_token.id = ERR_TK;
            result_token.tokenContents = accumulated_chars + next_char;
            result_token.lineNumber = current_line;
            return result_token;
        }

        current_state = next_state;
        accumulated_chars += next_char;

        // Stop if the next char would lead to an error or a different token
        char peek = input_stream.peek();
        if (FSA_TABLE[current_state][get_character_column(peek)] == -1) break;
    }

    // set the token ID based on the final FSA state
    if (current_state == 1)
        result_token.id = T1_TK;
    else if (current_state == 2 || current_state == 4)
        result_token.id = T2_TK;
    else if (current_state == 3)
        result_token.id = T3_TK;
    else if (input_stream.eof() && accumulated_chars.empty())
        result_token.id = EOF_TK;

    result_token.tokenContents = accumulated_chars;
    result_token.lineNumber = current_line;
    return result_token;
}
