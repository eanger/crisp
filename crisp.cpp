#include <algorithm>
#include <cassert>
#include <cctype>
#include <exception>
#include <iostream>
#include <limits>
#include <stack>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "read.hpp"
#include "value.hpp"

using namespace std;
using namespace crisp;

class EvaluationError: public exception {
  virtual const char* what() const throw(){
    return "Unable to evaluate command.";
  }
};

unordered_map<Value*, Value*> Global_Environment;

class Evaluator {
  public:
    Evaluator() {}
    Value* eval(Value* input);
  private:
    Value* evalDefine(Value* input);
    Value* evalSet(Value* input);
    Value* evalSymbol(Value* symbol);
    Value* evalIf(Value* input);
};

Value* Evaluator::evalDefine(Value* input) {
  Global_Environment[input->car] = eval(input->cdr);
  // MUST return null, since define has no printed result
  return nullptr;
}

Value* Evaluator::evalSet(Value* input) {
  if(Global_Environment.find(input->car) == end(Global_Environment)){
    throw EvaluationError();
  }
  // technically still returns null, since set! has no printed result
  return evalDefine(input);
}

Value* Evaluator::evalSymbol(Value* symbol) {
  // should return the value this symbol was bound to

  auto binding_it = Global_Environment.find(symbol);
  if(binding_it == end(Global_Environment)){
    throw EvaluationError();
  }
  return eval(binding_it->second);
}

Value* Evaluator::evalIf(Value* input) {
  if(input->car != &False){
    return eval(input->cdr->car);
  } else {
    return eval(input->cdr->cdr);
  }
}

Value* Evaluator::eval(Value* input) {
  switch(input->type){
    case Value::Type::FIXNUM:
    case Value::Type::BOOLEAN:
    case Value::Type::CHARACTER:
    case Value::Type::STRING:{
      return input;
    } break;
    case Value::Type::PAIR:{
      if(input == &EmptyPair){
        return input;
      }
      if(input->car == Quote){
        return input->cdr;
      }
      if(input->car == Define){
        return evalDefine(input->cdr);
      }
      if(input->car == Set){
        return evalSet(input->cdr);
      }
      if(input->car == If){
        return evalIf(input->cdr);
      }
      return nullptr;
    } break;
    case Value::Type::SYMBOL:{
      return evalSymbol(input);
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
      if(value->car){
        print(value->car);
        cout << " . ";
        print(value->cdr);
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
  Evaluator evaluator;
  Quote = getInternedSymbol("quote");
  Define = getInternedSymbol("define");
  Set = getInternedSymbol("set!");
  If = getInternedSymbol("if");

  while(true){
    cout << "crisp> ";
    try{
      auto val = read(cin);
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
