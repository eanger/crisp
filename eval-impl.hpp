#pragma once

#include <unordered_map>

#include "value.hpp"

namespace crisp{
namespace{

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
  Environment() : bindings{}, parent{nullptr} {}
};

Value* doEval(Value* input, Environment* envt);
Value* evalQuote(Value* input, Environment*);
Value* evalDefine(Value* input, Environment* envt);
Value* evalSet(Value* input, Environment* envt);
Value* evalSymbol(Value* symbol, Environment* envt);
Value* evalIf(Value* input, Environment* envt);
Value* evalLet(Value* input, Environment* envt);

}// end unnamed namespace
}
