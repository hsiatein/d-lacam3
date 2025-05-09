#include "../include/hnode.hpp"

#include <random>

int HNode::COUNT = 0;

HNode::HNode(Config _C, DistTable *D, HNode *_parent, int _g, int _h)
    : C(_C),
      parent(_parent),
      neighbor(),
      g(_g),
      h(_h),
      f(g + h),
      priorities(C.size(), 0),
      order(C.size(), 0),
      search_tree(std::queue<LNode *>())
{
  ++COUNT;

  search_tree.push(new LNode());
  const auto N = C.size();

  // update neighbor
  if (parent != nullptr) {
    neighbor.insert(parent);
    parent->neighbor.insert(this);
  }

  // set priorities
  if (parent == nullptr) {
    // initialize
    for (auto i = 0; i < N; ++i) priorities[i] = (float)D->get(i, C[i]) / 10000;
  } else {
    // dynamic priorities, akin to PIBT
    for (auto i = 0; i < N; ++i) {
      if (D->get(i, C[i]) != 0) {
        priorities[i] = parent->priorities[i] + 1;
      } else {
        priorities[i] = parent->priorities[i] - (int)parent->priorities[i];
      }
    }
  }

  // set order
  std::iota(order.begin(), order.end(), 0);
  std::sort(order.begin(), order.end(),
            [&](int i, int j) { return priorities[i] > priorities[j]; });
}

HNode::~HNode()
{
  while (!search_tree.empty()) {
    delete search_tree.front();
    search_tree.pop();
  }
}

void HNode::check_lowlevel_feasibility(LNode *L){
  if(L->who.size()==0) return;
  std::set<int> occupiedVertices;
  std::set<std::array<int,2>> occupiedEdges;
  for (auto d = 0; d < L->depth; ++d){
    if(occupiedVertices.find(L->where[d]->id)==occupiedVertices.end()) occupiedVertices.insert(L->where[d]->id);
    else{
      L->feasibility=false;
      return;
    }
    if(occupiedEdges.find(std::array<int,2> {this->C[L->who[d]]->id,L->where[d]->id})==occupiedEdges.end()) occupiedEdges.insert({L->where[d]->id,this->C[L->who[d]]->id});
    else{
      //runtime_log(3,"约束有交换冲突");
      L->feasibility=false;
      return;
    }
  }
}

LNode *HNode::get_next_lowlevel_node(std::mt19937 &MT)
{
  if (search_tree.empty()) return nullptr;

  auto L = search_tree.front();
  search_tree.pop();
  if (L->depth < C.size()) {
    auto i = order[L->depth];
    auto cands = C[i]->neighbor;
    cands.push_back(C[i]);
    std::shuffle(cands.begin(), cands.end(), MT);  // randomize
    for (auto u : cands) search_tree.push(new LNode(L, i, u));
  }
  return L;
}

LNode *HNode::get_next_lowlevel_node_without_generate()
{
  while(true){
    if (search_tree.empty()) return nullptr;

    auto L = search_tree.front();
    search_tree.pop();
    check_lowlevel_feasibility(L);
    if(!L->feasibility){
      delete L;
      discard_constraint_num++;
      continue;
    }
    return L;
  }
}

void HNode::generate_lowlevel_node(std::mt19937 &MT, LNode* L)
{
  if(!L->feasibility){
    discard_constraint_num++;
    return;
  }
  if (L->depth < C.size()) {
    auto i = order[L->depth];
    auto cands = C[i]->neighbor;
    cands.push_back(C[i]);
    std::shuffle(cands.begin(), cands.end(), MT);  // randomize
    for (auto u : cands) search_tree.push(new LNode(L, i, u));
  }
}

std::ostream &operator<<(std::ostream &os, const HNode *H)
{
  os << "f=" << std::setw(6) << H->f << "\tg=" << std::setw(6) << H->g
     << "\th=" << std::setw(6) << H->h << "\tQ=" << H->C;
  return os;
}

bool CompareHNodePointers::operator()(const HNode *l, const HNode *r) const
{
  const auto N = l->C.size();
  for (auto i = 0; i < N; ++i) {
    if (l->C[i] != r->C[i]) return l->C[i]->id < r->C[i]->id;
  }
  return false;
}
