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

TEST_CASE("add is a varadic function doing addition"){
  initEval();
  stringstream ss{"add"};
  auto res = doEval(doRead(ss));
  REQUIRE(res != nullptr);
  REQUIRE(res->type == Value::Type::PROCEDURE);
  REQUIRE(res->is_primitive);
  ss.str(string());
  ss.clear();
  ss << "(add)";
  res = doEval(doRead(ss));
  REQUIRE(res != nullptr);
  REQUIRE(res->type == Value::Type::FIXNUM);
  REQUIRE(res->fixnum == 0);
  ss.str(string());
  ss.clear();
  ss << "(add 1)";
  res = doEval(doRead(ss));
  REQUIRE(res != nullptr);
  REQUIRE(res->type == Value::Type::FIXNUM);
  REQUIRE(res->fixnum == 1);
  ss.str(string());
  ss.clear();
  ss << "(add 1 2 3 4 5)";
  res = doEval(doRead(ss));
  REQUIRE(res != nullptr);
  REQUIRE(res->type == Value::Type::FIXNUM);
  REQUIRE(res->fixnum == 15);
}

TEST_CASE("add2ormore adds two or more values"){
  initEval();
  stringstream ss{"add2ormore"};
  auto res = doEval(doRead(ss));
  REQUIRE(res != nullptr);
  REQUIRE(res->type == Value::Type::PROCEDURE);
  ss.str(string());
  ss.clear();
  ss << "(add2ormore 1)";
  REQUIRE_THROWS_AS(doEval(doRead(ss)), EvaluationError);
  ss.str(string());
  ss.clear();
  ss << "(add2ormore 1 2)";
  res = doEval(doRead(ss));
  REQUIRE(res != nullptr);
  REQUIRE(res->type == Value::Type::FIXNUM);
  REQUIRE(res->fixnum == 3);
  ss.str(string());
  ss.clear();
  ss << "(add2ormore 1 2 3 4 5)";
  res = doEval(doRead(ss));
  REQUIRE(res != nullptr);
  REQUIRE(res->type == Value::Type::FIXNUM);
  REQUIRE(res->fixnum == 15);
}

TEST_CASE("quasiquote is like quote but only evaluates at runtime"){
  initEval();
  stringstream ss{"quasiquote"};
  auto res = doEval(doRead(ss));
  REQUIRE(res != nullptr);
  REQUIRE(res->type == Value::Type::SPECIAL_FORM);

  ss.str(string());
  ss.clear();
  ss << "(quasiquote (1 2 3))";
  res = doEval(doRead(ss));
  REQUIRE(res != nullptr);
  REQUIRE(res->type == Value::Type::PAIR);
  REQUIRE(res->car->type == Value::Type::FIXNUM);
  REQUIRE(res->car->fixnum == 1);
  REQUIRE(res->cdr->type == Value::Type::PAIR);
  REQUIRE(res->cdr->car->type == Value::Type::FIXNUM);
  REQUIRE(res->cdr->car->fixnum == 2);
  REQUIRE(res->cdr->cdr->type == Value::Type::PAIR);
  REQUIRE(res->cdr->cdr->car->type == Value::Type::FIXNUM);
  REQUIRE(res->cdr->cdr->car->fixnum == 3);
  REQUIRE(res->cdr->cdr->cdr == EmptyList);

  ss.str(string());
  ss.clear();
  ss << "(quasiquote (9 (unquote (add 1 2))))";
  res = doEval(doRead(ss));
  REQUIRE(res != nullptr);
  REQUIRE(res->type == Value::Type::PAIR);
  REQUIRE(res->car->type == Value::Type::FIXNUM);
  REQUIRE(res->car->fixnum == 9);
  REQUIRE(res->cdr->car->type == Value::Type::FIXNUM);
  REQUIRE(res->cdr->car->fixnum == 3);
}

TEST_CASE("symbol bindings added to environment"){
  Environment envt{nullptr};
  Value dummy;
  envt.setSymbolBinding("test", &dummy);
  REQUIRE(envt.getSymbolBinding("test") == &dummy);
}

