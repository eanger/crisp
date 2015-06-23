#pragma once

#include <string>

namespace crisp{

struct Environment;
class Value;

using PrimitiveProcedure = Value*(*)(Environment* envt);
class Value {
  public:
    enum class Type {
      FIXNUM,
      BOOLEAN,
      CHARACTER,
      STRING,
      PAIR,
      SYMBOL,
      PRIMITIVE_PROCEDURE,
      PROCEDURE
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
      struct { // for both kinds of procedures
        Value* args; // list of arguments names for this procedure
        Environment* envt;  // closure-style environment for this procedure
        union {
          PrimitiveProcedure proc; // for primitive proc
          Value* body; // for procedures
        };
      };
    };

    explicit Value(long n) : type{Type::FIXNUM}, fixnum{n} {}
    explicit Value(bool b) : type{Type::BOOLEAN}, boolean{b} {}
    explicit Value(char c) : type{Type::CHARACTER}, character{c} {}
    explicit Value(Str s) : type{Type::STRING}, str(s) {}
    explicit Value(Value* a, Value* d) : type{Type::PAIR}, car{a}, cdr{d} {}
    explicit Value(Sym s) : type{Type::SYMBOL}, symbol(s) {}
    explicit Value(Value* a, Environment* e, PrimitiveProcedure p)
        : type{Type::PRIMITIVE_PROCEDURE}, args{a}, envt{e}, proc{p} {}
    explicit Value(Value* a, Environment* e, Value* b)
        : type{Type::PROCEDURE}, args{a}, envt{e}, body{b} {}
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
