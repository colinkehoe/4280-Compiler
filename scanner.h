#ifndef SCANNER_H
#define SCANNER_H

#include <iostream>
#include "token.h"

Token get_scanner_token(std::istream& input_stream, int& current_line);

#endif
