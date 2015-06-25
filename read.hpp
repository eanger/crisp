#pragma once
#include <istream>
#include <string>
#include <utility>

#include "value.hpp"
#include "exception.hpp"

namespace crisp{

/***** Exceptions *****/
class LexingError : public VerboseError {
  public:
    LexingError(const char* problem) : VerboseError("Lexing error: ", problem) {}
};

class ParsingError : public VerboseError {
  public:
    ParsingError(const char* problem) : VerboseError("Parsing error: ", problem) {}
};

/***** Classes *****/
enum class Token {
  NUMBER, BOOLEAN, CHARACTER, STRING, LPAREN, RPAREN, SYMBOL, COMMA
};

/***** Functions *****/
Value* read(std::istream& input_stream);
Value* readList(std::istream& input_stream, Value* list_so_far);
bool isDelimiter(char c);
std::pair<Token, std::string> readLiteral(std::istream& input_stream);
std::pair<Token, std::string> readNumber(std::istream& input_stream, char first_ch);
std::pair<Token, std::string> readString(std::istream& input_stream);
std::pair<Token, std::string> readSymbol(std::istream& input_stream, char first_ch);
std::pair<Token, std::string> readToken(std::istream& input_stream);

}
