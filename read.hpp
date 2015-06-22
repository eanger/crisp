#pragma once

#include "value.hpp"
#include "exception.hpp"

namespace crisp{

/***** API        *****/
Value* read(std::istream& input_stream);

/***** Exceptions *****/
class LexingError : public VerboseError {
  public:
    LexingError(const char* problem) : VerboseError("Lexing error: ", problem) {}
};

class ParsingError : public VerboseError {
  public:
    ParsingError(const char* problem) : VerboseError("Parsing error: ", problem) {}
};

}
