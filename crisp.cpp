#include <cassert>
#include <cctype>
#include <iostream>
#include <stack>
#include <string>

using namespace std;

class Value {
  public:
    enum class Type {
      FIXNUM,
      BOOLEAN,
    };
    Type type;
    union {
      long fixnum;
      bool boolean;
    };

    explicit Value(long num) : type{Type::FIXNUM}, fixnum{num} {}
    explicit Value(bool b) : type{Type::BOOLEAN}, boolean{b} {}
    Value() {}
  private:
};

Value True(true);
Value False(false);

class Expression {
  public:
    Value val;
  private:
};

bool isDelimiter(char x) {
  return x == ' ';
}

Expression read(stack<char> input) {
  Expression expr;
  char character = input.top();
  input.pop();

  if(character == '#'){ // read a boolean
    character = input.top();
    input.pop();
    if(character == 't'){
      expr.val = True;
    } else if(character == 'f'){
      expr.val = False;
    } else {
      cerr << "Error: Unknown bool literal '" << character << "'\n";
      exit(-1);
    }
  } else {
    long sign = 1;
    if(character == '-'){
      sign = -1;
      character = input.top();
      input.pop();
    }
    if(isdigit(character)){ // read a number
      expr.val.type = Value::Type::FIXNUM;
      expr.val.fixnum = character - '0';
      while(!input.empty()){
        character = input.top();
        input.pop();
        if(isdigit(character)){
          expr.val.fixnum = expr.val.fixnum * 10 + (character - '0');
        } else {
          input.push(character);
          break;
        }
      }
      expr.val.fixnum *= sign;
    }
  }
  if(!input.empty() && !isDelimiter(input.top())){
    cerr << "Error: Invalid character '" << input.top() << "'\n";
    exit(-1);
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
    default:{
      assert(false && "Invalid Value type");
    } break;
  }
}

int main(int, char*[]) {
  cout << "Welcome to Crisp. Use ctrl-c to exit.\n";

  while(true){
    cout << "crisp> ";
    string line;
    getline(cin, line);
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
