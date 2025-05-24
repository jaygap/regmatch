# Regmatch

## What is it?

Regmatch is a single-header file regular expression parser. It works by converting a regular expression into a minimal deterministic finite state automaton (minimal DFA). It can only parse *regular* languages as it does not support back referencing, so for example it cannot parse HTML.

## How to use it

Place the header file (found at `include/regmatch.h`) in the `include` (or equivalent) folder of your project and add `#include "regmatch.h"` to the top of the file you want to use it in.

Instantiate a variable of type `Matcher` and pass in a regular expression to the constructor.

### Syntax

- `*` zero or more of the preceding character or group (*not completed*)
- `+` one or more of the preceding character or group (*not completed*)
- `?` zero or one of the preceding character or group (*not completed*)
- `|` the left or right sequence of characters or group (*not completed*)
- `^` the start of a line (or not the following when at the start of a `[]` group) (*not completed*)
- `$` the end of a line (*not completed*)
- `\` the proceding character literally (*not completed*)
- `()` groups a sequence of characters
- `[]` defines a list of potential characters (`[abc]` is equivalent to `a|b|c` but provides additional functionality with ranges and `^` operator) (*not completed*)
- `{x}` *x* occurences of the prceding character or group (*not completed*)

*Support for other syntax elements will be included such as for white space characters or non-whitespace characters but will only be listed after these basic syntax elements are completed*

## How does it work

The `Matcher` class takes in a regular expression, converts it into a finite-state automaton (FSA) which may contain epsilon transitions. Then the epsilon transitions are removed, so that the FSA can be converted into a deterministic finite-state automaton (DFA) and then into a minimal DFA to allow for easy comparison of regular expressions.