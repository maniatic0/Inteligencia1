#ifndef NODE_HPP
#define NODE_HPP 1

#include <cstdio>
#include <memory>
#include <utility>

/* Node Color */
enum class Color { White = 0, Gray = 1, Black = 2 };

/* Node Class to help save paths when iterating through a problem */
class Node : public std::enable_shared_from_this<Node> {
public:
  /* Create a Node with a state from a parent with a rule. DO NOT CALL this
   * directly */
  inline Node(state_t state, std::shared_ptr<Node> parent, int rule)
      : m_parent(std::move(parent)), m_rule(rule), m_state(state) {}

  Node(Node &&) noexcept = default;
  Node(const Node &) = default;
  Node &operator=(Node &&) noexcept = default;
  Node &operator=(const Node &) = default;
  inline ~Node() = default;

  /* Create a Node with a state from a parent with a rule */
  static std::shared_ptr<Node>
  CreateNode(state_t state, std::shared_ptr<Node> parent, int rule) {
    return std::make_shared<Node>(state, std::move(parent), rule);
  }

  /* Get Parent Node */
  inline std::shared_ptr<Node> GetParent() { return m_parent; }

  /* Get Rule from which we were derived */
  inline int GetRule() { return m_rule; }

  /* Get Associated State */
  inline const state_t *GetState() { return &m_state; }

  /* Create a new node with this one as its parent */
  inline std::shared_ptr<Node> MakeNode(state_t state, int rule) {
    return std::make_shared<Node>(state, shared_from_this(), rule);
  }

private:
  /* Who is our parent in the search */
  std::shared_ptr<Node> m_parent = nullptr;

  /* Which rule generated this node */
  int m_rule = -1;

  /* Our current state */
  state_t m_state;
};

/* Prints to console a path. It prints from the goal to the start */
void PrintPath(std::shared_ptr<Node> node) {
  std::printf("Printing path from goal to ORIGIN\n");
  // The variable node contains the goal node
  while (node->GetParent() != nullptr) {
    print_state(stdout, node->GetState());
    std::printf("  From %s (cost %d), goal=%d\n",
                get_fwd_rule_label(node->GetRule()),
                get_fwd_rule_cost(node->GetRule()), is_goal(node->GetState()));

    node = node->GetParent();
  }

  print_state(stdout, node->GetState());
  std::printf(" ORIGIN, goal=%d\n", is_goal(node->GetState()));
}

#endif