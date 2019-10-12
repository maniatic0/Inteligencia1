#ifndef NODE_HPP
#define NODE_HPP 1

#include <cstdint>
#include <cstdio>
#include <inttypes.h>
#include <memory>
#include <utility>

/* Node Color */
enum class Color { White = 0, Gray = 1, Black = 2 };

/* Node Class to help save paths when iterating through a problem */
class Node : public std::enable_shared_from_this<Node> {
public:
  /* Create a Node with a state from a parent with a rule, history and
   * accumulated cost. DO NOT CALL this directly */
  explicit inline Node(state_t state, std::shared_ptr<Node> &&parent, int rule,
                       int history, long long unsigned cost = 0)
      : m_parent(std::move(parent)), m_rule(rule), m_state(state),
        m_history(history), m_cost(cost) {}

  /* Create a Node with a state from a parent with a rule, history and
   * accumulated cost. DO NOT CALL this directly */
  explicit inline Node(state_t state, const std::shared_ptr<Node> &parent,
                       int rule, int history, long long unsigned cost = 0)
      : m_parent(parent), m_rule(rule), m_state(state), m_history(history),
        m_cost(cost) {}

  Node(Node &&) noexcept = default;
  Node(const Node &) = default;
  Node &operator=(Node &&) noexcept = default;
  Node &operator=(const Node &) = default;
  inline ~Node() = default;

  /* Create a Node with a state from a parent with a rule, history and
   * accumulated cost */
  static std::shared_ptr<Node> CreateNode(state_t state,
                                          std::shared_ptr<Node> &&parent,
                                          int rule, int history,
                                          long long unsigned cost = 0) {
    return std::make_shared<Node>(state, std::move(parent), rule, history,
                                  cost);
  }

  /* Create a Node with a state from a parent with a rule, history and
   * accumulated cost */
  static std::shared_ptr<Node> CreateNode(state_t state,
                                          const std::shared_ptr<Node> &parent,
                                          int rule, int history,
                                          long long unsigned cost = 0) {
    return std::make_shared<Node>(state, parent, rule, history, cost);
  }

  /* Get Parent Node */
  inline std::shared_ptr<Node> GetParent() { return m_parent; }

  /* Set Parent Node */
  inline void SetParent(std::shared_ptr<Node> &&parent) {
    m_parent = std::move(parent);
  }

  /* Set Parent Node */
  inline void SetParent(const std::shared_ptr<Node> &parent) {
    m_parent = parent;
  }

  /* Get Rule from which we were derived */
  inline int GetRule() { return m_rule; }

  /* Set Rule from which we were derived */
  inline void SetRule(int rule) { m_rule = rule; }

  /* Get Associated State */
  inline const state_t *GetState() { return &m_state; }

  /* Set Associated State */
  inline void SetState(state_t state) { m_state = state; }

  /* Set History */
  inline int GetHistory() { return m_history; }

  /* Set History */
  inline void SetHistory(int history) { m_history = history; }

  /* Get Cost */
  inline long long unsigned GetCost() { return m_cost; }

  /* Set Cost */
  inline void SetCost(long long unsigned cost) { m_cost = cost; }

  /* Create a new node with this one as its parent */
  inline std::shared_ptr<Node> MakeNode(state_t state, int rule, int history,
                                        long long unsigned heuristic = 0) {
    return std::make_shared<Node>(state, shared_from_this(), rule, history,
                                  m_cost + get_fwd_rule_cost(GetRule()) +
                                      heuristic);
  }

private:
  /* Who is our parent in the search */
  std::shared_ptr<Node> m_parent = nullptr;

  /* Which rule generated this node */
  int m_rule = -1;

  /* Our current state */
  state_t m_state;

  /* History of this node */
  int m_history = -1;

  /* Accumulated path cost */
  long long unsigned m_cost = 0;
};

/* Prints to console a path. It prints from the goal to the start */
void PrintPath(std::shared_ptr<Node> node) {
  std::printf("Printing path from goal to ORIGIN\n");

  std::int64_t pathCost = 0;

  // The variable node contains the goal node
  while (node->GetParent() != nullptr) {
    print_state(stdout, node->GetState());
    pathCost += get_fwd_rule_cost(node->GetRule());
    std::printf("  From %s (cost %d), goal=%d,",
                get_fwd_rule_label(node->GetRule()),
                get_fwd_rule_cost(node->GetRule()), is_goal(node->GetState()));

    std::printf(" accumulated=%" PRId64 "\n", pathCost);

    node = node->GetParent();
  }

  print_state(stdout, node->GetState());
  std::printf(" ORIGIN, goal=%d", is_goal(node->GetState()));
  std::printf(" accumulated=%" PRId64 "\n", pathCost);
}

#endif