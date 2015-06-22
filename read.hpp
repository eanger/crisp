#pragma once
#include <exception>

#include "value.hpp"

namespace crisp{

/***** API        *****/
Value* read(std::istream& input_stream);

/***** Exceptions *****/
class LexingError : public std::exception {
  public:
    virtual const char* what() const throw(){
      return message_;
    }

    LexingError(const char* problem);
  private:
    char* message_;
};

class ParsingError : public std::exception {
  public:
    virtual const char* what() const throw(){
      return message_;
    }

    ParsingError(const char* problem);
  private:
    char* message_;
};

}
