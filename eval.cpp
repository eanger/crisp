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

Value* Environment::getBinding(Value* value) {
  auto bdg = bindings.find(value);
  if(bdg != end(bindings)){
    return bdg->second;
  } else{
    if(parent){
      return parent->getBinding(value);
    } else {
      return nullptr;
    }
  }
}

Value* Environment::getSymbolBinding(const std::string& key){
  return getBinding(getInternedSymbol(key));
}

void Environment::setBinding(Value* key, Value* binding) {
  bindings[key] = binding;
}

void Environment::setSymbolBinding(const std::string& key, Value* binding) {
  setBinding(getInternedSymbol(key), binding);
}

void initEval() {
  /* Special Forms */
  GlobalEnvironment.setSymbolBinding("quote", new Value(Quote));
  GlobalEnvironment.setSymbolBinding("define", new Value(Define));
  GlobalEnvironment.setSymbolBinding("set!", new Value(Set));
  GlobalEnvironment.setSymbolBinding("if", new Value(If));
  GlobalEnvironment.setSymbolBinding("let", new Value(Let));
  GlobalEnvironment.setSymbolBinding("lambda", new Value(Lambda));
  GlobalEnvironment.setSymbolBinding("quasiquote", new Value(Quasiquote));
  GlobalEnvironment.setSymbolBinding("unquote", new Value(Unquote));

  /* Primitive Procedures */
  Value* args = new Value(getInternedSymbol("x"), new Value(getInternedSymbol("y"), EmptyList));
  Value* addxy_proc = new Value(args, &GlobalEnvironment, addxyproc);
  GlobalEnvironment.setSymbolBinding("addxy", addxy_proc);

  args = new Value(getInternedSymbol("input"), EmptyList);
  Value* read_proc = new Value(args, &GlobalEnvironment, read);
  GlobalEnvironment.setSymbolBinding("read", read_proc);

  args = new Value(getInternedSymbol("x"), new Value(getInternedSymbol("y"), EmptyList));
  Value* cons_proc = new Value(args, &GlobalEnvironment, cons);
  GlobalEnvironment.setSymbolBinding("cons", cons_proc);
  
  args = getInternedSymbol("args");
  Value* add_proc = new Value(args, &GlobalEnvironment, add);
  GlobalEnvironment.setSymbolBinding("add", add_proc);

  args = new Value(
      getInternedSymbol("first"),
      new Value(getInternedSymbol("second"), getInternedSymbol("rest")));
  Value* add2ormore_proc = new Value(args, &GlobalEnvironment, add2ormore);
  GlobalEnvironment.setSymbolBinding("add2ormore", add2ormore_proc);
}

Value* doEval(Value* input){
  return eval(input, &GlobalEnvironment);
}

Value* eval(Value* input, Environment* envt) {
  if(!input || input == EmptyList){
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
        if(binding->type == Value::Type::SPECIAL_FORM){
          return binding->special_form(input->cdr, envt);
        }
        return eval(new Value(eval(input->car, envt), input->cdr), envt);
      } else if(input->car->type == Value::Type::PAIR){
        return eval(new Value(eval(input->car, envt), input->cdr), envt);
      } else if(input->car->type == Value::Type::PROCEDURE){
        Value* proc = input->car;
        Value* args = input->cdr;

        // First, evaluate all parameters
        Value* eval_params = EmptyList;
        for(Value* arg = args; arg != EmptyList; arg = arg->cdr){
          eval_params = new Value(eval(arg->car, envt), eval_params);
        }
        Value* params = reverse(eval_params);

        // Second, bind parameters to names. this may not be one to one (variadic)
        Environment* new_envt = new Environment(proc->envt);
        for(Value* param = params, *name = proc->args; name != EmptyList;
             param = param->cdr, name = name->cdr) {
          if(name->type != Value::Type::PAIR){
            new_envt->setBinding(name, param);
            break;
          }
          if(param == EmptyList){
            throw EvaluationError("Missing required arguments.");
          }
          new_envt->setBinding(name->car, param->car);
        }

        if(proc->is_primitive){
          return proc->prim_procedure(new_envt);
        }
        // eval sequence of the proc body, using the new envt
        Value* res = nullptr;
        for (Value* statement = proc->body; statement != EmptyList;
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
    case Value::Type::SPECIAL_FORM:
    case Value::Type::PROCEDURE:{
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
  return bdg;
}

/***** Special Forms *****/
Value* Quote(Value* input, Environment*) {
  return input->car;
};

Value* Define(Value* input, Environment* envt) {
  envt->setBinding(input->car, eval(input->cdr->car, envt));
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
    new_envt->setBinding(binding_form->car->car, bound);
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

Value* Quasiquote(Value* input, Environment* envt){
  return doQuasiquote(input->car, envt);
}

// This form takes the list of values left to be quasiquoted
// ((unquote (add 1 2)))
Value* doQuasiquote(Value* input, Environment* envt){
  if(input->type != Value::Type::PAIR || input == EmptyList){
    return input;
  }
  auto first_subexpr = input->car; // (unquote (add 1 2))
  auto rest_subexprs = input->cdr; // ()
  if(first_subexpr->type == Value::Type::PAIR &&
     first_subexpr->car == getInternedSymbol("unquote")){
    // unquote, ie evaluate, the unquoted form
    first_subexpr = eval(first_subexpr->cdr->car, envt);
  }
  // take resulting first_subexpr (which may have been evaluated)
  // and continue quasiquoting rest_subexprs
  return new Value(first_subexpr, doQuasiquote(rest_subexprs, envt));
}

Value* Unquote(Value*, Environment*){
  throw EvaluationError("Unquote can only happen inside quasiquote.");
}

/***** Primitive Procedures *****/
// this dummy procedure adds the value of x with the value of y
Value* addxyproc(Environment* envt){
  auto x = envt->getSymbolBinding("x");
  auto y = envt->getSymbolBinding("y");
  if(!x || !y){
    throw EvaluationError("Unable to get parameter for procedure.");
  }
  if(x->type != Value::Type::FIXNUM || y->type != Value::Type::FIXNUM){
    throw EvaluationError("Unable to add two values that aren't fixnums.");
  }

  long res = x->fixnum + y->fixnum;
  return new Value(res);
}

Value* cons(Environment* envt){
  auto x = envt->getSymbolBinding("x");
  auto y = envt->getSymbolBinding("y");
  if(!x || !y){
    throw EvaluationError("Unable to get parameter for procedure.");
  }
  return new Value(eval(x, envt), eval(y, envt));
}

Value* add(Environment* envt){
  auto args = envt->getSymbolBinding("args");
  // args is a list of things to add
  if(!args || args->type != Value::Type::PAIR){
    throw EvaluationError("Must pass a list to add, even if its empty");
  }
  long res = 0;
  for(Value* arg = args; arg != EmptyList; arg = arg->cdr){
    if(arg->car->type != Value::Type::FIXNUM){
      throw EvaluationError("Can only add things that evaluate to numbers.");
    }
    res += arg->car->fixnum;
  }
  return new Value{res};
}

Value* add2ormore(Environment* envt){
  auto first = envt->getSymbolBinding("first");
  auto second = envt->getSymbolBinding("second");
  auto rest = envt->getSymbolBinding("rest");
  if(!first || !second || !rest){
    throw EvaluationError("Insufficient parameters, expects two or more.");
  }
  if(first->type != Value::Type::FIXNUM || second->type != Value::Type::FIXNUM){
    throw EvaluationError("Can only add things that evaluate to numbers.");
  }
  long res = first->fixnum + second->fixnum;
  for(Value* arg = rest; arg != EmptyList; arg = arg->cdr){
    if(arg->car->type != Value::Type::FIXNUM){
      throw EvaluationError("Can only add things that evaluate to numbers.");
    }
    res += arg->car->fixnum;
  }
  return new Value(res);
}

}

