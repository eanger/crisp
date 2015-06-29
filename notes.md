# Current To-do
- implement cons to facilitate building lists: ie refactoring
- implementing quasiquote and unquote


# General
- we're leaking memory now

# Read
- add special characters #\newline and #\space
- add escaped characters to strings
- maybe switch from distinct lexing tokens and value indicators

# Evaluate
- we shouldn't be able to define/set/operate on the result of a define or set, since they have a meaningless return value
## Refactor eval into a primitive procedure
- Eval has two params, the input value (to be evaluated) and the execution environment
    (define (eval input envt) ...)
- This means we need to convert Environment into a scheme data structure, which we can operate on
