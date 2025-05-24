# Regmatch

## What is it?

Regmatch is a single-header regular expression parser. It works by converting a regular expression into a minimal deterministic finite state automata (minimal DFA).

## How to use

Place the header file (found at `include/regmatch.h`) in the `include` (or equivalent) folder of your project and add `#include "regmatch.h"` to the top of the file you want to use it in.