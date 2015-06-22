#pragma once

#include <exception>

namespace crisp{
class VerboseError : public std::exception {
  public:
    virtual const char* what() const throw(){
      return message_;
    }
    VerboseError(const char* prefix, const char* problem) {
      message_ = new char[strlen(prefix) + strlen(problem)]();
      strcpy(message_, prefix);
      strcat(message_, problem);
    }
  private:
    char* message_;
};
}
