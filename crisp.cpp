#include <algorithm>
#include <cassert>
#include <cctype>
#include <exception>
#include <iostream>
#include <limits>
#include <stack>
#include <string>
#include <utility>
#include <vector>

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
  NUMBER, BOOLEAN, CHARACTER, STRING, LPAREN, RPAREN, DOT, SYMBOL
};

bool isDelimiter(char c){
  return isspace(c) || (string{"()[]\";#"}.find(c) != string::npos);
}

class Value {
  public:
    enum class Type {
      FIXNUM,
      BOOLEAN,
      CHARACTER,
      STRING,
      PAIR,
      SYMBOL
    };
    Type type;
    struct Pair{
      Value* car;
      Value* cdr;
    };
    struct Str{
      char* str;
    };
    struct Sym{
      char* name;
    };
    union {
      long fixnum;
      bool boolean;
      char character;
      Str str;
      Pair pair;
      Sym symbol;
    };

    explicit Value(long n) : type{Type::FIXNUM}, fixnum{n} {}
    explicit Value(bool b) : type{Type::BOOLEAN}, boolean{b} {}
    explicit Value(char c) : type{Type::CHARACTER}, character{c} {}
    explicit Value(Str s) : type{Type::STRING}, str(s) {}
    explicit Value(Value* a, Value* d) : type{Type::PAIR}, pair{a,d} {}
    explicit Value(Sym s) : type{Type::SYMBOL}, symbol(s) {}
    Value() : type{Type::PAIR}, pair{nullptr,nullptr} {}
  private:
};

class Reader {
  public:
    Reader(istream& input) : input_stream_{input} {}
    pair<Token, string> tryReadToken();
    Value* read();
  private:
    istream& input_stream_;
    pair<Token, string> tryReadNumber(char first_ch);
    pair<Token, string> tryReadLiteral();
    pair<Token, string> tryReadString();
    pair<Token, string> tryReadSymbol(char first_ch);
    Value* tryReadPair();
    Value* parse(const pair<Token, string>& token);
};

Value True{true};
Value False{false};
Value EmptyPair{};
vector<Value*> Symbols;
Value* Quote;

Value* Reader::read() {
  return parse(tryReadToken());
}

Value* getOrCreateSymbol(const string& name) {
  auto symbol_itr = find_if(begin(Symbols), end(Symbols),
                            [&](Value* v){ 
                              return !strcmp(v->symbol.name, name.c_str());
                            });
  if(symbol_itr == end(Symbols)){
    // add new symbol
    Value::Sym sym;
    sym.name = new char[name.size()]();
    strcpy(sym.name, name.c_str());
    Value* symbol = new Value(sym);
    Symbols.push_back(symbol);
    symbol_itr = Symbols.end() - 1;
  }
  return *symbol_itr;
}

Value* Reader::parse(const pair<Token, string>& token) {
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
      result = tryReadPair();
    } break;
    case Token::SYMBOL:{
      return getOrCreateSymbol(token.second);
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

Value* Reader::tryReadPair() {
  auto token = tryReadToken();
  if(token.first  == Token::RPAREN){
    return &EmptyPair;
  }
  Value* first = parse(token);
  if(first == nullptr){
    throw ParsingError();
  }
  auto dot = tryReadToken();
  // must be a dot
  if(dot.first != Token::DOT){
    throw ParsingError();
  }
  token = tryReadToken();
  // second can't be an end-paren
  if(token.first == Token::RPAREN){
    throw ParsingError();
  }
  Value* second = parse(token);

  auto rparen = tryReadToken();
  // must end in an rparen
  if(rparen.first != Token::RPAREN){
    throw ParsingError();
  }
  return new Value{first, second};
}

pair<Token, string> Reader::tryReadToken() {
  // attempt to read token, throw exception if failure
  char ch;
  input_stream_.get(ch);
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
  } else if(ch == '.'){
    if(isDelimiter(input_stream_.peek())){
      return make_pair(Token::DOT, string{"."});
    }
    throw LexingError();
  } else if(isalpha(ch) || (string{"!$%&*/:<=>?^_~"}.find(ch) != string::npos)){
    return tryReadSymbol(ch);
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
    } else if(isDelimiter(ch)){
      input_stream_.unget();
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
    if(isDelimiter(input_stream_.peek())){
      return make_pair(Token::BOOLEAN, string{"#t"});
    }
    throw LexingError();
  } else if(ch == 'f'){
    if(isDelimiter(input_stream_.peek())){
      return make_pair(Token::BOOLEAN, string{"#f"});
    }
    throw LexingError();
  } else if(ch == '\\'){
    input_stream_.get(ch);
    if(isDelimiter(input_stream_.peek())){
      return make_pair(Token::CHARACTER, string{"#\\"} + ch);
    }
    throw LexingError();
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
      if(isDelimiter(input_stream_.peek())){
        break;
      }
      throw LexingError();
    }
  }
  return make_pair(Token::STRING, result);
}

pair<Token, string> Reader::tryReadSymbol(char first_ch) {
  string result = "";
  result.push_back(first_ch);
  char ch;
  while(input_stream_.get(ch)){
    if(isDelimiter(ch)){
      input_stream_.unget();
      break;
    } else if(isalnum(ch) || 
              (string{"!$%&*/:<=>?^_~+-.@"}.find(ch) != string::npos)){
      result.push_back(ch);
    } else {
      throw LexingError();
    }
  }
  return make_pair(Token::SYMBOL, result);
}

class Evaluator {
  public:
    Evaluator() {}
    Value* eval(Value* input);
  private:
};

Value* Evaluator::eval(Value* input) {
  switch(input->type){
    case Value::Type::FIXNUM:
    case Value::Type::BOOLEAN:
    case Value::Type::CHARACTER:
    case Value::Type::STRING:{
      return input;
    } break;
    case Value::Type::PAIR:{
      if(input->pair.car == Quote){
        return input->pair.cdr;
      }
      return nullptr;
    } break;
    case Value::Type::SYMBOL:{
      return nullptr;
    } break;
  }
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
      cout << value->str.str;
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
    case Value::Type::SYMBOL:{
      cout << value->symbol.name;
    } break;
  }
}

int main(int, char*[]) {
  cout << "Welcome to Crisp. Use ctrl-c to exit.\n";
  Reader reader(cin);
  Evaluator evaluator;
  Quote = getOrCreateSymbol("quote");

  while(true){
    cout << "crisp> ";
    try{
      auto val = reader.read();
      auto res = evaluator.eval(val);
      print(res);
    } catch(const exception& e){
      cout << e.what();
      cin.clear();
      cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
    cout << endl;
  }
  return 0;
}
