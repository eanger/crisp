#include <cassert>
#include <cctype>
#include <cstring>
#include <iostream>
#include <string>
#include <tuple>
#include <unordered_map>

#include "read.hpp"
#include "value.hpp"
#include "eval.hpp"

using namespace std;

namespace crisp{

bool isDelimiter(char c){
  return c == '\0' || isspace(c) || (string{"()[]\";#"}.find(c) != string::npos);
}

Value* read(Environment* envt){
  auto input = envt->getBinding(getInternedSymbol("input"));
  if(!input){
    throw EvaluationError("Cannot get input binding.");
  }
  if(input->type != Value::Type::STRING){
    throw EvaluationError("Unable to read anything but a string.");
  }
  return get<0>(readElement(input->str.str));
}

tuple<Value*, char*> readElement(char* input){
  // attempt to read token, throw exception if failure
  if(strlen(input) == 0){
    throw EvaluationError("Unable to read token from empty string.");
  }
  Token token_type;
  string token_contents;
  char* rest;
  tie(token_type, token_contents, rest) = readToken(input);
  Value* result = nullptr;
  switch(token_type){
    case Token::NUMBER:{
      result = new Value(stol(token_contents));
    } break;
    case Token::BOOLEAN:{
      if(token_contents == "#f"){
        result = &False;
      } else {
        result = &True;
      }
    } break;
    case Token::CHARACTER:{
      result = new Value(token_contents.back());
    } break;
    case Token::STRING:{
      Value::Str s{token_contents.c_str()};
      result = new Value(s);
    } break;
    case Token::LPAREN:{
      tie(result, rest) = readList(rest, EmptyList);
    } break;
    case Token::SYMBOL:{
      result = getInternedSymbol(token_contents);
    } break;
    case Token::RPAREN:{
      result = nullptr; // indicate that we're finished with a list
    } break;
    case Token::COMMA:{
      Value* quoted;
      tie(quoted, rest) = readElement(rest);
      result = new Value(getInternedSymbol("quote"), new Value(quoted, nullptr));
    } break;
  }
  return {result, rest};
}

tuple<Value*, char*> readList(char* input, Value* list_so_far){
  Value* v;
  char* rest;
  tie(v, rest) = readElement(input);
  if(v){
    Value* new_list = new Value(v, list_so_far);
    return readList(rest, new_list);
  } else {
    return {reverse(list_so_far), rest};
  }
}

tuple<Token, string, char*> readLiteral(char* input) {
  if(strlen(input) == 0){
    throw EvaluationError("Unable to read token from empty string.");
  }
  char ch = input[0];
  char* rest = &input[1];
  if(ch == 't'){
    if(isDelimiter(rest[0])){
      return make_tuple(Token::BOOLEAN, string{"#t"}, rest);
    }
    throw LexingError("Missing delimiter.");
  } else if(ch == 'f'){
    if(isDelimiter(rest[0])){
      return make_tuple(Token::BOOLEAN, string{"#f"}, rest);
    }
    throw LexingError("Missing delimiter.");
  } else if(ch == '\\'){
    ch = rest[0];
    rest = &rest[1];
    if(isDelimiter(rest[1])){
      return make_tuple(Token::CHARACTER, string{"#\\"} + ch, rest);
    }
    throw LexingError("Missing delimiter.");
  } else {
    // throw, invalid literal syntax
    throw LexingError("Invalid literal syntax.");
  }
}

tuple<Token, string, char*> readNumber(char* input, char first_ch) {
  char ch;
  string result = "";
  result.push_back(first_ch);
  size_t idx;
  for(idx = 0; idx < strlen(input); ++idx){
    ch = input[idx];
    if(isdigit(ch)){
      result.push_back(ch);
    } else if(isDelimiter(ch)){
      break;
    } else {
      // throw, cant have weird symbols in a number
      throw LexingError("Non-numeric digit.");
    }
  }
  return make_tuple(Token::NUMBER, result, &input[idx]);
}

tuple<Token, string, char*> readString(char* input) {
  char ch;
  string result = "";
  size_t idx;
  for(idx = 0; idx < strlen(input); ++idx){
    ch = input[idx];
    if(ch == '\"'){
      if(isDelimiter(input[idx + 1])){
        break;
      }
      throw LexingError("Missing delimiter.");
    }
    result.push_back(ch);
  }
  return make_tuple(Token::STRING, result, &input[idx + 1]);
}

tuple<Token, string, char*> readSymbol(char* input, char first_ch) {
  string result = "";
  result.push_back(first_ch);
  char ch;
  size_t idx;
  for(idx = 0; idx < strlen(input); ++idx){
    ch = input[idx];
    if(isDelimiter(ch)){
      break;
    } else if(isalnum(ch) || 
              (string{"!$%&*/:<=>?^_~+-.@"}.find(ch) != string::npos)){
      result.push_back(ch);
    } else {
      throw LexingError("Invalid character for symbols.");
    }
  }
  return make_tuple(Token::SYMBOL, result, &input[idx]);
}

// returns the token, the string, and the value representing the remaining input
tuple<Token, string, char*> readToken(char* input) {
  char ch = input[0];
  char* rest = &input[1];
  if(ch == '-' || isdigit(ch)){
    return readNumber(rest, ch);
  } else if(ch == '#'){
    return readLiteral(rest);
  } else if(ch == '"'){
    return readString(rest);
  } else if(ch == '('){
    return make_tuple(Token::LPAREN, string{"("}, rest);
  } else if(ch == ')'){
    return make_tuple(Token::RPAREN, string{")"}, rest);
  } else if(isspace(ch)){
    return readToken(rest);
  } else if(isalpha(ch) || (string{"!$%&*/:<=>?^_~"}.find(ch) != string::npos)){
    return readSymbol(rest, ch);
  } else if(ch == '.'){
    throw LexingError("Cannot parse Dot Notation at this time.");
  } else if(ch == '\''){
    return make_tuple(Token::COMMA, string{"'"}, rest);
  } else {
    assert(false && "Should never be able to insert an unreadible character");
  }
}

Value* doRead(istream& input_stream){
  string line;
  getline(input_stream, line);
  if(line.empty()){
    return EmptyList;
  }
  Value::Str s{line.c_str()};
  Value v{s};
  Environment* envt = new Environment{&GlobalEnvironment};
  envt->setBinding(getInternedSymbol("input"), &v);
  auto res = read(envt);
  return res;
}

}

