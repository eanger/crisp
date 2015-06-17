# Lexing
- This happens first, to turn a string into a list of tokens
    tokens = [(string, category),...]
- Turns into parser objects
    Operator: + - * /
    Lparen: (
    Rparen: )
    Number: [-] 0-9... [.] 0-9...

# Parser
- start with polish notation calculator
    Command: Operator Expression...
    Expression: Number or '(' Command ')'
    Number: [-]0-9...
    Operator: '+' '-' '*' '/'
- What's the data structure??
    class SExpression:
        def eval(self):
            return self.operand

    eg "* 13 2"
    (operator.mul, 13, 2)
    "* (+ 12 3) 4"
    (operator.mul, (operator.plus, 12, 3), 4)


class Atom:
    def eval(self):
        return self.value

class Expression:
    def eval(self):
        return self.operator(child.eval() for child in children)

# Evaluation

# Built-ins

# Library

# Types?
