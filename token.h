#ifndef TOKEN_H
#define TOKEN_H

#include <string>
#include <vector>

// TokenID enum
enum TokenId 
{
	T1_TK,  // " or #, followed by digits
	T2_TK,  // Letters followed by digits
	T3_TK,  // Special characters
	EOF_TK, // End of file
	ERR_TK  // Scanner error
};

// Token Object
struct Token 
{
	TokenId id;
	std::string tokenContents;
	int lineNumber;
};

// token descriptions for printing
const std::vector<std::string> TOKEN_DESCRIPTIONS = {
	"t1 token", "t2 token", "t3 token", "End of File", "SCANNER ERROR"
};

#endif
