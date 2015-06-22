#include <iostream>

#include "read.hpp"
#include "value.hpp"
#include "eval.hpp"

using namespace std;
using namespace crisp;

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

int main(int, char*[]) {
  cout << "Welcome to Crisp. Use ctrl-c to exit.\n";
  Quote = getInternedSymbol("quote");
  Define = getInternedSymbol("define");
  Set = getInternedSymbol("set!");
  If = getInternedSymbol("if");

  while(true){
    cout << "crisp> ";
    try{
      print(eval(read(cin)));
    } catch(const exception& e){
      cout << e.what();
      cin.clear();
      cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
    cout << endl;
  }
  return 0;
}
