#include "catch.hpp"

#include "value.hpp"
#include "eval.hpp"
#include "read.hpp"

using namespace crisp;
using namespace std;

TEST_CASE("cons builds a pair"){
  istringstream ss{"(cons 1 2)"};
  initEval();
  auto res = doEval(doRead(ss));
  REQUIRE(res != nullptr);
  REQUIRE(res->type == Value::Type::PAIR);
  REQUIRE(res->car->type == Value::Type::FIXNUM);
  REQUIRE(res->car->fixnum == 1);
  REQUIRE(res->cdr->type == Value::Type::FIXNUM);
  REQUIRE(res->cdr->fixnum == 2);
}

