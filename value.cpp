#include <cstring>
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

Value True{true};
Value False{false};
Value EmptyPair{};
Value* Quote;
Value* Define;
Value* Set;
Value* If;
}

