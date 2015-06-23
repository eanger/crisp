#include <unordered_map>
#include <vector>

#include "value.hpp"
#include "eval.hpp"
#include "eval-impl.hpp"

using namespace std;

namespace crisp{
namespace { // unnamed namespace

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

Value* evalQuote(Value* input, Environment*) {
  return input->car;
};

Value* evalDefine(Value* input, Environment* envt) {
  envt->setBinding(input->car, Binding(doEval(input->cdr->car, envt)));
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
    return doEval(input->cdr->cdr->car, envt);
  }
}

Value* evalLet(Value* input, Environment* envt) {
  auto binding_forms = input->car;
  auto body_forms = input->cdr;

  // make new envt
  Environment* new_envt = new Environment();
  new_envt->parent = envt;
  // walk through all binding forms and add bindings
  for (Value* binding_form = binding_forms; binding_form != nullptr;
       binding_form = binding_form->cdr) {
    auto bound = doEval(binding_form->car->cdr->car, envt);
    Binding bdg(bound);
    new_envt->setBinding(binding_form->car->car, bdg);
  }

  // walk through and evaluate all body forms with the new envt
  Value* res = nullptr;
  for (Value* body_form = body_forms; body_form != nullptr;
       body_form = body_form->cdr) {
    res = doEval(body_form->car, new_envt);
  }
  return res;
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
      if(input == EmptyList){
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
        return doEval(new Value(doEval(input->car, envt), input->cdr), envt);
      } else if(input->car->type == Value::Type::PRIMITIVE_PROCEDURE){
        // TODO: eval the combo (ie apply?)
        Value* proc = input->car;
        Value* args = input->cdr;

        Value* eval_args = nullptr;
        for(Value* arg = args; arg != nullptr; arg = arg->cdr){
          Value* res = doEval(arg->car, envt);
          eval_args = new Value(res, eval_args);
        }
        eval_args = reverse(eval_args);
        return proc->proc(eval_args);
      } else {
        throw EvaluationError("Cannot evaluate a pair that doesn't start with a pair, symbol, or procedure.");
      }
    } break;
    case Value::Type::SYMBOL:{
      return evalSymbol(input, envt);
    } break;
    case Value::Type::PRIMITIVE_PROCEDURE:{
      throw EvaluationError("Trying to evaluate a procedure that's not in a list.");
    } break;
  }
}

Value* addproc(Value* operands){
  long res = 0;
  for(Value* operand = operands; operand != nullptr; operand = operand->cdr){
    res += operand->car->fixnum;
  }
  return new Value(res);
}
} // end unnamed namespace

void initEval() {
  GlobalEnvironment.setBinding(getInternedSymbol("quote"), Binding(evalQuote));
  GlobalEnvironment.setBinding(getInternedSymbol("define"), Binding(evalDefine));
  GlobalEnvironment.setBinding(getInternedSymbol("set!"), Binding(evalSet));
  GlobalEnvironment.setBinding(getInternedSymbol("if"), Binding(evalIf));
  GlobalEnvironment.setBinding(getInternedSymbol("let"), Binding(evalLet));
  Value* add_proc = new Value(addproc);
  GlobalEnvironment.setBinding(getInternedSymbol("add"), Binding(add_proc));
}

Value* eval(Value* input) {
  return doEval(input, &GlobalEnvironment);
}
}

