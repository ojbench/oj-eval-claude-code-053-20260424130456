#pragma once
#include <queue>
#include <string>
#include <unordered_set>
#include <vector>
// You are NOT allowed to add any headers
// without the permission of TAs.
namespace Grammar {
class NFA;
NFA MakeStar(const char &character);
NFA MakePlus(const char &character);
NFA MakeQuestion(const char &character);
NFA Concatenate(const NFA &nfa1, const NFA &nfa2);
NFA Union(const NFA &nfa1, const NFA &nfa2);
NFA MakeSimple(const char &character);
/*!
  \brief This class is used to store the type of the transition.
  \details You can use it like this:
  \code
      TransitionType type = TransitionType::Epsilon;
      TransitionType type2 = TransitionType::a;
      TransitionType type3 = TransitionType::b;
  \endcode
*/
enum class TransitionType { Epsilon, a, b };
struct Transition {
  /*!
    \brief value is used to store which character to match.
  */
  TransitionType type;
  /*!
    \brief This is used to store which node the transition is going to.
  */
  int to;
  Transition(TransitionType type, int to) : type(type), to(to) {}
};
class NFA {
private:
  /*!
    \brief This is used to store the start state of the NFA.
  */
  int start;
  /*!
    \brief This is used to store the end state of the NFA.
  */
  std::unordered_set<int> ends;
  /*!
    \brief This is used to store the transitions of the NFA.
    \details For example, transitions[3] stores all the transitions beginning
    \details with state 3.
  */
  std::vector<std::vector<Transition>> transitions;

public:
  NFA() = default;
  ~NFA() = default;
  /*!
    \brief Get the epsilon closure of a state.
    \param state The state to get the epsilon closure of.
    \return The epsilon closure of the state.
*/
  std::unordered_set<int>
  GetEpsilonClosure(std::unordered_set<int> states) const {
    std::unordered_set<int> closure;
    std::queue<int> queue;
    for (const auto &state : states) {
      if (closure.find(state) != closure.end())
        continue;
      queue.push(state);
      closure.insert(state);
    }
    while (!queue.empty()) {
      int current = queue.front();
      queue.pop();
      for (const auto &transition : transitions[current]) {
        if (transition.type == TransitionType::Epsilon) {
          if (closure.find(transition.to) == closure.end()) {
            queue.push(transition.to);
            closure.insert(transition.to);
          }
        }
      }
    }
    return closure;
  }
  /*!
    \brief Advance the NFA to the next states.
    \param current_states The current states of the NFA.
    \param character The character to match.
    \return The next states of the NFA.
  */
  std::unordered_set<int> Advance(std::unordered_set<int> current_states,
                                  char character) const {
    // First get epsilon closure of current states
    std::unordered_set<int> epsilon_closure = GetEpsilonClosure(current_states);

    // Find all states reachable through the character
    std::unordered_set<int> reachable;
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

  /*!
    \brief Check if a state is accepted.
    \param state The state to check.
    \return true if the state is accepted, false otherwise.
  */
  bool IsAccepted(int state) const { return ends.find(state) != ends.end(); }

  /*!
    \brief Return the start state.
    \return The start state.
  */

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
  /*!
    \brief This is used to store the regex string.
  */
  NFA nfa;

public:
  /*!
    \brief To check if the string is accepted by the regex.
    \param str The string to be checked.
    \return true if the string is accepted by the regex, false otherwise.
  */
  bool Check(const std::string &str) const {
    std::unordered_set<int> current_states;
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
  /*!
    \brief This is used to build the NFA from the regex string.
  */
  RegexChecker(const std::string &regex) {
    // Parse the regex and build NFA
    std::vector<NFA> nfas;

    // First, split by | (union operator)
    std::vector<std::string> parts;
    std::string current;

    for (size_t i = 0; i < regex.size(); i++) {
      if (regex[i] == '|') {
        parts.push_back(current);
        current.clear();
      } else {
        current += regex[i];
      }
    }
    if (!current.empty()) {
      parts.push_back(current);
    }

    // If we have multiple parts, we need to union them
    if (parts.size() > 1) {
      // Parse each part separately and union them
      std::vector<NFA> part_nfas;
      for (const auto &part : parts) {
        part_nfas.push_back(ParsePart(part));
      }

      // Union all parts
      nfa = part_nfas[0];
      for (size_t i = 1; i < part_nfas.size(); i++) {
        nfa = Union(nfa, part_nfas[i]);
      }
    } else {
      // Single part, just parse it
      nfa = ParsePart(regex);
    }
  }

private:
  NFA ParsePart(const std::string &part) {
    // Parse a part without | operators (handle concatenation and +, *, ?)
    std::vector<NFA> nfas;

    for (size_t i = 0; i < part.size(); i++) {
      char c = part[i];

      if (c == 'a' || c == 'b') {
        // Check for operators
        if (i + 1 < part.size()) {
          char next_c = part[i + 1];
          if (next_c == '*') {
            nfas.push_back(MakeStar(c));
            i++; // Skip the operator
          } else if (next_c == '+') {
            nfas.push_back(MakePlus(c));
            i++;
          } else if (next_c == '?') {
            nfas.push_back(MakeQuestion(c));
            i++;
          } else {
            nfas.push_back(MakeSimple(c));
          }
        } else {
          nfas.push_back(MakeSimple(c));
        }
      }
    }

    // Concatenate all NFAs
    if (nfas.empty()) {
      // Empty NFA (shouldn't happen with proper input)
      return MakeStar('a'); // a* accepts empty string
    }

    NFA result = nfas[0];
    for (size_t i = 1; i < nfas.size(); i++) {
      result = Concatenate(result, nfas[i]);
    }

    return result;
  }
};

/*!
  \brief Return a NFA to match a* or b*.
  \param character The character to match.
  \details This function will create a NFA that matches a* or b*.
*/
NFA MakeStar(const char &character) {
  NFA nfa;
  nfa.start = 0;
  nfa.ends.insert(0);
  nfa.transitions.push_back(std::vector<Transition>());
  if (character == 'a') {
    nfa.transitions[0].push_back({TransitionType::a, 0});
  } else {
    nfa.transitions[0].push_back({TransitionType::b, 0});
  }
  return nfa;
}
/*!
  \brief Return a NFA to match a+ or b+.
  \param character The character to match.
  \details This function will create a NFA that matches a+ or b+.
*/
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
/*!
  \brief Return a NFA to match a? or b?.
  \param character The character to match.
  \details This function will create a NFA that matches a? or b?.
*/
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

/*!
  \brief Return a NFA to match ...(the first part) ...(the second part).
  \param nfa1 The first NFA to match, for example, it is a.
  \param nfa2 The second NFA to match, for example, it is b.
  \details This function will create a NFA that matches ab.
*/
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
/*!
  \brief Return a NFA to match ... | ...
  \param nfa1 The first NFA to match, for example, it is a.
  \param nfa2 The second NFA to match, for example, it is b.
  \details This function will create a NFA that matches a|b.
*/
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

/*!
  \brief Return a NFA to match a, or return a NFA to match b.
  \param character The character to match.
  \return The required NFA.
*/
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