#include "catch.hpp"

#include "value.hpp"
#include "read.hpp"
#include "eval.hpp"

using namespace std;
using namespace crisp;

TEST_CASE("single quote expands to quote"){
  istringstream ss{"'input"};
  auto res = doRead(ss);
  REQUIRE(res != nullptr);
  REQUIRE(res->type == Value::Type::PAIR);
  REQUIRE(res->car != nullptr);
  REQUIRE(res->car->type == Value::Type::SYMBOL);
  REQUIRE(strcmp(res->car->symbol.name, "quote") == 0);
  REQUIRE(res->cdr->type == Value::Type::PAIR);
  REQUIRE(res->cdr->car->type == Value::Type::SYMBOL);
  REQUIRE(strcmp(res->cdr->car->symbol.name, "input") == 0);
}

TEST_CASE("backtick expands to quasiquote"){
  istringstream ss{"`input"};
  auto res = doRead(ss);
  REQUIRE(res != nullptr);
  REQUIRE(res->type == Value::Type::PAIR);
  REQUIRE(res->car != nullptr);
  REQUIRE(res->car->type == Value::Type::SYMBOL);
  REQUIRE(strcmp(res->car->symbol.name, "quasiquote") == 0);
  REQUIRE(res->cdr->type == Value::Type::PAIR);
  REQUIRE(res->cdr->car->type == Value::Type::SYMBOL);
  REQUIRE(strcmp(res->cdr->car->symbol.name, "input") == 0);
}

TEST_CASE("comma expands to unquote"){
  istringstream ss{",input"};
  auto res = doRead(ss);
  REQUIRE(res != nullptr);
  REQUIRE(res->type == Value::Type::PAIR);
  REQUIRE(res->car != nullptr);
  REQUIRE(res->car->type == Value::Type::SYMBOL);
  REQUIRE(strcmp(res->car->symbol.name, "unquote") == 0);
  REQUIRE(res->cdr->type == Value::Type::PAIR);
  REQUIRE(res->cdr->car->type == Value::Type::SYMBOL);
  REQUIRE(strcmp(res->cdr->car->symbol.name, "input") == 0);
}

