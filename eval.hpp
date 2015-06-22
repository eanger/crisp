#pragma once

#include "value.hpp"
#include "exception.hpp"

namespace crisp{
/***** API        *****/
void initEval();
Value* eval(Value* input);

/***** Exceptions *****/
class EvaluationError : public VerboseError {
  public:
    EvaluationError(const char* problem) : VerboseError("Eval error: ", problem) {}
};
}
