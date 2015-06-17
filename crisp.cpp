#include <cassert>
#include <cctype>
#include <iostream>
#include <string>

using namespace std;

class Value {
  public:
    enum class Type {
      FIXNUM,
    };
    Type type;
    union {
      long fixnum;
    };
  private:
};

class Expression {
  public:
    Value val;
  private:
};

bool isDelimiter(char x) {
  return x == ' ';
}

Expression read(string line) {
  long sign = 1;
  unsigned idx = 0;
  Expression expr;
  expr.val.type = Value::Type::FIXNUM;
  expr.val.fixnum = 0l;
  if(line[0] == '-'){
    sign = -1;
    ++idx;
  }
  for(; idx < line.size(); ++idx){
    auto character = line[idx];
    if(isdigit(character)){
      expr.val.fixnum = expr.val.fixnum * 10 + (character - '0');
    } else {
      break;
    }
  }
  if(idx != line.size() && !isDelimiter(line[idx])){
    cerr << "Error: Invalid character '" << line[idx] << "'\n";
    exit(-1);
  }
  expr.val.fixnum *= sign;
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
    auto ast = read(line);
    auto res = eval(ast);
    print(res);
  }
  assert(false && "Should never reach here");
  return 0;
}
