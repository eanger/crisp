#include <cassert>
#include <iostream>
#include <unordered_map>
#include <vector>

#include "value.hpp"
#include "eval.hpp"
#include "eval-impl.hpp"

using namespace std;

namespace crisp{

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

namespace { // unnamed namespace
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

Value* evalLambda(Value* input, Environment* envt) {
  return new Value(input->car, envt, input->cdr);
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
      } else if(input->car->type == Value::Type::PRIMITIVE_PROCEDURE || 
                input->car->type == Value::Type::PROCEDURE){
        Value* proc = input->car;
        Value* args = input->cdr;

        // TODO: this should create a new environment, using the 
        // procedure's store envt as a parent
        // proc->args is a list of argument symbols
        Environment* new_envt = new Environment();
        new_envt->parent = envt;
        for (Value* arg = args, *name = proc->args; arg != nullptr;
             arg = arg->cdr, name = name->cdr) {
          if(!name){
            throw EvaluationError("Mismatching number of arg names and args provided.");
          }
          new_envt->setBinding(name->car, Binding(doEval(arg->car, envt)));
        }

        if(proc->type == Value::Type::PRIMITIVE_PROCEDURE){
          return proc->proc(new_envt);
        }
        // TODO: eval sequence of the proc body, using the new envt
        Value* res = nullptr;
        for (Value* statement = proc->body; statement != nullptr;
             statement = statement->cdr) {
          res = doEval(statement->car, new_envt);
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
} // end unnamed namespace

void initEval() {
  /* Special Forms */
  GlobalEnvironment.setBinding(getInternedSymbol("quote"), Binding(evalQuote));
  GlobalEnvironment.setBinding(getInternedSymbol("define"), Binding(evalDefine));
  GlobalEnvironment.setBinding(getInternedSymbol("set!"), Binding(evalSet));
  GlobalEnvironment.setBinding(getInternedSymbol("if"), Binding(evalIf));
  GlobalEnvironment.setBinding(getInternedSymbol("let"), Binding(evalLet));
  GlobalEnvironment.setBinding(getInternedSymbol("lambda"), Binding(evalLambda));

  /* Primitive Procedures */
  // need args and envt
  Value* args = new Value(getInternedSymbol("x"), new Value(getInternedSymbol("y"), nullptr));
  Value* addxy_proc = new Value(args, &GlobalEnvironment, addxyproc);
  GlobalEnvironment.setBinding(getInternedSymbol("addxy"), Binding(addxy_proc));
}

Value* eval(Value* input) {
  return doEval(input, &GlobalEnvironment);
}
}

