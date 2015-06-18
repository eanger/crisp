#include <cassert>
#include <cctype>
#include <iostream>
#include <stack>
#include <string>

using namespace std;

class Value {
  public:
    enum class Type {
      EMPTY_LIST,
      FIXNUM,
      BOOLEAN,
      CHARACTER,
      STRING,
    };
    Type type;
    union {
      long fixnum;
      bool boolean;
      char character;
      char* str;
    };

    explicit Value(long n) : type{Type::FIXNUM}, fixnum{n} {}
    explicit Value(bool b) : type{Type::BOOLEAN}, boolean{b} {}
    explicit Value(char c) : type{Type::CHARACTER}, character{c} {}
    explicit Value(const char* s) : type{Type::STRING} {
      str = new char[strlen(s)]();
      strcpy(str, s);
    }
    Value() : type{Type::EMPTY_LIST} {}
  private:
};

Value True{true};
Value False{false};
Value EmptyList{};

class Expression {
  public:
    Value val;
  private:
};

bool isDelimiter(char x) {
  return x == ' ' || x == EOF;
}

Expression read(stack<char> input) {
  Expression expr;
  if(!input.empty()){
    char character = input.top();
    input.pop();

    if(character == '#'){ // read a boolean
      character = input.top();
      input.pop();
      if(character == 't'){
        expr.val = True;
      } else if(character == 'f'){
        expr.val = False;
      } else if(character == '\\'){
        if(input.empty()){
          cerr << "Error: Empty character literal\n";
          return expr;
        }
        character = input.top();
        input.pop();
        expr.val = Value(character);
      } else {
        cerr << "Error: Unknown char literal '" << character << "'\n";
        return expr;
      }
    } else if(character == '-' || isdigit(character)){
      long sign = 1;
      if(character == '-'){
        sign = -1;
        character = input.top();
        input.pop();
      }
      if(isdigit(character)){ // read a number
        long num = character - '0';
        while(!input.empty()){
          character = input.top();
          input.pop();
          if(isdigit(character)){
            num = num * 10 + (character - '0');
          } else {
            input.push(character);
            break;
          }
        }
        num *= sign;
        expr.val = Value(num);
      }
    } else if(character == '"'){
      string s = "";
      bool success = false;
      while(!input.empty()){
        character = input.top();
        input.pop();
        if(character == '"'){
          success = true;
          break;
        }
        s.push_back(character);
      }
      if(!success){
        cerr << "Error: Invalid string format\n";
        return expr;
      } else {
        expr.val = Value(s.c_str());
      }
    } else if(character == '('){
      bool success = false;
      if(!input.empty()){
        character = input.top();
        input.pop();
        if(character == ')'){
          expr.val = EmptyList;
          success = true;
        }
      }
      if(!success){
        cerr << "Error: Invalid empty list specification\n";
        return expr;
      }
    } else {
      input.push(character);
    }
    if(!input.empty() && !isDelimiter(input.top())){
      cerr << "Error: Invalid character '" << input.top() << "'\n";
      return expr;
    }
  }

  return expr;
}

Value eval(Expression ast) {
  return ast.val;
}

void print(Value value) {
  switch(value.type){
    case Value::Type::FIXNUM:{
      cout << value.fixnum << "\n";
    } break;
    case Value::Type::BOOLEAN:{
      if(value.boolean){
        cout << "True\n";
      } else {
        cout << "False\n";
      }
    } break;
    case Value::Type::CHARACTER:{
      cout << "#\\" << value.character << "\n";
    } break;
    case Value::Type::STRING:{
      cout << value.str << "\n";
    } break;
    case Value::Type::EMPTY_LIST:{
      cout << "()\n";
    } break;
    default:{
    } break;
  }
}

int main(int, char*[]) {
  cout << "Welcome to Crisp. Use ctrl-c to exit.\n";

  while(true){
    cout << "crisp> ";
    string line;
    getline(cin, line);
    if(cin.eof()){
      cout << "\nShutting down\n";
      return 0;
    }
    stack<char> input;
    for(auto x = line.rbegin(); x != line.rend(); ++x){
      input.push(*x);
    }
    auto ast = read(input);
    auto res = eval(ast);
    print(res);
  }
  assert(false && "Should never reach here");
  return 0;
}
