#pragma once
#include <unordered_map>

#include "value.hpp"
#include "exception.hpp"

namespace crisp{

/***** Exceptions *****/
class EvaluationError : public VerboseError {
  public:
    EvaluationError(const char* problem) : VerboseError("Eval error: ", problem) {}
};

/***** Classes *****/
struct Environment{
  std::unordered_map<Value*, Value*> bindings;
  Environment* parent;

  Value* getBinding(Value* value);
  void setBinding(Value* key, Value* binding);
  Environment(Environment* e) : bindings{}, parent{e} {}
};

/***** Function *****/
void initEval();
Value* doEval(Value* input);
Value* evalSymbol(Value* symbol, Environment* envt);

/***** Special Forms *****/
Value* Quote(Value* input, Environment*);
Value* Define(Value* input, Environment* envt);
Value* Set(Value* input, Environment* envt);
Value* If(Value* input, Environment* envt);
Value* Let(Value* input, Environment* envt);
Value* Lambda(Value* input, Environment* envt);
Value* doQuasiquote(Value* input, Environment* envt);
Value* Quasiquote(Value* input, Environment* envt);
Value* Unquote(Value* input, Environment* envt);
/***** Primitive Procedures *****/
Value* addxyproc(Environment* envt);
Value* cons(Environment* envt);
Value* add(Environment* envt);
Value* add2ormore(Environment* envt);

Value* eval(Value* input, Environment* envt);

extern Environment GlobalEnvironment;
}
