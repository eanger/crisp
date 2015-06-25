#include <iostream>

#include "value.hpp"
#include "read.hpp"
#include "eval.hpp"

using namespace std;
using namespace crisp;

int main(int, char*[]) {
  cout << "Welcome to Crisp. Use ctrl-c to exit.\n";

  initEval();

  while(true){
    cout << "crisp> ";
    try{
      print(doEval(doRead(cin)));
    } catch(const exception& e){
      cout << e.what();
      cin.clear();
      cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
    cout << endl;
  }
  return 0;
}
