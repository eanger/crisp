#include <cassert>
#include <iostream>
#include <unordered_map>
#include <vector>

#include "value.hpp"
#include "eval.hpp"
#include "read.hpp"

using namespace std;

namespace crisp{
Environment GlobalEnvironment(nullptr);

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

void initEval() {
  /* Special Forms */
  GlobalEnvironment.setBinding(getInternedSymbol("quote"), Binding(Quote));
  GlobalEnvironment.setBinding(getInternedSymbol("define"), Binding(Define));
  GlobalEnvironment.setBinding(getInternedSymbol("set!"), Binding(Set));
  GlobalEnvironment.setBinding(getInternedSymbol("if"), Binding(If));
  GlobalEnvironment.setBinding(getInternedSymbol("let"), Binding(Let));
  GlobalEnvironment.setBinding(getInternedSymbol("lambda"), Binding(Lambda));

  /* Primitive Procedures */
  // need args and envt
  Value* args = new Value(getInternedSymbol("x"), new Value(getInternedSymbol("y"), nullptr));
  Value* addxy_proc = new Value(args, &GlobalEnvironment, addxyproc);
  GlobalEnvironment.setBinding(getInternedSymbol("addxy"), Binding(addxy_proc));
  args = new Value(getInternedSymbol("input"), nullptr);
  Value* read_proc = new Value(args, &GlobalEnvironment, read);
  GlobalEnvironment.setBinding(getInternedSymbol("read"), Binding(read_proc));
}

Value* doEval(Value* input){
  return eval(input, &GlobalEnvironment);
}

Value* eval(Value* input, Environment* envt) {
  if(!input){
    return input;
  }
  switch(input->type){
    case Value::Type::FIXNUM:
    case Value::Type::BOOLEAN:
    case Value::Type::CHARACTER:
    case Value::Type::STRING:{
      return input;
    } break;
    case Value::Type::PAIR:{
      if(input->car->type == Value::Type::SYMBOL){
        auto binding = envt->getBinding(input->car);
        if(!binding){
          throw EvaluationError("Cannot call undefined symbol.");
        }
        switch(binding->type){
          case Binding::Type::VARIABLE:{
            Value val(eval(input->car, envt), input->cdr);
            return eval(&val, envt);
          } break;
          case Binding::Type::SPECIALFORM:{
            return binding->special_form(input->cdr, envt);
          } break;
        }
      } else if(input->car->type == Value::Type::PAIR){
        return eval(new Value(eval(input->car, envt), input->cdr), envt);
      } else if(input->car->type == Value::Type::PRIMITIVE_PROCEDURE || 
                input->car->type == Value::Type::PROCEDURE){
        Value* proc = input->car;
        Value* args = input->cdr;

        // TODO: this should create a new environment, using the 
        // procedure's store envt as a parent
        // proc->args is a list of argument symbols
        Environment* new_envt = new Environment(envt);
        for (Value* arg = args, *name = proc->args; arg != nullptr;
             arg = arg->cdr, name = name->cdr) {
          if(!name){
            throw EvaluationError("Mismatching number of arg names and args provided.");
          }
          new_envt->setBinding(name->car, Binding(eval(arg->car, envt)));
        }

        if(proc->type == Value::Type::PRIMITIVE_PROCEDURE){
          return proc->proc(new_envt);
        }
        // TODO: eval sequence of the proc body, using the new envt
        Value* res = nullptr;
        for (Value* statement = proc->body; statement != nullptr;
             statement = statement->cdr) {
          res = eval(statement->car, new_envt);
        }
        return res;
      } else {
        throw EvaluationError("Cannot evaluate a pair that doesn't start with a pair, symbol, or procedure.");
      }
    } break;
    case Value::Type::SYMBOL:{
      return evalSymbol(input, envt);
    } break;
    case Value::Type::PROCEDURE:
    case Value::Type::PRIMITIVE_PROCEDURE:{
      throw EvaluationError("Trying to evaluate a procedure that's not in a list.");
    } break;
  }
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

/***** Special Forms *****/
Value* Quote(Value* input, Environment*) {
  return input->car;
};

Value* Define(Value* input, Environment* envt) {
  envt->setBinding(input->car, Binding(eval(input->cdr->car, envt)));
  // MUST return null, since define has no printed result
  return nullptr;
}

Value* Set(Value* input, Environment* envt) {
  auto bdg = envt->getBinding(input->car);
  if(!bdg){
    throw EvaluationError("Cannot set undefined symbol");
  }
  // returns null, since set! has no printed result
  return Define(input, envt);
}

Value* If(Value* input, Environment* envt) {
  if(eval(input->car,envt) != &False){
    return eval(input->cdr->car, envt);
  } else {
    return eval(input->cdr->cdr->car, envt);
  }
}

Value* Let(Value* input, Environment* envt) {
  auto binding_forms = input->car;
  auto body_forms = input->cdr;

  // make new envt
  Environment* new_envt = new Environment(envt);
  // walk through all binding forms and add bindings
  for (Value* binding_form = binding_forms; binding_form != nullptr;
       binding_form = binding_form->cdr) {
    auto bound = eval(binding_form->car->cdr->car, envt);
    Binding bdg(bound);
    new_envt->setBinding(binding_form->car->car, bdg);
  }

  // walk through and evaluate all body forms with the new envt
  Value* res = nullptr;
  for (Value* body_form = body_forms; body_form != nullptr;
       body_form = body_form->cdr) {
    res = eval(body_form->car, new_envt);
  }
  return res;
}

Value* Lambda(Value* input, Environment* envt) {
  return new Value(input->car, envt, input->cdr);
}

/***** Primitive Procedures *****/
// this dummy procedure adds the value of x with the value of y
Value* addxyproc(Environment* envt){
  auto x = getInternedSymbol("x");
  auto xbdg = envt->getBinding(x);
  auto y = getInternedSymbol("y");
  auto ybdg = envt->getBinding(y);
  if(!xbdg || !ybdg){
    throw EvaluationError("Unable to get parameter for procedure.");
  }

  long res = xbdg->variable->fixnum + ybdg->variable->fixnum;
  return new Value(res);
}

}

