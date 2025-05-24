#include <string>
#include <vector>
#include <stack>
#include <unordered_map>
#include <unordered_set>

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

struct State{
    std::unordered_multimap<char, int> transitions; //int is index of the state in the FSA
    bool is_final;
    std::string label;

    State(){};
};

struct FSA{
    std::vector<State> states;

    FSA(){};

    void printFSA(){
        std::cout << "States: {";

        for(int i = 0; i < states.size(); i++){
            std::cout << states[i].label;

            if(i + 1 < states.size()){
                std::cout << ", ";
            }
        }
        std::cout << "}\nTransitions:\n{\n";

        for(State s : states){
            for(auto transition : s.transitions){
                std::cout << "(" << s.label << ", " << transition.first << ", " << states[transition.second].label << ")\n"; 
            }
        }
        
        std::cout << '}' << std::endl;
    }
};

class Matcher{
    private:
    FSA automata;
    std::string regex = "";

    bool checkBrackets(char left, char right){
        if(left == '(' && right == ')'){
            return true;
        }
    
        if(left == '[' && right == ']'){
            return true;
        }
    
        if(left == '{' && right == '}'){
            return true;
        }
    
        return false;
    }

        FSA genFSA(){
        FSA s;
        int regex_pointer = 0;
        int state_pointer = 0;
        std::stack<int> state_pointers;
        char current;
        State state = State();

        state.label = "q0";
        state.is_final = false;
        s.states.push_back(state); //initial state

        do{
            current = regex.at(regex_pointer);

            if(current == '('){
                state_pointers.push(state_pointer);
            } else if(current == ')'){
                //do nothing
            } else if(current == '*' || current == '+' || current == '?'){
                if(regex_pointer == 0){
                    std::cout << "ERROR: Invalid regex. * or + or ? come at the start of the regex which is not allowed." << std::endl;
                    return FSA();
                }

                char prev = regex.at(regex_pointer - 1);
                
                if(prev == ')'){
                    if(current == '*'){
                        s.states[state_pointer].transitions.insert({'\0', state_pointers.top()});
                        s.states[state_pointers.top()].transitions.insert({'\0', state_pointer});
                    } else if (current == '+'){
                        s.states[state_pointer].transitions.insert({'\0', state_pointers.top()});
                    } else{
                        s.states[state_pointers.top()].transitions.insert({'\0', state_pointer});    
                    }

                    state_pointers.pop();
                } else{
                    s.states[state_pointer].transitions.insert({'\0', state_pointer - 1});
                }
            } else if(current == '|'){

            } else if(current == '^' || current == '$'){

            } else if(current == '\\'){

            } else{
                state = State();
                state.label = "q" + std::to_string(s.states.size());
                state.is_final = false;
                s.states.push_back(state);
                s.states[state_pointer].transitions.insert({current, s.states.size() - 1});
                state_pointer = s.states.size() - 1;
            }

            regex_pointer++;
        } while(regex_pointer < regex.length());

        s.states[s.states.size() - 1].is_final = true;

        return s;
    }

    bool hasEpsilonTransitions(State s){
        if(s.transitions.count('\0') > 0){
            return true;
        }

        return false;
    }

    public:
    FSA convertToMinDFA(FSA fsa){
        FSA fsa_no_epsilon = FSA();
        State state_to_add = State();

        state_to_add.label = "q0";
        state_to_add.is_final = false;
        fsa_no_epsilon.states.push_back(state_to_add);

        //remove \0 (epsilon transitions)
        for(int i = 0; i < fsa.states.size(); i++){
            State current_state = fsa.states[i];
            state_to_add = State();
            std::unordered_set<int> visited;
            std::stack<int> stack;

            stack.push(i);
            visited.insert(i);

            while(hasEpsilonTransitions(fsa.states[stack.top()])){
                current_state = fsa.states[current_state.transitions.find('\0')->second];
                stack.push(current_state.transitions.find('\0')->second);
                visited.insert(stack.top());
            }

            do{
                for(auto t : current_state.transitions){
                    if(t.first != '\0'){
                        state_to_add.transitions.insert(t);
                    }
                }

                if(current_state.transitions.count('\0') != 0){
                    current_state = fsa.states[current_state.transitions.find('\0')->second];
                } else{
                    break;
                }

            } while(!stack.empty());
        }

        //remove duplicate outgoing transitions
        //FSA dfa = FSA();

        //main min routine so that comparing regex is easy
        //FSA min_dfa = FSA();

        return fsa_no_epsilon;
    }

    Matcher(std::string s){
        regex = s;
        automata = genFSA();
    }

    std::string getRegex(){
        return regex;
    }

    FSA getFSA(){
        return automata;
    }

    void printFSA(){
        automata.printFSA();
    }
    
    bool matchString(std::string s){
        State active_state = automata.states.front(); //first state is always the initial state
        
        for(char& c : s){
            if(active_state.transitions.find(c) != active_state.transitions.end()){
                active_state = automata.states[active_state.transitions.find(c)->second];
            } else{
                return false;
            }
        }

        return active_state.is_final;
    }

    bool compareRegex(Matcher m){
        //check if the regex of self and m are equivalent

        return false;
    }

    bool compareRegex(std::string s){
        return compareRegex(Matcher(s));
    }
};