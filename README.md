# benalohCrypto

This project depends on:
- [cryptol](https://github.com/GaloisInc/cryptol/tree/master) for (.cry) files
- [clang](https://github.com/llvm/llvm-project), [gmp](https://gmplib.org/) (for .cpp) files

## co685.cry
The goal of this program is to capture the mathematical behaviour of the benaloh encryption scheme. It allows us to utilize the higher level constructions in cryptol (a domain specific language) to stay true to the mathematics behind the scheme.

I have added helper functions to achieve a program that is easy to understand. It is useful to validate the scheme in this language since the mathematical constructs can be easy checked (as in the last line) by random testing, exhausing testing and/or formal verification using [Z3](https://github.com/jetafese/z3).

## co685.cpp
This program implements a prototype benaloh encryption scheme following the description in the following [workshop](https://iopscience.iop.org/article/10.1088/1742-6596/1542/1/012039/pdf). This is imperative code that is compiled and run using the clang compiler.

The assertions can be detected by our software verification [tool](https://github.com/jetafese/seahorn) for formal verification. Since the tool is not built with cryptography in mind, the computation times are quite slow - this is something that can be fixed in the future.

## co685.pdf

This document summarizes the project, motivation and contribution. It provides the documentation for the project alongside the compilation instruction in the Dockerfile: [`benaloh.Dockerfile`](benaloh.Dockerfile).

