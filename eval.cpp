#include <unordered_map>
#include <vector>

#include "value.hpp"
#include "eval.hpp"

using namespace std;

namespace crisp{
namespace { // unnamed namespace

struct Environment;
struct Binding{
  using SpecialForm = Value*(*)(Value*, Environment*);
  enum class Type{
    VARIABLE, SPECIALFORM
  };
  Type type;
  union{ 
    Value* variable;
    SpecialForm special_form;
  };
  explicit Binding(Value* v) : type{Type::VARIABLE}, variable{v} {}
  explicit Binding(SpecialForm f) : type{Type::SPECIALFORM}, special_form{f} {}
  explicit Binding() {}
};

struct Environment{
  unordered_map<Value*, Binding> bindings;
  Environment* parent;

  Binding* getBinding(Value* value);
  void setBinding(Value* key, Binding binding);
  Environment() : bindings{}, parent{nullptr} {}
};

Binding* Environment::getBinding(Value* value) {
  auto bdg = bindings.find(value);
  if(bdg != end(bindings)){
    return &bdg->second;
  } else{
    if(parent){
      return parent->getBinding(value);
    } else {
      return nullptr;
    }
  }
}

void Environment::setBinding(Value* key, Binding binding) {
  bindings[key] = binding;
}

Environment GlobalEnvironment;

Value* doEval(Value* input, Environment* envt);

Value* evalQuote(Value* input, Environment*) {
  return input;
};

Value* evalDefine(Value* input, Environment* envt) {
  envt->setBinding(input->car, Binding(doEval(input->cdr, envt)));
  // MUST return null, since define has no printed result
  return nullptr;
}

Value* evalSet(Value* input, Environment* envt) {
  auto bdg = envt->getBinding(input->car);
  if(!bdg){
    throw EvaluationError("Cannot set undefined symbol");
  }
  // returns null, since set! has no printed result
  return evalDefine(input, envt);
}

Value* evalSymbol(Value* symbol, Environment* envt) {
  // should return the value this symbol was bound to
  auto bdg = envt->getBinding(symbol);
  if(!bdg){
    throw EvaluationError("Cannot evaluate undefined symbol");
  }
  if(bdg->type == Binding::Type::VARIABLE){
    return bdg->variable;
  } else {
    throw EvaluationError("Non-variable name referenced as a variable");
  }
}

Value* evalIf(Value* input, Environment* envt) {
  if(doEval(input->car,envt) != &False){
    return doEval(input->cdr->car, envt);
  } else {
    return doEval(input->cdr->cdr, envt);
  }
}

Value* doEval(Value* input, Environment* envt) {
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
      } else if(input->car->type == Value::Type::SYMBOL){
        auto binding = envt->getBinding(input->car);
        switch(binding->type){
          case Binding::Type::VARIABLE:{
            Value val(doEval(input->car, envt), input->cdr);
            return doEval(&val, envt);
          } break;
          case Binding::Type::SPECIALFORM:{
            return binding->special_form(input->cdr, envt);
          } break;
        }
      } else if(input->car->type == Value::Type::PAIR){
        // evaluate the car, and use its result to evaluate the cdr
        Value val(doEval(input->car, envt), input->cdr);
        return doEval(&val, envt);
      } else {
        throw EvaluationError("Cannot evaluate a pair with non-symbol, non-pair car");
      }
    } break;
    case Value::Type::SYMBOL:{
      return evalSymbol(input, envt);
    } break;
  }
}
} // end unnamed namespace

void initEval() {
  Quote = getInternedSymbol("quote");
  GlobalEnvironment.setBinding(Quote, Binding(evalQuote));
  Define = getInternedSymbol("define");
  GlobalEnvironment.setBinding(Define, Binding(evalDefine));
  Set = getInternedSymbol("set!");
  GlobalEnvironment.setBinding(Set, Binding(evalSet));
  If = getInternedSymbol("if");
  GlobalEnvironment.setBinding(If, Binding(evalIf));
}

Value* eval(Value* input) {
  return doEval(input, &GlobalEnvironment);
}
}

