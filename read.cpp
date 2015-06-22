#include <cassert>
#include <cctype>
#include <cstring>
#include <iostream>
#include <string>
#include <unordered_map>
#include <utility>

#include "read.hpp"
#include "value.hpp"

using namespace std;

namespace crisp{
namespace{ // implementation namespace
enum class Token {
  NUMBER, BOOLEAN, CHARACTER, STRING, LPAREN, RPAREN, DOT, SYMBOL
};

bool isDelimiter(char c){
  return isspace(c) || (string{"()[]\";#"}.find(c) != string::npos);
}

Value* tryReadPair(istream& input_stream);

Value* parse(istream& input_stream,
             const pair<Token, string>& token) {
  Value* result = nullptr;
  switch(token.first){
    case Token::NUMBER:{
      result = new Value(stol(token.second));
    } break;
    case Token::BOOLEAN:{
      if(token.second == "#f"){
        result = &False;
      } else {
        result = &True;
      }
    } break;
    case Token::CHARACTER:{
      result = new Value(token.second.back());
    } break;
    case Token::STRING:{
      Value::Str s;
      s.str = new char[token.second.size()]();
      strcpy(s.str, token.second.c_str());
      result = new Value(s);
    } break;
    case Token::LPAREN:{
      result = tryReadPair(input_stream);
    } break;
    case Token::SYMBOL:{
      if(token.second == "quote"){
        result = Quote;
      } else if(token.second == "define"){
        result = Define;
      } else if(token.second == "set!"){
        result = Set;
      } else if(token.second == "if"){
        result = If;
      } else {
        return getInternedSymbol(token.second);
      }
    } break;
    case Token::RPAREN:{
      // do nothing, return nullptr
    } break;
    case Token::DOT:{
      // do nothing, return nullptr
    } break;
  }
  return result;
}

pair<Token, string> tryReadToken(istream& input_stream);
Value* tryReadPair(istream& input_stream) {
  auto token = tryReadToken(input_stream);
  if(token.first  == Token::RPAREN){
    // TODO: This should try and read another pair object, not abort
    return &EmptyPair;
  }
  Value* first = parse(input_stream, token);
  if(first == nullptr){
    throw ParsingError("First element in pair can't be a dot or a right paren.");
  }
  auto dot = tryReadToken(input_stream);
  // must be a dot
  if(dot.first != Token::DOT){
    throw ParsingError("Must have a dot separate elements in a list.");
  }
  token = tryReadToken(input_stream);
  // second can't be an end-paren
  if(token.first == Token::RPAREN){
    throw ParsingError("Must have a second element in a pair.");
  }
  Value* second = parse(input_stream, token);

  auto rparen = tryReadToken(input_stream);
  // must end in an rparen
  if(rparen.first != Token::RPAREN){
    throw ParsingError("Pair must end in a right paren.");
  }
  return new Value{first, second};
}

pair<Token, string> tryReadNumber(istream& input_stream, char first_ch);
pair<Token, string> tryReadLiteral(istream& input_stream);
pair<Token, string> tryReadString(istream& input_stream);
pair<Token, string> tryReadSymbol(istream& input_stream, char first_ch);
pair<Token, string> tryReadToken(istream& input_stream) {
  // attempt to read token, throw exception if failure
  char ch;
  input_stream.get(ch);
  if(ch == '-' || isdigit(ch)){
    return tryReadNumber(input_stream, ch);
  } else if(ch == '#'){
    return tryReadLiteral(input_stream);
  } else if(ch == '"'){
    return tryReadString(input_stream);
  } else if(ch == '('){
    return make_pair(Token::LPAREN, string{"("});
  } else if(ch == ')'){
    return make_pair(Token::RPAREN, string{")"});
  } else if(isspace(ch)){
    return tryReadToken(input_stream);
  } else if(ch == '.'){
    if(isDelimiter(input_stream.peek())){
      return make_pair(Token::DOT, string{"."});
    }
    throw LexingError("Need delimiter after dot.");
  } else if(isalpha(ch) || (string{"!$%&*/:<=>?^_~"}.find(ch) != string::npos)){
    return tryReadSymbol(input_stream, ch);
  } else {
    assert(false && "Should never be able to insert an unreadible character");
  }
}

pair<Token, string> tryReadNumber(istream& input_stream, char first_ch) {
  char ch;
  string result = "";
  result.push_back(first_ch);
  while(input_stream.get(ch)){
    if(isdigit(ch)){
      result.push_back(ch);
    } else if(isDelimiter(ch)){
      input_stream.unget();
      break;
    } else {
      // throw, cant have weird symbols in a number
      throw LexingError("Non-numeric digit.");
    }
  }
  return make_pair(Token::NUMBER, result);
}

pair<Token, string> tryReadLiteral(istream& input_stream) {
  char ch;
  input_stream.get(ch);
  if(ch == 't'){
    if(isDelimiter(input_stream.peek())){
      return make_pair(Token::BOOLEAN, string{"#t"});
    }
    throw LexingError("Missing delimiter.");
  } else if(ch == 'f'){
    if(isDelimiter(input_stream.peek())){
      return make_pair(Token::BOOLEAN, string{"#f"});
    }
    throw LexingError("Missing delimiter.");
  } else if(ch == '\\'){
    input_stream.get(ch);
    if(isDelimiter(input_stream.peek())){
      return make_pair(Token::CHARACTER, string{"#\\"} + ch);
    }
    throw LexingError("Missing delimiter.");
  } else {
    // throw, invalid literal syntax
    throw LexingError("Invalid literal syntax.");
  }
}

pair<Token, string> tryReadString(istream& input_stream) {
  char ch;
  string result = "\"";
  while(input_stream.get(ch)){
    result.push_back(ch);
    if(ch == '\"'){
      if(isDelimiter(input_stream.peek())){
        break;
      }
      throw LexingError("Missing delimiter.");
    }
  }
  return make_pair(Token::STRING, result);
}

pair<Token, string> tryReadSymbol(istream& input_stream, char first_ch) {
  string result = "";
  result.push_back(first_ch);
  char ch;
  while(input_stream.get(ch)){
    if(isDelimiter(ch)){
      input_stream.unget();
      break;
    } else if(isalnum(ch) || 
              (string{"!$%&*/:<=>?^_~+-.@"}.find(ch) != string::npos)){
      result.push_back(ch);
    } else {
      throw LexingError("Invalid character for symbols.");
    }
  }
  return make_pair(Token::SYMBOL, result);
}

} // end unnamed namespace

/***** API        *****/
Value* read(istream& input_stream) {
  return parse(input_stream, tryReadToken(input_stream));
}

}

