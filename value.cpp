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

Value::Str::Str(const char* s) {
  str = new char[strlen(s)]();
  strcpy(str, s);
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
  if(value == EmptyList){
    cout << "()";
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
      cout << "\"" << value->str.str << "\"";
    } break;
    case Value::Type::PAIR:{
      cout << "(";
      print(value->car);
      for(Value* cdr = value->cdr; cdr; cdr = cdr->cdr){
        if(cdr->type != Value::Type::PAIR){
          cout << " . ";
          print(cdr);
          break;
        }
        cout << " ";
        print(cdr->car);
      }
      cout << ")";
    } break;
    case Value::Type::SYMBOL:{
      cout << value->symbol.name;
    } break;
    case Value::Type::PROCEDURE:
    case Value::Type::PRIMITIVE_PROCEDURE:{
      cout << "#<procedure>";
    } break;
  }
}

Value* reverse(Value* list) {
  Value* result_list = EmptyList;
  for(Value* ptr = list; ptr != nullptr; ptr = ptr->cdr){
    result_list = new Value(ptr->car, result_list);
  }
  return result_list;
}

Value True{true};
Value False{false};
Value* EmptyList{nullptr};
}

