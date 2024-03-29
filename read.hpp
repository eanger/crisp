#pragma once
#include <istream>
#include <string>
#include <tuple>

#include "value.hpp"
#include "exception.hpp"
#include "eval.hpp"

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
  NUMBER, BOOLEAN, CHARACTER, STRING, LPAREN, RPAREN, SYMBOL, QUOTE, BACKTICK, COMMA
};

/***** Functions *****/
Value* doRead(std::istream& input_stream);
Value* read(Environment* envt);
std::tuple<Value*, char*> readElement(char* input);
std::tuple<Value*, char*> readList(char* input, Value* list_so_far);
bool isDelimiter(char c);
std::tuple<Token, std::string, char*> readLiteral(char* input);
std::tuple<Token, std::string, char*> readNumber(char* input, char first_ch);
std::tuple<Token, std::string, char*> readString(char* input);
std::tuple<Token, std::string, char*> readSymbol(char* input, char first_ch);
std::tuple<Token, std::string, char*> readToken(char* input);

}
