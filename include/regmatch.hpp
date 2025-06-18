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

struct State
{
  std::unordered_multimap<char, int>
      transitions; // int is index of the state in the FSA
  bool is_final;
  std::string label;
};

struct FSA
{
  std::vector<State> states;

  void printFSA()
  {
    std::cout << "States: {";

    for (int i = 0; i < states.size(); i++)
    {
      if(states[i].is_final){
        std::cout << "\e[4m"; //underlines final states
      }

      std::cout << states[i].label;

      if(states[i].is_final){
        std::cout << "\e[0m"; //ends unerline of final states
      }

      if (i + 1 < states.size())
      {
        std::cout << ", ";
      }
    }
    std::cout << "}\nTransitions:\n{\n";

    for (const State &s : states)
    {
      for (auto transition : s.transitions)
      {
        std::cout << "(" << s.label << ", " << transition.first << ", " << states[transition.second].label << ")\n";
      }
    }

    std::cout << '}' << std::endl;
  }
};

class Matcher
{
private:
  FSA automata;
  std::string regex;

  bool checkBrackets(char left, char right)
  {
    if (left == '(' && right == ')')
    {
      return true;
    }

    if (left == '[' && right == ']')
    {
      return true;
    }

    if (left == '{' && right == '}')
    {
      return true;
    }

    return false;
  }

  FSA genFSA()
  {
    FSA s = FSA();
    int regex_pointer = 0;
    int state_pointer = 0;
    std::stack<int> state_pointers;
    char current;
    int depth = 0;
    std::stack<std::pair<int, int>> depth_of_pipes; //keeps track of the (depth, state index of final state in first branch) of different pipe operators

    State state = State();
    state.label = "q0";
    state.is_final = false;
    s.states.push_back(state); // initial state
    state_pointers.push(0);

    do
    {
      current = regex.at(regex_pointer);

      if (current == '(')
      {
        state_pointers.push(state_pointer);
        depth++;
      }
      else if (current == ')')
      {
        if(!depth_of_pipes.empty() && depth_of_pipes.top().first == depth){
          auto pipe_info = depth_of_pipes.top();
          depth_of_pipes.pop();

          state = State();
          state.is_final = false;
          state.label = "q" + std::to_string(s.states.size());
          s.states.push_back(state);

          s.states[s.states.size() - 2].transitions.insert({'\0', s.states.size() - 1});
          s.states[pipe_info.second].transitions.insert({'\0', s.states.size() - 1});

          state_pointer = s.states.size() - 1;
        }

        depth--;
      }
      else if (current == '*' || current == '+' || current == '?')
      {
        if (regex_pointer == 0)
        {
          std::cout << "ERROR: Invalid regex. * or + or ? come at the start of the regex which is not allowed." << std::endl;
          return FSA();
        }

        char prev = regex.at(regex_pointer - 1);
        int other_state;

        if (prev == ')')
        {
          other_state = state_pointers.top();
          state_pointers.pop();
        }
        else
        {
          other_state = state_pointer - 1;
        }

        if (current == '*')
        {
          s.states[state_pointer].transitions.insert({'\0', other_state});
          s.states[other_state].transitions.insert({'\0', state_pointer});
        }
        else if (current == '+')
        {
          s.states[state_pointer].transitions.insert({'\0', other_state});
        }
        else
        {
          s.states[other_state].transitions.insert({'\0', state_pointer});
        }
      }
      else if (current == '|'){
        if(!depth_of_pipes.empty() && depth_of_pipes.top().first == depth){
          auto pipe_info = depth_of_pipes.top();

          state = State();
          state.is_final = false;
          state.label = "q" + std::to_string(s.states.size());
          s.states.push_back(state);

          s.states[s.states.size() - 2].transitions.insert({'\0', s.states.size() - 1});
          s.states[pipe_info.second].transitions.insert({'\0', s.states.size() - 1});

          depth_of_pipes.pop();
          depth_of_pipes.push({depth, s.states.size() - 1});

          state = State();
          state.is_final = false;
          state.label = "q" + std::to_string(s.states.size());
          s.states.push_back(state);

          s.states[pipe_info.second + 1].transitions.insert({'\0', s.states.size() - 1});
        }
        else {
          depth_of_pipes.push({depth, state_pointer});

          state = State();
          state.label = "q" + std::to_string(s.states.size());
          state.is_final = false;
          s.states.push_back(state);
          s.states[state_pointers.top()].transitions.insert({'\0', s.states.size() - 1});
        }

        state_pointer = s.states.size() - 1;
      }
      else if (current == '^' || current == '$')
      {
        std::cout << "start/end of line";
      }
      else if (current == '\\')
      {
        if (regex_pointer + 1 >= regex.size())
        {
          std::cout << "ERROR: Invalid regex. \\ is at the end of a string which is not allowed. It should always be followed by another character";
        }

        state = State();
        state.label = "q" + std::to_string(s.states.size());
        state.is_final = false;
        s.states.push_back(state);
        s.states[state_pointer].transitions.insert({'\0', s.states.size() - 1});
        state_pointer = s.states.size() - 1; 

        regex_pointer++;
        state = State();
        state.label = "q" + std::to_string(s.states.size());
        state.is_final = false;
        s.states.push_back(state);
        s.states[state_pointer].transitions.insert({regex.at(regex_pointer), s.states.size() - 1});
        state_pointer = s.states.size() - 1;
     }
      else
      {
        state = State();
        state.label = "q" + std::to_string(s.states.size());
        state.is_final = false;
        s.states.push_back(state);
        s.states[state_pointer].transitions.insert({'\0', s.states.size() - 1});
        state_pointer = s.states.size() - 1;
 
        state = State();
        state.label = "q" + std::to_string(s.states.size());
        state.is_final = false;
        s.states.push_back(state);
        s.states[state_pointer].transitions.insert({current, s.states.size() - 1});
        state_pointer = s.states.size() - 1;
     }
      regex_pointer++;
    } while (regex_pointer < regex.length());

    if(!depth_of_pipes.empty() && depth_of_pipes.top().first == depth){
      auto pipe_info = depth_of_pipes.top();
      depth_of_pipes.pop();

      state = State();
      state.is_final = false;
      state.label = "q" + std::to_string(s.states.size());
      s.states.push_back(state);

      s.states[s.states.size() - 2].transitions.insert({'\0', s.states.size() - 1});
      s.states[pipe_info.second].transitions.insert({'\0', s.states.size() - 1});

      state_pointer = s.states.size() - 1;
    }

    s.states[s.states.size() - 1].is_final = true;

    s.printFSA();

    return convertToMinDFA(s);
 
  }

  bool hasEpsilonTransitions(State s)
  {
    if (s.transitions.count('\0') > 0)
    {
      return true;
    }

    return false;
  }

  FSA convertToMinDFA(FSA fsa)
  {
    // epsilon transition removal
    FSA fsa_no_epsilon = FSA();

    State state_to_add;

    std::stack<int> index_stack;
    std::unordered_set<int> visited;
    std::unordered_map<int, int> old_new_indexes;

    index_stack.push(0);
    visited.insert(0);

    while(!index_stack.empty()){
      state_to_add = State();
      state_to_add.is_final = false;
      state_to_add.label = "q" + std::to_string(fsa_no_epsilon.states.size());

      std::stack<int> states_to_check;

      int initial = index_stack.top();
      index_stack.pop();

      int current;

      states_to_check.push(initial);

      do{
        current = states_to_check.top();
        states_to_check.pop();

        std::cout << fsa.states.size() << '\n';

        for(auto t : fsa.states.at(current).transitions){
          if(t.first == '\0'){
            states_to_check.push(t.second);
          } else{
            state_to_add.transitions.insert(t);

            if(visited.count(t.second) == 0){
              index_stack.push(t.second);
              visited.insert(t.second);
            }
          }
        }
      }while(!states_to_check.empty());

      old_new_indexes.insert({initial, fsa_no_epsilon.states.size()});
      fsa_no_epsilon.states.push_back(state_to_add);
    }

    // subset construction
    // FSA dfa = FSA();

    // main min routine so that comparing regex is easy
    // FSA min_dfa = FSA();

    return fsa_no_epsilon;
  }

  int indexOf(std::vector<int> needle, std::vector<std::vector<int>> haystack)
  {
    if(needle.empty() || haystack.empty()){
      return -1;
    }

    for (int i = 0; i < haystack.size(); i++)
    {
      if (equalVectors(needle, haystack[i]))
      {
        return i;
      }
    }

    return -1;
  }

  // compares the values of each vector, regardless of position
  bool equalVectors(std::vector<int> a, std::vector<int> b)
  {
    if(a.empty() && b.empty()){
      return true;
    } else if(a.empty()){
      return false;
    } else if(b.empty()){
      return false;
    }

    if (a.size() != b.size())
    {
      return false;
    }

    std::unordered_multiset<int> set_a(a.begin(), a.end());
    std::unordered_multiset<int> set_b(b.begin(), b.end());

    for (auto itr_a = a.begin(); itr_a != a.end(); itr_a++)
    {
      if (set_b.count(*itr_a) != set_a.count(*itr_a))
      {
        return false;
      }
    }

    return true;
  }

  bool vectorContains(int needle, std::vector<int> haystack)
  {
    if(haystack.empty()){
      return false;
    }

    for (int i = 0; i < haystack.size(); i++)
    {
      if (haystack[i] == needle)
      {
        return true;
      }
    }

    return false;
  }

  bool vectorInVector(std::vector<int> needle, std::vector<std::vector<int>> haystack)
  {
    if(needle.empty() || haystack.empty()){
      return false;
    }

    for (auto itr = haystack.begin(); itr != haystack.end(); itr++)
    {
      if (equalVectors(needle, *itr))
      {
        return true;
      }
    }

    return false;
  }

public:
  Matcher(std::string s)
  {
    regex = s;
    automata = genFSA();
    convertToMinDFA(automata);
  }

  std::string getRegex() { return regex; }

  FSA getFSA() { return automata; }

  void printFSA() { automata.printFSA(); }

  bool matchString(std::string s)
  {
    State active_state =
        automata.states.front(); // first state is always the initial state

    for (char &c : s)
    {
      if (active_state.transitions.find(c) != active_state.transitions.end())
      {
        active_state =
            automata.states[active_state.transitions.find(c)->second];
      }
      else
      {
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
