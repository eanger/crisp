#include <cassert>
#include <cctype>
#include <exception>
#include <iostream>
#include <stack>
#include <string>

using namespace std;

class LexingError : public exception {
  virtual const char* what() const throw(){
    return "Unable to lex token.";
  }
};

class ParsingError : public exception {
  virtual const char* what() const throw(){
    return "Unable to parse command.";
  }
};

enum class Token {
  NUMBER, BOOLEAN, CHARACTER, STRING, LPAREN, RPAREN
};

class Value {
  public:
    enum class Type {
      FIXNUM,
      BOOLEAN,
      CHARACTER,
      STRING,
      PAIR,
    };
    Type type;
    struct Pair{
      Value* car;
      Value* cdr;
    };
    union {
      long fixnum;
      bool boolean;
      char character;
      char* str;
      Pair pair;
    };

    explicit Value(long n) : type{Type::FIXNUM}, fixnum{n} {}
    explicit Value(bool b) : type{Type::BOOLEAN}, boolean{b} {}
    explicit Value(char c) : type{Type::CHARACTER}, character{c} {}
    explicit Value(const char* s) : type{Type::STRING} {
      str = new char[strlen(s)]();
      strcpy(str, s);
    }
    explicit Value(Value* a, Value* d) : type{Type::PAIR}, pair{a,d} {}
    Value() : type{Type::PAIR}, pair{nullptr,nullptr} {}
  private:
};

class Reader {
  public:
    Reader(istream& input) : input_stream_{input} {}
    pair<Token, string> tryReadToken();
    Value* tryRead();
  private:
    istream& input_stream_;
    pair<Token, string> tryReadNumber(char first_ch);
    pair<Token, string> tryReadLiteral();
    pair<Token, string> tryReadString();
};

Value True{true};
Value False{false};
Value EmptyList{};

Value* Reader::tryRead() {
  Value* result = nullptr;
  auto token = tryReadToken();
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
      result = new Value(token.second.c_str());
    } break;
    case Token::LPAREN:{
    } break;
    case Token::RPAREN:{
    } break;
    default:{
      throw ParsingError();
    }
  }
  return result;
}

pair<Token, string> Reader::tryReadToken() {
  // attempt to read token, throw exception if failure
  char ch;
  input_stream_.get(ch);
  if(!input_stream_){
      // throw, unable to get a character
  }
  if(ch == '-' || isdigit(ch)){
    return tryReadNumber(ch);
  } else if(ch == '#'){
    return tryReadLiteral();
  } else if(ch == '"'){
    return tryReadString();
  } else if(ch == '('){
    return make_pair(Token::LPAREN, string{"("});
  } else if(ch == ')'){
    return make_pair(Token::RPAREN, string{")"});
  } else if(isspace(ch)){
    return tryReadToken();
  } else {
    // throw, unexpected character
    throw LexingError();
  }
}

pair<Token, string> Reader::tryReadNumber(char first_ch) {
  char ch;
  string result = "";
  result.push_back(first_ch);
  while(input_stream_.get(ch)){
    if(isdigit(ch)){
      result.push_back(ch);
    } else if(isspace(ch)){
      break;
    } else {
      // throw, cant have weird symbols in a number
      throw LexingError();
    }
  }
  return make_pair(Token::NUMBER, result);
}

pair<Token, string> Reader::tryReadLiteral() {
  char ch;
  input_stream_.get(ch);
  if(ch == 't'){
    return make_pair(Token::BOOLEAN, string{"#t"});
  } else if(ch == 'f'){
    return make_pair(Token::BOOLEAN, string{"#f"});
  } else if(ch == '\\'){
    input_stream_.get(ch);
    return make_pair(Token::CHARACTER, string{"#\\"} + ch);
  } else {
    // throw, invalid literal syntax
    throw LexingError();
  }
}

pair<Token, string> Reader::tryReadString() {
  char ch;
  string result = "\"";
  while(input_stream_.get(ch)){
    result.push_back(ch);
    if(ch == '\"'){
      break;
    }
  }
  return make_pair(Token::STRING, result);
}

void print(Value* value) {
  if(!value){
    return;
  }
  switch(value->type){
    case Value::Type::FIXNUM:{
      cout << value->fixnum;
    } break;
    case Value::Type::BOOLEAN:{
      if(value->boolean){
        cout << "True";
      } else {
        cout << "False";
      }
    } break;
    case Value::Type::CHARACTER:{
      cout << "#\\" << value->character;
    } break;
    case Value::Type::STRING:{
      cout << value->str;
    } break;
    case Value::Type::PAIR:{
      cout << "(";
      if(value->pair.car){
        print(value->pair.car);
        cout << " . ";
        print(value->pair.cdr);
      }
      cout << ")";
    } break;
    default:{
    } break;
  }
}

int main(int, char*[]) {
  cout << "Welcome to Crisp. Use ctrl-c to exit.\n";
  Reader reader(cin);

  while(true){
    cout << "crisp> ";
    auto val = reader.tryRead();
    print(val);
    cout << endl;
  }
  return 0;
}
