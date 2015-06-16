#!/usr/bin/env python3
from collections import namedtuple
import operator

operators = {'+' : operator.add,
             '-' : operator.sub,
             '*' : operator.mul,
             '/' : operator.floordiv}
def parse(line):
    # precondition: line has no beginning or trailing white space
    print("You entered: {}".format(line))
    command = Expression(operators[line[0]],

if __name__ == "__main__":
    print("Welcome to crisp")
    try:
        while True:
            line = input("crisp> ")
            parse(line.strip())
    except KeyboardInterrupt:
        print("\nShutting down.")
