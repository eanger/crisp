#include <unordered_map>

#include "eval.hpp"

using namespace std;

namespace crisp{
namespace { // unnamed namespace

unordered_map<Value*, Value*> Global_Environment;

Value* evalDefine(Value* input) {
  Global_Environment[input->car] = eval(input->cdr);
  // MUST return null, since define has no printed result
  return nullptr;
}

Value* evalSet(Value* input) {
  if(Global_Environment.find(input->car) == end(Global_Environment)){
    throw EvaluationError("Cannot set undefined symbol");
  }
  // technically still returns null, since set! has no printed result
  return evalDefine(input);
}

Value* evalSymbol(Value* symbol) {
  // should return the value this symbol was bound to

  auto binding_it = Global_Environment.find(symbol);
  if(binding_it == end(Global_Environment)){
    throw EvaluationError("Cannot evaluate undefined symbol");
  }
  return eval(binding_it->second);
}

Value* evalIf(Value* input) {
  if(input->car != &False){
    return eval(input->cdr->car);
  } else {
    return eval(input->cdr->cdr);
  }
}
} // end unnamed namespace

Value* eval(Value* input) {
  if(!input){
    throw EvaluationError("Cannot evaluate null Value");
  }
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

}

