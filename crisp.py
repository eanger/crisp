#!/usr/bin/env python3

from enum import Enum

Lexeme = Enum('Lexeme', 'Operator LParen RParen Number')

def addFunction(*args):
    result = 0
    for arg in args:
        result = result + arg
    return result

def lex(line):
    if line == "":
        return []

    def readNumber(l):
        """ Return end idx of number read from l """
        end_idx = 0
        for char in l:
            if char in "0123456789":
                end_idx = end_idx + 1
        return end_idx
    
    # do some lexing, return new list combined with further lexing
    char = line[0]
    lexing_idx = 1
    if char == " ":
        # white space, continue
        return lex(line[lexing_idx:])

    if char == "+":
        res = (Lexeme.Operator, addFunction)
    if char == "-":
        res = (Lexeme.Operator, operator.sub)
    if char == "*":
        res = (Lexeme.Operator, operator.mul)
    if char == "/":
        res = (Lexeme.Operator, operator.truediv)
    if char in "0123456789":
        num_idx = readNumber(line)
        res = (Lexeme.Number, int(line[:num_idx]))
        lexing_idx = num_idx

    return [res] + lex(line[lexing_idx:])

def parseCommand(tokens):
    # recursive parsing
    pass

def evaluate(ast):
    pass

def eval(line):
    # precondition: line has no beginning or trailing white space
    print("You entered: {}".format(line))
    tokens = lex(line)
    print(tokens)
    ast = parse(tokens)
    result = evaluate(ast)
    print(result)

if __name__ == "__main__":
    print("Welcome to crisp")
    try:
        while True:
            line = input("crisp> ")
            eval(line.strip())
    except KeyboardInterrupt:
        print("\nShutting down.")
