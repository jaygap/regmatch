#include <algorithm>
#include <cctype>
#include <iostream>
#include <queue>
#include <set>
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
                        return std::vector<State>{};
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
                } else if (current == '[') {
                    std::string range_definition = "";

                    regex_pointer++;
                    current = regex.at(regex_pointer);

                    do{
                        range_definition += current;
                        regex_pointer++;
                        current = regex.at(regex_pointer);
                    } while(current != ']');

                    //check ranges are valid (no letter-number or nonalphanumeric-anything)
                    for(int i = 0; i < range_definition.length(); i++){

                        if(range_definition.at(i) == '\\' && i + 1 >= range_definition.length()){
                            std::cout << "ERROR: the range definition: [" + range_definition + "] is not valid as it ends with a '\\' without specifying the character.";
                            return std::vector<State>{};
                        }

                        if(range_definition.at(i) != '-'){
                            continue;
                        }

                        const std::unordered_set<char> alphanumeric_char = {  'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l',
                                                                        'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x',
                                                                        'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};

                        if(i > 0 && range_definition.at(i - 1) == '\\'){
                            continue;
                        }

                        if(i + 1 >= range_definition.length()){
                            std::cout << "ERROR: the range definition: [" + range_definition + "] is not valid as it does only defines the start of a range and not the end.\n";
                            return std::vector<State>{};
                        }

                        if(alphanumeric_char.count(std::tolower(range_definition.at(i - 1))) == 0 || alphanumeric_char.count(std::tolower(range_definition.at(i + 1))) == 0){
                            std::cout << "ERROR: the range definition: [" + range_definition + "] is not valid as it defines a range using non-alphanumeric characters.\n";
                            return std::vector<State>{};
                        }

                        if(0x30 <= range_definition.at(i - 1) && range_definition.at(i - 1) <= 0x39){
                            if(0x30 > range_definition.at(i + 1) || range_definition.at(i + 1) > 0x39){
                                std::cout << "ERROR: the range definition: [" + range_definition + "] is not valid as it defines a range from a number to a letter.\n";
                                return std::vector<State>{};
                            }
                        } else{
                            if(0x30 <= range_definition.at(i + 1) && range_definition.at(i + 1) <= 0x39){
                                std::cout << "ERROR: the range definition: [" + range_definition + "] is not valid as it defines a range from a letter to a number.\n";
                                return std::vector<State>{};
                            }
                        }

                    }

                    std::unordered_set<char> range = getCharInRange(range_definition);

                    state = State();
                    state.label = "q" + std::to_string(s.size());
                    s.push_back(state);
                    s[state_pointer].transitions.insert({'\0', s.size() - 1});
                    state_pointer = s.size() - 1;

                    state = State();
                    state.label = "q" + std::to_string(s.size());
                    s.push_back(state);

                    for(char c : range){
                        s[state_pointer].transitions.insert({c, s.size() - 1});
                    }

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

            return s;
        }

        std::unordered_set<char> getCharInRange(std::string range_definition) {
            std::unordered_set<char> valid_chars;
            const int LOWERCASE_ASCII_START = 0x61;
            const int UPPERCASE_ASCII_START = 0x41;
            int pointer = 0;

            for(int i = 0; i < range_definition.size(); i++){
                if(range_definition.at(i) == '-'){
                    char start = range_definition.at(i - 1);
                    char end = range_definition.at(i + 1);

                    //if its a range of numbers, or its a range of letters in the same case (upper-upper or lower-lower)
                    if((0x30 <= start && start <= 0x39 && 0x30 <= end && end <= 0x39) || (std::tolower(start) == start && std::tolower(end) == end) || (std::toupper(start) == start) && std::toupper(end)){
                        for(int i = 0; start + i <= end; i++){
                            valid_chars.insert(start + i);
                        }
                    } else{
                        std::pair<char, char> lowercase_bounds = {std::tolower(start), std::tolower(end)};
                        std::pair<char, char> uppercase_bounds = {std::toupper(start), std::toupper(end)};

                        for(int i = 0; lowercase_bounds.first + i <= lowercase_bounds.second; i++){
                            valid_chars.insert(lowercase_bounds.first + i);
                            valid_chars.insert(uppercase_bounds.first + i);
                        }
                    }

                } else{
                    if(range_definition.at(i) == '\\'){
                        valid_chars.insert(range_definition.at(i + 1));
                        i++;
                    }

                    valid_chars.insert(range_definition.at(i));
                }
            }

            return valid_chars;
        }

        bool hasEpsilonTransitions(State s) {
            if (s.transitions.count('\0') > 0) {
                return true;
            }

            return false;
        }

        std::vector<State>
            removeEpsilonTransitions(const std::vector<State> &initial_fsa) {
                // epsilon transition removal
                std::vector<State> fsa_no_epsilon;

                // lambda function to calculate epsilon closure
                auto epsilonClosure = [&fsa_no_epsilon, initial_fsa](
                        int start_index) -> std::unordered_set<int> {
                    std::unordered_set<int> closure;
                    std::stack<int> stack;
                    stack.push(start_index);

                    closure.insert(start_index);

                    while (!stack.empty()) {
                        int current = stack.top();
                        stack.pop();

                        for (auto t : initial_fsa[current].transitions) {
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

                // creates a string that is a unique key for epsilon-closure
                auto getClosureKey =
                    [](const std::unordered_set<int> &closure) -> std::string {
                        std::vector<int> sorted(closure.begin(), closure.end());
                        std::sort(sorted.begin(), sorted.end());
                        std::string key;

                        for (int index : sorted) {
                            key += std::to_string(index) + ",";
                        }

                        return key;
                    };

                // creates a state with a given label and epsilon-closure
                auto createState =
                    [initial_fsa](const std::string &label,
                            const std::unordered_set<int> &closure) -> State {
                        State state;
                        state.label = label;
                        state.is_final = false;

                        for (int index : closure) {
                            if (initial_fsa[index].is_final) {
                                state.is_final = true;
                            }
                        }

                        return state;
                    };

                std::unordered_map<std::string, int> closure_to_index;
                std::unordered_set<int> first_closure = epsilonClosure(0);
                std::string start_key = getClosureKey(first_closure);

                closure_to_index.insert({start_key, 0});

                State start_state = createState("q0", first_closure);

                std::unordered_map<std::string, std::pair<char, std::string>>
                    new_transitions;

                fsa_no_epsilon.push_back(start_state);

                std::queue<std::unordered_set<int>> unmarked;
                unmarked.push(first_closure);

                while (!unmarked.empty()) {

                    std::unordered_set<int> current_closure = unmarked.front();
                    unmarked.pop();

                    std::string current_key = getClosureKey(current_closure);
                    int current_index = closure_to_index[current_key];

                    std::unordered_multimap<char, std::unordered_set<int>> symbol_to_closure;

                    for (int state_index : current_closure) {
                        for (auto t : initial_fsa[state_index].transitions) {
                            if (t.first == '\0') {
                                continue;
                            }

                            char symbol = t.first;
                            int new_origin_state_index = t.second;

                            std::unordered_set<int> closure =
                                epsilonClosure(new_origin_state_index);
                            std::string new_key = getClosureKey(closure);
                            new_transitions.insert({current_key, {symbol, new_key}});
                            symbol_to_closure.insert({symbol, closure});
                        }
                    }

                    for (auto &entry : symbol_to_closure) {
                        char symbol = entry.first;
                        auto closure = entry.second;

                        std::string closure_key = getClosureKey(closure);
                        int target_index;

                        if (closure_to_index.find(closure_key) == closure_to_index.end()) {
                            target_index = fsa_no_epsilon.size();
                            closure_to_index.insert({closure_key, target_index});

                            State new_state =
                                createState("q" + std::to_string(target_index), closure);

                            fsa_no_epsilon.push_back(new_state);
                            unmarked.push(closure);
                        } else {
                            target_index = closure_to_index[closure_key];
                        }

                        fsa_no_epsilon[current_index].transitions.insert(
                                {symbol, target_index});
                    }
                }

                return fsa_no_epsilon;
            }

        // makes fsa detereministic
        std::vector<State> subsetConstruction(const std::vector<State> &initial_fsa) {
            std::vector<State> dfa;

            auto setStatesReachable = [](const State &state)
                -> std::unordered_map<char, std::unordered_set<int>> {
                    std::unordered_map<char, std::unordered_set<int>> set_states;

                    for (auto t : state.transitions) {
                        set_states[t.first].insert(t.second);
                    }

                    return set_states;
                };

            auto getSetStateKey = [](std::unordered_set<int> set_state) -> std::string {
                std::vector<int> sorted(set_state.begin(), set_state.end());
                std::sort(sorted.begin(), sorted.end());
                std::string key;

                for (int index : sorted) {
                    key += std::to_string(index) + ",";
                }

                return key;
            };

            auto createState =
                [initial_fsa](std::string label,
                        std::unordered_set<int> set_state) -> State {
                    State state;
                    state.label = label;
                    state.is_final = false;

                    for (int index : set_state) {
                        if (initial_fsa[index].is_final) {
                            state.is_final = true;
                        }
                    }

                    return state;
                };

            std::unordered_map<std::string, int> set_to_index;
            std::unordered_set<int> initial_set;
            initial_set.insert(0);

            State state = createState("q0", initial_set);
            dfa.push_back(state);

            std::string initial_key = getSetStateKey(initial_set);
            set_to_index.insert({initial_key, 0});

            std::queue<std::unordered_set<int>> sets_to_visit;

            sets_to_visit.push(initial_set);

            while (!sets_to_visit.empty()) {
                std::unordered_set<int> current_set = sets_to_visit.front();
                sets_to_visit.pop();

                std::string current_key = getSetStateKey(current_set);
                int current_index = set_to_index[current_key];

                std::unordered_map<char, std::unordered_set<int>> symbol_to_set;

                for (int index : current_set) {
                    auto temp = setStatesReachable(initial_fsa[index]);

                    for (auto t : temp) {
                        if (symbol_to_set.find(t.first) == symbol_to_set.end()) {
                            symbol_to_set.insert(t);
                        } else {
                            std::unordered_set<int> *set = &symbol_to_set[t.first];

                            for (int i : t.second) {
                                set->insert(i);
                            }
                        }
                    }
                }

                for (auto t : symbol_to_set) {
                    std::string key = getSetStateKey(t.second);
                    int target_index;

                    if (set_to_index.find(key) == set_to_index.end()) {
                        target_index = dfa.size();
                        set_to_index.insert({key, target_index});

                        state = createState("q" + std::to_string(target_index), t.second);

                        dfa.push_back(state);
                        sets_to_visit.push(t.second);
                    } else {
                        target_index = set_to_index[key];
                    }

                    dfa[current_index].transitions.insert({t.first, target_index});
                }
            }

            return dfa;
        }

        std::vector<State> minimiseDFA(const std::vector<State> &initial_fsa) {
            std::vector<State> min_dfa;

            std::set<std::pair<int, int>> state_pairs;

            for (int i = 0; i < initial_fsa.size(); i++) {
                for (int x = i + 1; x < initial_fsa.size(); x++) {
                    state_pairs.insert({i, x});
                }
            }

            std::set<std::pair<int, int>> marked_states;
            bool marked_set_updated = true;

            while (marked_set_updated) {
                marked_set_updated = false;

                for (auto itr = state_pairs.begin(); itr != state_pairs.end(); itr++) {
                    if (marked_states.find({itr->first, itr->second}) !=
                            marked_states.end()) {
                        continue;
                    }

                    State state1 = initial_fsa[itr->first];
                    State state2 = initial_fsa[itr->second];

                    if (state1.is_final != state2.is_final) {
                        marked_states.insert({itr->first, itr->second});
                        marked_set_updated = true;
                        continue;
                    }

                    auto transitions1 = state1.transitions;
                    auto transitions2 = state2.transitions;

                    if (transitions1.size() != transitions2.size()) {
                        marked_states.insert({itr->first, itr->second});
                        marked_set_updated = true;
                        continue;
                    }

                    for (auto t1 : transitions1) {

                        auto t2 = transitions2.find(t1.first);

                        if (t2 == transitions2.end()) {
                            marked_states.insert({itr->first, itr->second});
                            marked_set_updated = true;
                            goto property_matched;
                        } else if (marked_states.find({t1.second, t2->second}) !=
                                marked_states.end()) {
                            marked_states.insert({itr->first, itr->second});
                            marked_set_updated = true;
                            goto property_matched;
                        }
                    }

                    property_matched:;
                }
            }

            for (auto pair : marked_states) {
                state_pairs.erase(state_pairs.find(pair));
            }

            std::unordered_map<int, int> index_to_merged_index;

            for (auto pair : state_pairs) {
                index_to_merged_index.insert({pair.second, pair.first});
            }

            for (int i = 0; i < initial_fsa.size(); i++) {
                if (index_to_merged_index.count(i) == 0) {
                    // need to be able to determine if state is result of merging multiple

                    State new_state;
                    new_state.label = "q" + std::to_string(min_dfa.size());
                    new_state.is_final = initial_fsa[i].is_final;

                    for (auto t : initial_fsa[i].transitions) {
                        int transition_end;

                        if (index_to_merged_index.count(t.second) != 0) {
                            transition_end = index_to_merged_index[t.second];
                        } else {
                            transition_end = t.second;
                        }

                        new_state.transitions.insert({t.first, transition_end});
                    }

                    min_dfa.push_back(new_state);
                }
            }

            return min_dfa;
        }

    public:
        Matcher(std::string s) {
            regex = s;
            fsa = genFSA();

            if(fsa.empty()){
                std::cout << "There has been an error in generating the FSA.\n";
                return;
            }

            // convert to min-dfa
            fsa = removeEpsilonTransitions(fsa);
            fsa = subsetConstruction(fsa);
            printFSA();
            fsa = minimiseDFA(fsa);
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

        bool compareRegex(Matcher m) {
            // check if the regex of self and m are equivalent

            return false;
        }

        bool compareRegex(std::string s) { return compareRegex(Matcher(s)); }
};
