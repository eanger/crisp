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

bool isDelimiter(char c){
  return isspace(c) || (string{"()[]\";#"}.find(c) != string::npos);
}

Value* read(istream& input_stream){
  auto token = readToken(input_stream);
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
      result = readList(input_stream, EmptyList);
    } break;
    case Token::SYMBOL:{
      return getInternedSymbol(token.second);
    } break;
    case Token::RPAREN:{
      return nullptr; // indicate that we're finished with a list
    } break;
    case Token::COMMA:{
      auto quoted = read(input_stream);
      result = new Value(getInternedSymbol("quote"), new Value(quoted, nullptr));
    } break;
  }
  return result;
}

Value* readList(istream& input_stream, Value* list_so_far) {
  Value* v = read(input_stream);
  if(v){
    Value* new_list = new Value(v, list_so_far);
    return readList(input_stream, new_list);
  } else {
    return reverse(list_so_far);
  }
}

pair<Token, string> readLiteral(istream& input_stream) {
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

pair<Token, string> readNumber(istream& input_stream, char first_ch) {
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

pair<Token, string> readString(istream& input_stream) {
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

pair<Token, string> readSymbol(istream& input_stream, char first_ch) {
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

pair<Token, string> readToken(istream& input_stream) {
  // attempt to read token, throw exception if failure
  char ch;
  input_stream.get(ch);
  if(ch == '-' || isdigit(ch)){
    return readNumber(input_stream, ch);
  } else if(ch == '#'){
    return readLiteral(input_stream);
  } else if(ch == '"'){
    return readString(input_stream);
  } else if(ch == '('){
    return make_pair(Token::LPAREN, string{"("});
  } else if(ch == ')'){
    return make_pair(Token::RPAREN, string{")"});
  } else if(isspace(ch)){
    return readToken(input_stream);
  } else if(isalpha(ch) || (string{"!$%&*/:<=>?^_~"}.find(ch) != string::npos)){
    return readSymbol(input_stream, ch);
  } else if(ch == '.'){
    throw LexingError("Cannot parse Dot Notation at this time.");
  } else if(ch == '\''){
    return make_pair(Token::COMMA, string{"'"});
  } else {
    assert(false && "Should never be able to insert an unreadible character");
  }
}

}

