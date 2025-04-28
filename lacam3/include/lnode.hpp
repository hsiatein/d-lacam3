/*
 * low-level node of LaCAM
 */

#pragma once
#include "graph.hpp"

// low-level search node
struct LNode {
  static int COUNT;

  std::vector<int> who;
  Vertices where;
  bool feasibility;
  const int depth;
  LNode();
  LNode(LNode *parent, int i, Vertex *v);  // who and where
  ~LNode();
};
