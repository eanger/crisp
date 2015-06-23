#pragma once
#include <iostream>
#include <string>
#include <utility>

namespace crisp{
namespace{

enum class Token {
  NUMBER, BOOLEAN, CHARACTER, STRING, LPAREN, RPAREN, SYMBOL
};

Value* doRead(std::istream& input_stream);
Value* readList(std::istream& input_stream, Value* list_so_far);
bool isDelimiter(char c);
std::pair<Token, std::string> readLiteral(std::istream& input_stream);
std::pair<Token, std::string> readNumber(std::istream& input_stream, char first_ch);
std::pair<Token, std::string> readString(std::istream& input_stream);
std::pair<Token, std::string> readSymbol(std::istream& input_stream, char first_ch);
std::pair<Token, std::string> readToken(std::istream& input_stream);

} // end unnamed namespace
}
