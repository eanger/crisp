# General
- we're leaking memory now

# Read
- add special characters #\newline and #\space
- add escaped characters to strings
- maybe switch from distinct lexing tokens and value indicators
- should we validate special forms to make sure they have the correct syntax before eval?

# Evaluate
- we shouldn't be able to define/set/operate on the result of a define or set, since they have a meaningless return value

# Lambda
    (lambda (x y ...) body...)
- lambda creates a closure that binds an environment (the current environment) to a piece of execution
