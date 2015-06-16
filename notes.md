# Parser
- start with polish notation calculator
    Command: Operator Expression...
    Expression: Number or '(' Operator Expression... ')'
    Number: [-]0-9...
    Operator: '+' '-' '*' '/'


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
