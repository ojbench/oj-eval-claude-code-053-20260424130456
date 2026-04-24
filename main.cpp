#include <iostream>
#include <string>
#include <queue>
#include <unordered_set>
#include <vector>
using namespace std;

namespace Grammar {

class NFA;
NFA MakeStar(const char &character);
NFA MakePlus(const char &character);
NFA MakeQuestion(const char &character);
NFA Concatenate(const NFA &nfa1, const NFA &nfa2);
NFA Union(const NFA &nfa1, const NFA &nfa2);
NFA MakeSimple(const char &character);

enum class TransitionType { Epsilon, a, b };

struct Transition {
  TransitionType type;
  int to;
  Transition(TransitionType type, int to) : type(type), to(to) {}
};

class NFA {
private:
  int start;
  unordered_set<int> ends;
  vector<vector<Transition>> transitions;

public:
  NFA() = default;
  ~NFA() = default;

  unordered_set<int> GetEpsilonClosure(unordered_set<int> states) const {
    unordered_set<int> closure;
    queue<int> q;
    for (const auto &state : states) {
      if (closure.find(state) != closure.end())
        continue;
      q.push(state);
      closure.insert(state);
    }
    while (!q.empty()) {
      int current = q.front();
      q.pop();
      for (const auto &transition : transitions[current]) {
        if (transition.type == TransitionType::Epsilon) {
          if (closure.find(transition.to) == closure.end()) {
            q.push(transition.to);
            closure.insert(transition.to);
          }
        }
      }
    }
    return closure;
  }

  unordered_set<int> Advance(unordered_set<int> current_states, char character) const {
    // First get epsilon closure of current states
    unordered_set<int> epsilon_closure = GetEpsilonClosure(current_states);

    // Find all states reachable through the character
    unordered_set<int> reachable;
    for (int state : epsilon_closure) {
      for (const auto &transition : transitions[state]) {
        if ((character == 'a' && transition.type == TransitionType::a) ||
            (character == 'b' && transition.type == TransitionType::b)) {
          reachable.insert(transition.to);
        }
      }
    }

    // Get epsilon closure of reachable states
    return GetEpsilonClosure(reachable);
  }

  bool IsAccepted(int state) const { return ends.find(state) != ends.end(); }

  int GetStart() const { return start; }

  friend NFA MakeStar(const char &character);
  friend NFA MakePlus(const char &character);
  friend NFA MakeQuestion(const char &character);
  friend NFA MakeSimple(const char &character);
  friend NFA Concatenate(const NFA &nfa1, const NFA &nfa2);
  friend NFA Union(const NFA &nfa1, const NFA &nfa2);
};

class RegexChecker {
private:
  NFA nfa;

public:
  bool Check(const string &str) const {
    unordered_set<int> current_states;
    current_states.insert(nfa.GetStart());

    // Process each character
    for (char c : str) {
      current_states = nfa.Advance(current_states, c);
      if (current_states.empty()) {
        return false;
      }
    }

    // After processing all characters, check if we're in an accepting state
    for (int state : current_states) {
      if (nfa.IsAccepted(state)) {
        return true;
      }
    }
    return false;
  }

  RegexChecker(const string &regex) {
    // Parse the regex and build NFA
    vector<NFA> nfa_stack;
    vector<char> op_stack;

    for (size_t i = 0; i < regex.size(); i++) {
      char c = regex[i];

      if (c == 'a' || c == 'b') {
        // Check for operators
        if (i + 1 < regex.size()) {
          char next_c = regex[i + 1];
          if (next_c == '*') {
            nfa_stack.push_back(MakeStar(c));
            i++; // Skip the operator
          } else if (next_c == '+') {
            nfa_stack.push_back(MakePlus(c));
            i++;
          } else if (next_c == '?') {
            nfa_stack.push_back(MakeQuestion(c));
            i++;
          } else {
            nfa_stack.push_back(MakeSimple(c));
          }
        } else {
          nfa_stack.push_back(MakeSimple(c));
        }
      } else if (c == '|') {
        op_stack.push_back(c);
      }
    }

    // Handle concatenation and union
    vector<NFA> temp_stack = nfa_stack;

    // First handle unions (| operator has lowest precedence)
    for (size_t i = 0; i < op_stack.size(); i++) {
      if (op_stack[i] == '|') {
        NFA right = temp_stack.back();
        temp_stack.pop_back();
        NFA left = temp_stack.back();
        temp_stack.pop_back();
        temp_stack.push_back(Union(left, right));
      }
    }

    // Then handle concatenation
    while (temp_stack.size() > 1) {
      NFA right = temp_stack.back();
      temp_stack.pop_back();
      NFA left = temp_stack.back();
      temp_stack.pop_back();
      temp_stack.push_back(Concatenate(left, right));
    }

    if (!temp_stack.empty()) {
      nfa = temp_stack[0];
    }
  }
};

NFA MakeStar(const char &character) {
  NFA nfa;
  nfa.start = 0;
  nfa.ends.insert(0);
  nfa.transitions.push_back(vector<Transition>());
  if (character == 'a') {
    nfa.transitions[0].push_back({TransitionType::a, 0});
  } else {
    nfa.transitions[0].push_back({TransitionType::b, 0});
  }
  return nfa;
}

NFA MakePlus(const char &character) {
  NFA nfa;
  nfa.start = 0;
  nfa.ends.insert(1);
  nfa.transitions.resize(2);

  // One character to match at least once
  if (character == 'a') {
    nfa.transitions[0].push_back({TransitionType::a, 1});
    nfa.transitions[1].push_back({TransitionType::a, 1});  // Loop for additional matches
  } else {
    nfa.transitions[0].push_back({TransitionType::b, 1});
    nfa.transitions[1].push_back({TransitionType::b, 1});  // Loop for additional matches
  }

  return nfa;
}

NFA MakeQuestion(const char &character) {
  NFA nfa;
  nfa.start = 0;
  nfa.ends.insert(1);
  nfa.ends.insert(0);  // Can also end at start (empty match)
  nfa.transitions.resize(2);

  // Optional single character
  if (character == 'a') {
    nfa.transitions[0].push_back({TransitionType::a, 1});
  } else {
    nfa.transitions[0].push_back({TransitionType::b, 1});
  }

  return nfa;
}

NFA Concatenate(const NFA &nfa1, const NFA &nfa2) {
  NFA result;

  // Result starts at nfa1's start
  result.start = nfa1.start;

  // Copy all transitions from both NFAs with appropriate offset
  result.transitions.resize(nfa1.transitions.size() + nfa2.transitions.size());

  // Copy nfa1 transitions
  for (size_t i = 0; i < nfa1.transitions.size(); i++) {
    for (const auto &t : nfa1.transitions[i]) {
      result.transitions[i].push_back({t.type, t.to});
    }
  }

  // Copy nfa2 transitions with offset
  int offset = nfa1.transitions.size();
  for (size_t i = 0; i < nfa2.transitions.size(); i++) {
    for (const auto &t : nfa2.transitions[i]) {
      result.transitions[i + offset].push_back({t.type, t.to + offset});
    }
  }

  // Add epsilon transitions from nfa1's end states to nfa2's start
  for (int end_state : nfa1.ends) {
    result.transitions[end_state].push_back({TransitionType::Epsilon, nfa2.start + offset});
  }

  // Result's end states are nfa2's end states
  for (int end_state : nfa2.ends) {
    result.ends.insert(end_state + offset);
  }

  return result;
}

NFA Union(const NFA &nfa1, const NFA &nfa2) {
  NFA result;

  // Create a new start state
  result.start = 0;

  // Size for both NFAs plus the new start state
  int size1 = nfa1.transitions.size();
  int size2 = nfa2.transitions.size();
  result.transitions.resize(1 + size1 + size2);

  // Epsilon from new start to both NFAs' starts
  result.transitions[0].push_back({TransitionType::Epsilon, 1});
  result.transitions[0].push_back({TransitionType::Epsilon, 1 + size1});

  // Copy nfa1 transitions with offset
  for (size_t i = 0; i < nfa1.transitions.size(); i++) {
    for (const auto &t : nfa1.transitions[i]) {
      result.transitions[i + 1].push_back({t.type, t.to + 1});
    }
  }

  // Copy nfa2 transitions with offset
  for (size_t i = 0; i < nfa2.transitions.size(); i++) {
    for (const auto &t : nfa2.transitions[i]) {
      result.transitions[i + 1 + size1].push_back({t.type, t.to + 1 + size1});
    }
  }

  // End states are both NFAs' end states
  for (int end_state : nfa1.ends) {
    result.ends.insert(end_state + 1);
  }
  for (int end_state : nfa2.ends) {
    result.ends.insert(end_state + 1 + size1);
  }

  return result;
}

NFA MakeSimple(const char &character) {
  NFA nfa;
  nfa.start = 0;
  nfa.ends.insert(1);
  nfa.transitions.resize(2);

  // Single character transition
  if (character == 'a') {
    nfa.transitions[0].push_back({TransitionType::a, 1});
  } else {
    nfa.transitions[0].push_back({TransitionType::b, 1});
  }

  return nfa;
}

} // namespace Grammar

int main() {
  string regex, str;
  cin >> regex >> str;

  Grammar::RegexChecker checker(regex);
  cout << (checker.Check(str) ? "Yes" : "No") << endl;

  return 0;
}