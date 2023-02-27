#include <iostream>
#include <vector>

#include "d_tree.h"

int main() {
  std::vector<Edge> edges = {{1, 2, 1}, {1, 3, 2}, {1, 5, 3},
                             {2, 4, 4}, {3, 4, 5}, {3, 5, 6},
                             {6, 7, 7}, {6, 8, 8}, {7, 9, 9}};

  DTree d_tree(edges.data(), edges.size());

  std::cout << d_tree.connected(1, 4) << std::endl;
  std::cout << d_tree.connected(1, 5) << std::endl;
  std::cout << d_tree.connected(6, 9) << std::endl;
  std::cout << d_tree.connected(2, 8) << std::endl;

  std::cout << "-----" << std::endl;

  std::cout << d_tree.connected(1, 6) << std::endl;
  d_tree.add_edge({1, 6, 10});
  std::cout << d_tree.connected(1, 6) << std::endl;
  std::cout << d_tree.connected(3, 7) << std::endl;

  std::cout << "-----" << std::endl;

  std::cout << d_tree.connected(1, 6) << std::endl;
  d_tree.delete_edge(10);
  std::cout << d_tree.connected(1, 6) << std::endl;

  std::cout << "-----" << std::endl;

  std::cout << d_tree.connected(3, 4) << std::endl;
  d_tree.delete_edge(5);
  std::cout << d_tree.connected(3, 4) << std::endl;
}
