#include <iostream>
#include <stack>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <algorithm> 
#include <vector>
#include <queue>

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
};

class Matcher {
private:
  std::vector<State> fsa;
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

  std::vector<State> genFSA() {
    std::vector<State> s;
    int regex_pointer = 0;
    int state_pointer = 0;
    std::stack<int> state_pointers;
    char current;
    int depth = 0;
    std::stack<std::pair<int, int>>
        depth_of_pipes; // keeps track of the (depth, state index of final state
                        // in first branch) of different pipe operators

    State state = State();
    state.label = "q0";
    state.is_final = false;
    s.push_back(state); // initial state
    state_pointers.push(0);

    do {
      current = regex.at(regex_pointer);

      if (current == '(') {
        state_pointers.push(state_pointer);
        depth++;
      } else if (current == ')') {
        if (!depth_of_pipes.empty() && depth_of_pipes.top().first == depth) {
          auto pipe_info = depth_of_pipes.top();
          depth_of_pipes.pop();

          state = State();
          state.is_final = false;
          state.label = "q" + std::to_string(s.size());
          s.push_back(state);

          s[s.size() - 2].transitions.insert({'\0', s.size() - 1});
          s[pipe_info.second].transitions.insert({'\0', s.size() - 1});

          state_pointer = s.size() - 1;
        }

        depth--;
      } else if (current == '*' || current == '+' || current == '?') {
        if (regex_pointer == 0) {
          std::cout << "ERROR: Invalid regex. * or + or ? come at the start of "
                       "the regex which is not allowed."
                    << std::endl;
          return std::vector<State>{};
        }

        char prev = regex.at(regex_pointer - 1);
        int other_state;

        if (prev == ')') {
          other_state = state_pointers.top();
          state_pointers.pop();
        } else {
          other_state = state_pointer - 1;
        }

        if (current == '*') {
          s[state_pointer].transitions.insert({'\0', other_state});
          s[other_state].transitions.insert({'\0', state_pointer});
        } else if (current == '+') {
          s[state_pointer].transitions.insert({'\0', other_state});
        } else {
          s[other_state].transitions.insert({'\0', state_pointer});
        }
      } else if (current == '|') {
        if (!depth_of_pipes.empty() && depth_of_pipes.top().first == depth) {
          auto pipe_info = depth_of_pipes.top();

          state = State();
          state.is_final = false;
          state.label = "q" + std::to_string(s.size());
          s.push_back(state);

          s[s.size() - 2].transitions.insert({'\0', s.size() - 1});
          s[pipe_info.second].transitions.insert({'\0', s.size() - 1});

          depth_of_pipes.pop();
          depth_of_pipes.push({depth, s.size() - 1});

          state = State();
          state.is_final = false;
          state.label = "q" + std::to_string(s.size());
          s.push_back(state);

          s[pipe_info.second + 1].transitions.insert({'\0', s.size() - 1});
        } else {
          depth_of_pipes.push({depth, state_pointer});

          state = State();
          state.label = "q" + std::to_string(s.size());
          state.is_final = false;
          s.push_back(state);
          s[state_pointers.top()].transitions.insert({'\0', s.size() - 1});
        }

        state_pointer = s.size() - 1;
      } else if (current == '^' || current == '$') {
        std::cout << "start/end of line";
      } else if (current == '\\') {
        if (regex_pointer + 1 >= regex.size()) {
          std::cout << "ERROR: Invalid regex. \\ is at the end of a string "
                       "which is not allowed. It should always be followed by "
                       "another character";
        }

        state = State();
        state.label = "q" + std::to_string(s.size());
        state.is_final = false;
        s.push_back(state);
        s[state_pointer].transitions.insert({'\0', s.size() - 1});
        state_pointer = s.size() - 1;

        regex_pointer++;
        state = State();
        state.label = "q" + std::to_string(s.size());
        state.is_final = false;
        s.push_back(state);
        s[state_pointer].transitions.insert(
            {regex.at(regex_pointer), s.size() - 1});
        state_pointer = s.size() - 1;
      } else {
        state = State();
        state.label = "q" + std::to_string(s.size());
        state.is_final = false;
        s.push_back(state);
        s[state_pointer].transitions.insert({'\0', s.size() - 1});
        state_pointer = s.size() - 1;

        state = State();
        state.label = "q" + std::to_string(s.size());
        state.is_final = false;
        s.push_back(state);
        s[state_pointer].transitions.insert({current, s.size() - 1});
        state_pointer = s.size() - 1;
      }
      regex_pointer++;
    } while (regex_pointer < regex.length());

    if (!depth_of_pipes.empty() && depth_of_pipes.top().first == depth) {
      auto pipe_info = depth_of_pipes.top();
      depth_of_pipes.pop();

      state = State();
      state.is_final = false;
      state.label = "q" + std::to_string(s.size());
      s.push_back(state);

      s[s.size() - 2].transitions.insert({'\0', s.size() - 1});
      s[pipe_info.second].transitions.insert({'\0', s.size() - 1});

      state_pointer = s.size() - 1;
    }

    s[s.size() - 1].is_final = true;

    fsa = s;
    printFSA();

    return s;
  }

  bool hasEpsilonTransitions(State s) {
    if (s.transitions.count('\0') > 0) {
      return true;
    }

    return false;
  }

  void convertToMinDFA(std::vector<State>& initial_fsa) {
    // epsilon transition removal
    std::vector<State> fsa_no_epsilon;

    //lambda function to calculate epsilon closure
    auto epsilonClosure = [&fsa_no_epsilon, initial_fsa](int start_index) -> std::unordered_set<int> {
      std::unordered_set<int> closure;
      std::stack<int> stack;
      stack.push(start_index);

      closure.insert(start_index);

      while(!stack.empty()){
        int current = stack.top();
        stack.pop();

        for(auto t : initial_fsa[current].transitions){
          if(t.first == '\0'){
            if(closure.find(t.second) == closure.end()){
              closure.insert(t.second);
              stack.push(t.second);
            }
          }
        }
      }

      return closure;
    };

    //creates a string that is a unique key for epsilon-closure
    auto getClosureKey = [](const std::unordered_set<int>& closure) -> std::string {
      std::vector<int> sorted(closure.begin(), closure.end());
      std::sort(sorted.begin(), sorted.end());
      std::string key;

      for (int index : sorted) {
        key += std::to_string(index) + ",";
      }
      
      return key;
    };

    //creates a state with a given label and epsilon-closure
    auto createState = [initial_fsa](const std::string& label, const std::unordered_set<int>& closure) -> State{
      State state;
      state.label = label;
      state.is_final = false;

      for(int index : closure) {
        if (initial_fsa[index].is_final) {
          state.is_final = true;
        }
        
        for (auto transition : initial_fsa[index].transitions) {
          if (transition.first != '\0') {
            state.transitions.insert(transition);
          }
        }
      }

      return state;
    };

    std::unordered_map<std::string, int> closure_to_index;
    std::unordered_set<int> first_closure = epsilonClosure(0);
    std::string start_key = getClosureKey(first_closure);

    closure_to_index.insert({start_key, 0});

    State start_state = createState("q0", first_closure);

    fsa_no_epsilon.push_back(start_state);

    std::queue<std::unordered_set<int>> unmarked;
    unmarked.push(first_closure);

    while(!unmarked.empty()){
      std::unordered_set<int> current_closure = unmarked.front();
      unmarked.pop();

      std::string current_key = getClosureKey(current_closure);
      int current_index = closure_to_index[current_key];

      std::unordered_map<char, std::unordered_set<int>> symbol_to_closure;

      for(int state_index : current_closure){
        for(auto t : initial_fsa[state_index].transitions){
          if(t.first == '\0'){
            continue;
          }

          char symbol = t.first;
          int new_origin_state_index = t.second;

          std::unordered_set<int> closure = epsilonClosure(new_origin_state_index);
          symbol_to_closure.insert({symbol, closure});
        }
      }

      for(auto& entry : symbol_to_closure){
        char symbol = entry.first;
        auto closure = entry.second;
        
        std::string closure_key = getClosureKey(closure);
        int target_index;

        if(closure_to_index.find(closure_key) == closure_to_index.end()){
          target_index = fsa_no_epsilon.size();
          closure_to_index.insert({closure_key, target_index});

          State new_state = createState("q" + std::to_string(target_index), closure);
          
          fsa_no_epsilon.push_back(new_state);
          unmarked.push(closure);
        } else{
          target_index = closure_to_index[closure_key];
        }

        //replaces existing transitions end state to the actual correct one (removes it first if it exists)
        auto itr = fsa_no_epsilon[current_index].transitions.find(symbol);

        if(itr != fsa_no_epsilon[current_index].transitions.end() && itr->second > fsa_no_epsilon.size()){
          fsa_no_epsilon[current_index].transitions.erase(itr);
        }

        fsa_no_epsilon[current_index].transitions.insert({symbol, target_index});
      }
    }

    // subset construction
    std::vector<State> dfa;

    // main min routine so that comparing regex is easy
    std::vector<State> min_dfa;

    initial_fsa = fsa_no_epsilon;
  }

  int indexOf(std::vector<int> needle, std::vector<std::vector<int>> haystack) {
    if (needle.empty() || haystack.empty()) {
      return -1;
    }

    for (int i = 0; i < haystack.size(); i++) {
      if (equalVectors(needle, haystack[i])) {
        return i;
      }
    }

    return -1;
  }

  // compares the values of each vector, regardless of position
  bool equalVectors(std::vector<int> a, std::vector<int> b) {
    if (a.empty() && b.empty()) {
      return true;
    }

    if (a.empty()) {
      return false;
    }

    if (b.empty()) {
      return false;
    }

    if (a.size() != b.size()) {
      return false;
    }

    std::unordered_multiset<int> set_a(a.begin(), a.end());
    std::unordered_multiset<int> set_b(b.begin(), b.end());

    for (auto itr_a = a.begin(); itr_a != a.end(); itr_a++) {
      if (set_b.count(*itr_a) != set_a.count(*itr_a)) {
        return false;
      }
    }

    return true;
  }

  bool vectorContains(int needle, std::vector<int> haystack) {
    if (haystack.empty()) {
      return false;
    }

    for (int i = 0; i < haystack.size(); i++) {
      if (haystack[i] == needle) {
        return true;
      }
    }

    return false;
  }

  bool vectorInVector(std::vector<int> needle, std::vector<std::vector<int>> haystack) {
    if (needle.empty() || haystack.empty()) {
      return false;
    }

    for (auto itr = haystack.begin(); itr != haystack.end(); itr++) {
      if (equalVectors(needle, *itr)) {
        return true;
      }
    }

    return false;
  }

public:
  Matcher(std::string s) {
    regex = s;
    fsa = genFSA();
    convertToMinDFA(fsa);
  }

  std::string getRegex() { return regex; }

  void printFSA() {
    std::cout << "States: {";

    for (int i = 0; i < fsa.size(); i++) {
      if (fsa[i].is_final) {
        std::cout << "\e[4m"; // underlines final states
      }

      std::cout << fsa[i].label;

      if (fsa[i].is_final) {
        std::cout << "\e[0m"; // ends underline of final states
      }

      if (i + 1 < fsa.size()) {
        std::cout << ", ";
      }
    }
    std::cout << "}\nTransitions:\n{\n";

    for (const State &s : fsa) {
      for (auto transition : s.transitions) {
        std::cout << "(" << s.label << ", " << transition.first << ", "
                  << fsa[transition.second].label << ")\n";
      }
    }

    std::cout << '}' << std::endl;
  }

  bool matchString(std::string s) {
    State active_state = fsa.front(); // first state is always the initial state

    for (char &c : s) {
      if (active_state.transitions.find(c) != active_state.transitions.end()) {
        active_state = fsa[active_state.transitions.find(c)->second];
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

/* #TODO generated by copilot, review before implementing

// Replace your convertToMinDFA implementation with the following code:

void convertToMinDFA(std::vector<State>& initial_fsa) {
    // This new implementation computes the epsilon-closure for each state-set,
    // then uses subset construction to merge transitions correctly.

    std::vector<State> fsa_no_epsilon;
    std::unordered_map<std::string, int> closure_to_index;

    // Helper lambda: compute epsilon-closure of a given state
    auto epsilonClosure = [&](int state_index) -> std::unordered_set<int> {
        std::unordered_set<int> closure;
        std::stack<int> stack;
        stack.push(state_index);
        closure.insert(state_index);
        while (!stack.empty()) {
            int cur = stack.top();
            stack.pop();
            for (auto t : initial_fsa[cur].transitions) {
                if (t.first == '\0') {
                    if (closure.find(t.second) == closure.end()) {
                        closure.insert(t.second);
                        stack.push(t.second);
                    }
                }
            }
        }
        return closure;
    };

    // Helper lambda: generate a unique key for a set of states
    auto getClosureKey = [&](const std::unordered_set<int>& closure) -> std::string {
        std::vector<int> sorted(closure.begin(), closure.end());
        std::sort(sorted.begin(), sorted.end());
        std::string key;
        for (int idx : sorted) {
            key += std::to_string(idx) + ",";
        }
        return key;
    };

    // Compute the closure for the initial state (state 0)
    std::unordered_set<int> first_closure = epsilonClosure(0);
    std::string start_key = getClosureKey(first_closure);
    closure_to_index[start_key] = 0;

    // Create the start state with merged non-epsilon transitions
    State startState;
    startState.label = "q0";
    startState.is_final = false;
    for (int idx : first_closure) {
        if (initial_fsa[idx].is_final)
            startState.is_final = true;
        for (auto t : initial_fsa[idx].transitions) {
            if (t.first != '\0')
                startState.transitions.insert(t);
        }
    }
    fsa_no_epsilon.push_back(startState);

    // Process each closure using a queue (subset construction)
    std::queue<std::unordered_set<int>> unmarked;
    unmarked.push(first_closure);

    while (!unmarked.empty()) {
        auto curr_closure = unmarked.front();
        unmarked.pop();
        std::string curr_key = getClosureKey(curr_closure);
        int curr_index = closure_to_index[curr_key];

        // Map input symbols to union of epsilon closures
        std::unordered_map<char, std::unordered_set<int>> symbol_to_closure;
        for (int state_idx : curr_closure) {
            for (auto t : initial_fsa[state_idx].transitions) {
                if (t.first == '\0')
                    continue;
                char symbol = t.first;
                int target = t.second;
                // Get epsilon closure of the target state.
                auto target_closure = epsilonClosure(target);
                symbol_to_closure[symbol].insert(target_closure.begin(), target_closure.end());
            }
        }

        // For each symbol create or identify the corresponding state
        for (auto& entry : symbol_to_closure) {
            char symbol = entry.first;
            auto target_closure = entry.second;
            std::string target_key = getClosureKey(target_closure);
            int target_index;
            if (closure_to_index.find(target_key) == closure_to_index.end()) {
                target_index = fsa_no_epsilon.size();
                closure_to_index[target_key] = target_index;

                // Merge all non-epsilon transitions and determine if it's final.
                State newState;
                newState.label = "q" + std::to_string(target_index);
                newState.is_final = false;
                for (int idx : target_closure) {
                    if (initial_fsa[idx].is_final)
                        newState.is_final = true;
                    for (auto t : initial_fsa[idx].transitions) {
                        if (t.first != '\0')
                            newState.transitions.insert(t);
                    }
                }

                fsa_no_epsilon.push_back(newState);
                unmarked.push(target_closure);
            } else {
                target_index = closure_to_index[target_key];
            }
            // Add the transition from current state via the symbol.
            fsa_no_epsilon[curr_index].transitions.insert({symbol, target_index});
        }
    }

    // Now initial_fsa becomes the epsilon-free FSA
    initial_fsa = fsa_no_epsilon;
}

*/
