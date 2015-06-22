# General
- we're leaking memory now
- let's do a cleanup
    - separate reading/evaluating/printing?
    - environment is in eval
    - main is just the repl

# Read
- add special characters #\newline and #\space
- add escaped characters to strings
- do pretty printing of lists
- maybe switch from distinct lexing tokens and value indicators
- should we validate special forms to make sure they have the correct syntax before eval?

# Evaluate
- we shouldn't be able to define/set/operate on the result of a define or set, since they have a meaningless return value
- the special forms are evaluated weird; we don't map the symbol name to a special Value in the global environment (which we should, even if it's a nullptr, or itself) and try to evaluate it

# Built-ins

# Library
