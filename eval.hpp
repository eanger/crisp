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
  std::unordered_map<Value*, Binding> bindings;
  Environment* parent;

  Binding* getBinding(Value* value);
  void setBinding(Value* key, Binding binding);
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
/***** Primitive Procedures *****/
Value* addxyproc(Environment* envt);

Value* eval(Value* input, Environment* envt);

extern Environment GlobalEnvironment;
}
