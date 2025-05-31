#include <iostream>
#include <stack>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

// TODO:
/*
High priority:
    *
    |
    ()

Low priority:
    +
    \
    ?
    []
    {}
    ^
    $


FSA W/ EPSILON TRANSITIONS -> FSA W/O EPSILON TRANSITIONS
FSA W/O EPSILON TRANSITIONS -> DFA
DFA -> MIN DFA
*/

struct State {
  std::unordered_multimap<char, int>
      transitions; // int is index of the state in the FSA
  bool is_final;
  std::string label;
};

struct FSA {
  std::vector<State> states;

  void printFSA() {
    std::cout << "States: {";

    for (int i = 0; i < states.size(); i++) {
      std::cout << states[i].label;

      if (i + 1 < states.size()) {
        std::cout << ", ";
      }
    }
    std::cout << "}\nTransitions:\n{\n";

    for (const State &s : states) {
      for (auto transition : s.transitions) {
        std::cout << "(" << s.label << ", " << transition.first << ", "
                  << states[transition.second].label << ")\n";
      }
    }

    std::cout << '}' << std::endl;
  }
};

class Matcher {
private:
  FSA automata;
  std::string regex;

  bool checkBrackets(char left, char right) {
    if (left == '(' && right == ')') {
      return true;
    }

    if (left == '[' && right == ']') {
      return true;
    }

    if (left == '{' && right == '}') {
      return true;
    }

    return false;
  }

  FSA genFSA() {
    FSA s;
    int regex_pointer = 0;
    int state_pointer = 0;
    std::stack<int> state_pointers;
    char current;
    State state = State();

    state.label = "q0";
    state.is_final = false;
    s.states.push_back(state); // initial state

    do {
      current = regex.at(regex_pointer);

      if (current == '(') {
        state_pointers.push(state_pointer);
      } else if (current == ')') {
        // do nothing
        std::cout << "closing bracket";
      } else if (current == '*' || current == '+' || current == '?') {
        if (regex_pointer == 0) {
          std::cout << "ERROR: Invalid regex. * or + or ? come at the start of "
                       "the regex which is not allowed."
                    << std::endl;
          return FSA();
        }

        char prev = regex.at(regex_pointer - 1);
        int other_state =
            (prev == ')' ? state_pointers.top() : state_pointer - 1);

        if (prev == ')') {
          state_pointers.pop();
        }

        if (current == '*') {
          s.states[state_pointer].transitions.insert({'\0', other_state});
          s.states[other_state].transitions.insert({'\0', state_pointer});
        } else if (current == '+') {
          s.states[state_pointer].transitions.insert({'\0', other_state});
        } else {
          s.states[other_state].transitions.insert({'\0', state_pointer});
        }
      } else if (current == '|') {
        std::cout << "or";
      } else if (current == '^' || current == '$') {
        std::cout << "start/end of line";
      } else if (current == '\\') {

      } else {
        state = State();
        state.label = "q" + std::to_string(s.states.size());
        state.is_final = false;
        s.states.push_back(state);
        s.states[state_pointer].transitions.insert(
            {current, s.states.size() - 1});
        state_pointer = s.states.size() - 1;
      }
      regex_pointer++;
    } while (regex_pointer < regex.length());

    s.states[s.states.size() - 1].is_final = true;

    return convertToMinDFA(s);
  }

  bool hasEpsilonTransitions(State s) {
    if (s.transitions.count('\0') > 0) {
      return true;
    }

    return false;
  }

  FSA convertToMinDFA(FSA fsa) {
    FSA fsa_no_epsilon = FSA();
    State state_to_add = State();

    // remove \0 (epsilon transitions)
    for (int i = 0; i < fsa.states.size(); i++) {
      int current_state = i;
      state_to_add = State();
      state_to_add.is_final = false;
      state_to_add.label = "q" + std::to_string(i);

      std::unordered_set<int> visited_states;
      std::stack<int> to_visit;

      to_visit.push(i);

      do {
        if (visited_states.count(current_state) == 0) {
          for (auto t : fsa.states[current_state].transitions) {
            if (t.first == '\0') {
              to_visit.push(t.second);
            } else {
              state_to_add.transitions.insert(t);
            }
          }

          if (fsa.states[current_state].is_final == true) {
            state_to_add.is_final = true;
          }
        }

        visited_states.insert(current_state);
        current_state = to_visit.top();
        to_visit.pop();
      } while (!to_visit.empty());

      fsa_no_epsilon.states.push_back(state_to_add);
    }

    // remove duplicate outgoing transitions
    FSA dfa = FSA();

    dfa.printFSA();

    // main min routine so that comparing regex is easy
    // FSA min_dfa = FSA();

    return fsa_no_epsilon;
  }

  bool vectorContains(int needle, std::vector<int> haystack) {
    for (int i = 0; i < haystack.size(); i++) {
      if (haystack[i] == needle) {
        return true;
      }
    }

    return false;
  }

  bool vectorInVector(std::vector<int> needle,
                      std::vector<std::vector<int>> haystack) {
    for (int i = 0; i < haystack.size(); i++) {
      if (haystack[i].size() != needle.size()) {
        continue;
      }

      for (int j = 0; j < needle.size(); j++) {
        if (haystack[i][j] != needle[j]) {
          break;
        }

        if (j + 1 == needle.size()) {
          return true;
        }
      }
    }

    return false;
  }

public:
  Matcher(std::string s) {
    regex = s;
    automata = genFSA();
  }

  std::string getRegex() { return regex; }

  FSA getFSA() { return automata; }

  void printFSA() { automata.printFSA(); }

  bool matchString(std::string s) {
    State active_state =
        automata.states.front(); // first state is always the initial state

    for (char &c : s) {
      if (active_state.transitions.find(c) != active_state.transitions.end()) {
        active_state =
            automata.states[active_state.transitions.find(c)->second];
      } else {
        return false;
      }
    }

    return active_state.is_final;
  }

  // bool compareRegex(Matcher m) {
  //   // check if the regex of self and m are equivalent

  //   return false;
  // }

  // bool compareRegex(std::string s) { return compareRegex(Matcher(s)); }
};
