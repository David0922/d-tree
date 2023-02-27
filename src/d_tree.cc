#include "d_tree.h"

#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

Node::Node() : is_root_(true) {}

ID Node::parent_id() const { return parent_id_; }

void Node::set_parent(ID parent_id) {
  parent_id_ = parent_id;
  is_root_ = false;
}

bool Node::is_root() const { return is_root_; }

const std::unordered_map<ID, std::unordered_set<ID>>& Node::neighbors() const {
  return neighbor_edges_;
}

ID Node::child_edge(ID child_id) { return child_edge_[child_id]; }

void Node::add_neighbor(ID neighbor_id, ID edge_id) {
  neighbor_edges_[neighbor_id].insert(edge_id);
}

ID Node::add_child(ID child_id) {
  ID edge_id = *neighbor_edges_[child_id].begin();
  add_child(child_id, edge_id);
  return edge_id;
}

void Node::add_child(ID child_id, ID edge_id) {
  child_edge_[child_id] = edge_id;
}

void Node::delete_parent() {
  // parent remains a neighbor
  is_root_ = true;
}

void Node::delete_child(ID child_id) {
  // child remains a neighbor
  child_edge_.erase(child_id);
}

void Node::delete_edge(ID neighbor_id, ID edge_id) {
  if (neighbor_id == parent_id_) delete_parent();

  if (child_edge_.find(neighbor_id) != child_edge_.end() &&
      child_edge_[neighbor_id] == edge_id)
    delete_child(neighbor_id);

  neighbor_edges_[neighbor_id].erase(edge_id);

  if (neighbor_edges_[neighbor_id].empty()) neighbor_edges_.erase(neighbor_id);
}

ID DTree::root(ID node_id) {
  const auto& node = nodes[node_id];
  return node.is_root() ? node_id : root(node.parent_id());
}

void DTree::connect_tree_edge(ID parent_id, ID child_id) {
  ID edge = nodes[parent_id].add_child(child_id);
  nodes[child_id].set_parent(parent_id);
  tree_edges[edge] = all_edges[edge];
}

void DTree::re_root(ID new_root_id) {
  Node& new_root = nodes[new_root_id];

  if (new_root.is_root()) return;

  ID prev_id = new_root_id, curr_id = new_root.parent_id(), next_id;

  auto re_wire = [&nodes = nodes](ID prev_id, ID curr_id) -> ID {
    Node &prev = nodes[prev_id], &curr = nodes[curr_id];
    ID next_id = curr.parent_id();

    prev.add_child(curr_id, curr.child_edge(prev_id));
    curr.delete_child(prev_id);
    curr.set_parent(prev_id);

    return next_id;
  };

  new_root.delete_parent();

  while (!nodes[curr_id].is_root()) {
    next_id = re_wire(prev_id, curr_id);

    prev_id = curr_id;
    curr_id = next_id;
  }

  re_wire(prev_id, curr_id);
}

DTree::DTree(Edge* edges, size_t n) {
  std::unordered_set<ID> visited;
  std::queue<std::pair<ID, ID>> bfs;

  auto is_visited = [&visited](ID id) {
    return visited.find(id) != visited.end();
  };

  for (size_t i = 0; i < n; ++i) {
    const auto [u, v, e] = edges[i];

    nodes[u].add_neighbor(v, e);
    nodes[v].add_neighbor(u, e);

    all_edges[e] = edges[i];
  }

  for (const auto& [id, node] : nodes) {
    if (is_visited(id)) continue;

    visited.insert(id);

    for (const auto& [child, _] : node.neighbors()) bfs.push({id, child});

    while (!bfs.empty()) {
      auto [parent_id, curr_id] = bfs.front();
      bfs.pop();

      if (is_visited(curr_id)) continue;

      visited.insert(curr_id);

      connect_tree_edge(parent_id, curr_id);

      for (const auto& [child_id, _] : nodes[curr_id].neighbors())
        if (!is_visited(child_id)) bfs.push({curr_id, child_id});
    }
  }
}

bool DTree::connected(ID node_u_id, ID node_v_id) {
  return root(node_u_id) == root(node_v_id);
}

void DTree::add_edge(Edge edge) {
  const auto [u, v, e] = edge;

  all_edges[e] = edge;

  if (connected(u, v)) return;

  nodes[u].add_neighbor(v, e);
  nodes[v].add_neighbor(u, e);

  re_root(v);
  connect_tree_edge(u, v);
}

void DTree::delete_edge(ID edge_id) {
  auto [u, v, e] = all_edges[edge_id];

  nodes[u].delete_edge(v, e);
  nodes[v].delete_edge(u, e);

  all_edges.erase(e);

  if (tree_edges.find(e) == tree_edges.end()) return;

  tree_edges.erase(e);

  if (nodes[u].is_root()) std::swap(u, v);

  ID u_root = root(u), v_root = v, curr;

  std::unordered_set<ID> visited, rooted_in_v;
  std::queue<ID> bfs;

  auto root_cache = [&](ID id) -> ID {
    if (rooted_in_v.find(id) != rooted_in_v.end()) return v_root;

    ID r = root(id);

    if (r == v_root) rooted_in_v.insert(id);

    return r;
  };

  bfs.push(v);

  while (!bfs.empty()) {
    curr = bfs.front();
    bfs.pop();

    if (visited.find(curr) != visited.end()) continue;

    visited.insert(curr);

    for (const auto& [neighbor, _] : nodes[curr].neighbors()) {
      if (root_cache(neighbor) == u_root) {
        re_root(curr);
        connect_tree_edge(neighbor, curr);
        return;
      }

      bfs.push(neighbor);
    }
  }
}
