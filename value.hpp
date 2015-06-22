#pragma once

#include <string>

namespace crisp{

class Value {
  public:
    enum class Type {
      FIXNUM,
      BOOLEAN,
      CHARACTER,
      STRING,
      PAIR,
      SYMBOL
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
      struct {
        Value* car;
        Value* cdr;
      };
      Sym symbol;
    };

    explicit Value(long n) : type{Type::FIXNUM}, fixnum{n} {}
    explicit Value(bool b) : type{Type::BOOLEAN}, boolean{b} {}
    explicit Value(char c) : type{Type::CHARACTER}, character{c} {}
    explicit Value(Str s) : type{Type::STRING}, str(s) {}
    explicit Value(Value* a, Value* d) : type{Type::PAIR}, car{a}, cdr{d} {}
    explicit Value(Sym s) : type{Type::SYMBOL}, symbol(s) {}
    Value() : type{Type::PAIR} {}
  private:
};

Value* getInternedSymbol(const std::string& name);
void print(Value* val);

extern Value True;
extern Value False;
extern Value EmptyPair;
extern Value* Quote;
extern Value* Define;
extern Value* Set;
extern Value* If;

}
