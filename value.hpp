#pragma once

#include <string>

namespace crisp{

struct Environment;
class Value;

using PrimitiveProcedure = Value*(*)(Value* args);
class Value {
  public:
    enum class Type {
      FIXNUM,
      BOOLEAN,
      CHARACTER,
      STRING,
      PAIR,
      SYMBOL,
      PRIMITIVE_PROCEDURE
    };
    Type type;
    struct Str{
      char* str;
    };
    struct Sym{
      char* name;
      Sym(const char* n);
    };
    union {
      long fixnum;
      bool boolean;
      char character;
      Str str;
      struct { // for pair
        Value* car;
        Value* cdr;
      };
      Sym symbol;
      PrimitiveProcedure proc; // for primitive proc
      //Value* body; // for proc
    };

    explicit Value(long n) : type{Type::FIXNUM}, fixnum{n} {}
    explicit Value(bool b) : type{Type::BOOLEAN}, boolean{b} {}
    explicit Value(char c) : type{Type::CHARACTER}, character{c} {}
    explicit Value(Str s) : type{Type::STRING}, str(s) {}
    explicit Value(Value* a, Value* d) : type{Type::PAIR}, car{a}, cdr{d} {}
    explicit Value(Sym s) : type{Type::SYMBOL}, symbol(s) {}
    explicit Value(PrimitiveProcedure p)
        : type{Type::PRIMITIVE_PROCEDURE}, proc{p} {}
    Value() : type{Type::PAIR} {}
  private:
};

Value* getInternedSymbol(const std::string& name);
void print(Value* val);
Value* reverse(Value* list);

extern Value True;
extern Value False;
extern Value* EmptyList;

}
