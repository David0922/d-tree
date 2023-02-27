#ifndef D_TREE
#define D_TREE

#include <cstdint>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using ID = std::uint64_t;

class Node {
  ID parent_id_;
  bool is_root_;
  std::unordered_map<ID, std::unordered_set<ID>> neighbor_edges_;
  std::unordered_map<ID, ID> child_edge_;

 public:
  Node();

  ID parent_id() const;
  void set_parent(ID parent_id);

  bool is_root() const;

  const std::unordered_map<ID, std::unordered_set<ID>>& neighbors() const;

  ID child_edge(ID child_id);

  void add_neighbor(ID neighbor_id, ID edge_id);
  ID add_child(ID child_id);
  void add_child(ID child_id, ID edge_id);

  void delete_parent();
  void delete_child(ID child_id);

  void delete_edge(ID neighbor_id, ID edge_id);
};

extern "C" {
struct Edge {
  ID node_u_id, node_v_id, edge_id;
};

class DTree {
  std::unordered_map<ID, Node> nodes;
  std::unordered_map<ID, Edge> all_edges, tree_edges;

  ID root(ID node_id);

  void connect_tree_edge(ID parent_id, ID child_id);

  void re_root(ID new_root_id);

 public:
  DTree(Edge* edges, size_t n);

  bool connected(ID node_u_id, ID node_v_id);

  void add_edge(Edge);

  void delete_edge(ID edge_id);
};
}

#endif /* D_TREE */
