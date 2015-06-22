#include <cstring>
#include <iostream>
#include <string>
#include <vector>

#include "value.hpp"

using namespace std;

namespace crisp{
namespace { // unnamed namespace

vector<Value*> SymbolTable;

} // end unnamed namespace

Value::Sym::Sym(const char* n) {
  name = new char[strlen(n)]();
  strcpy(name, n);
}

Value* getInternedSymbol(const string& name) {
  auto symbol_itr = find_if(begin(SymbolTable), end(SymbolTable),
                            [&](Value* v){ 
                              return !strcmp(v->symbol.name, name.c_str());
                            });
  if(symbol_itr == end(SymbolTable)){
    // add new symbol
    Value::Sym sym(name.c_str());
    Value* symbol = new Value(sym);
    SymbolTable.push_back(symbol);
    symbol_itr = SymbolTable.end() - 1;
  }
  return *symbol_itr;
}

void print(Value* value) {
  if(!value){
    return;
  }
  switch(value->type){
    case Value::Type::FIXNUM:{
      cout << value->fixnum;
    } break;
    case Value::Type::BOOLEAN:{
      if(value->boolean){
        cout << "True";
      } else {
        cout << "False";
      }
    } break;
    case Value::Type::CHARACTER:{
      cout << "#\\" << value->character;
    } break;
    case Value::Type::STRING:{
      cout << value->str.str;
    } break;
    case Value::Type::PAIR:{
      cout << "(";
      if(value->car){
        print(value->car);
        cout << " . ";
        print(value->cdr);
      }
      cout << ")";
    } break;
    case Value::Type::SYMBOL:{
      cout << value->symbol.name;
    } break;
  }
}

Value True{true};
Value False{false};
Value EmptyPair{};
Value* Quote;
Value* Define;
Value* Set;
Value* If;
}

