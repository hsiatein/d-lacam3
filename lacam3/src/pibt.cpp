#include "../include/pibt.hpp"

int PIBT::TIE_BREAKER=0;
int PIBT::nextID=0;

PIBT::PIBT(const Instance *_ins, DistTable *_D, int seed, bool _flg_swap,
           Scatter *_scatter)
    : ID(nextID),
      ins(_ins),
      MT(std::mt19937(seed)),
      N(ins->N),
      V_size(ins->G->size()),
      D(_D),
      NO_AGENT(N),
      occupied_now(V_size, NO_AGENT),
      occupied_next(V_size, NO_AGENT),
      C_next(N, std::array<Vertex *, 5>()),
      tie_breakers(V_size, 0),
      flg_swap(_flg_swap),
      scatter(_scatter)
{
  nextID++;
}

PIBT::~PIBT() {}

bool PIBT::set_new_config(const Config &Q_from, Config &Q_to,
                          const std::vector<int> &order)
{
  bool success = true;
  std::vector<int> orderOccupy;
  // setup cache & constraints check
  for (auto i = 0; i < N; ++i) {
    // set occupied now
    occupied_now[Q_from[i]->id] = i;

    // set occupied next
    if (Q_to[i] != nullptr) {
      if(cut_constraint_level==0){
        // vertex collision
        if (occupied_next[Q_to[i]->id] != NO_AGENT) {
          success = false;
          runtime_log(3,"约束互相冲突:顶点");
          break;
        }
        // swap collision
        auto j = occupied_now[Q_to[i]->id];
        if (j != NO_AGENT && j != i && Q_to[j] == Q_from[i]) {
          success = false;
          runtime_log(3,"约束互相冲突:交换");
          break;
        }
      }
      occupied_next[Q_to[i]->id] = i;
      if(occupied_now[Q_to[i]->id]!=NO_AGENT){
        orderOccupy.push_back(i);
      }
    }
  }

  int failed_agent=NO_AGENT;

  if (success) {
    for (auto i : orderOccupy) {
      if (Q_to[i] == nullptr && !funcPIBT(i, Q_from, Q_to)) {
        success = false;
        failed_agent = i;
        runtime_log(3,"PIBT生成失败:",i);
        break;
      }
    }
  }


  if (success) {
    for (auto i : order) {
      if (Q_to[i] == nullptr && !funcPIBT(i, Q_from, Q_to)) {
        success = false;
        failed_agent = i;
        runtime_log(3,"PIBT生成失败:",i);
        break;
      }
    }
  }


  // cleanup
  for (auto i = 0; i < N; ++i) {
    occupied_now[Q_from[i]->id] = NO_AGENT;
    if (Q_to[i] != nullptr) occupied_next[Q_to[i]->id] = NO_AGENT;
  }

  return success;
}

bool PIBT::funcPIBT(const int i, const Config &Q_from, Config &Q_to,const int index)
{
  const auto K = Q_from[i]->neighbor.size();
  const int current_idx=Q_from[i]->index;
  // exploit scatter data
  Vertex *prioritized_vertex = nullptr;
  if (scatter != nullptr) {
    auto itr_s = scatter->scatter_data[i].find(Q_from[i]->id);
    if (itr_s != scatter->scatter_data[i].end()) {
      prioritized_vertex = itr_s->second;
    }
  }

  // set C_next
  for (size_t k = 0; k < K; ++k) {
    auto u = Q_from[i]->neighbor[k];
    C_next[i][k] = u;
    double break_tie=0;
    switch (PIBT::TIE_BREAKER)
    {
    case 1:
      break_tie=(occupied_now[u->id]==NO_AGENT)?0.0:1.0;
      break;
    case 2:
      break_tie=(occupied_next[u->id]==NO_AGENT)?0.0:1.0;
      break;
    case 3:
      break_tie=(occupied_now[u->id]==NO_AGENT&&occupied_next[u->id]==NO_AGENT)?0.0:1.0;
      break;
    case 4:
      break_tie=(occupied_now[u->id]==NO_AGENT&&occupied_next[u->id]==NO_AGENT&&current_idx-index!=u->index-current_idx)?0.0:1.0;
      break;
    }
    tie_breakers[u->id] = (get_random_float(MT)+break_tie)*0.5;
  }
  C_next[i][K] = Q_from[i];

  // sort, note: K + 1 is sufficient
  std::sort(C_next[i].begin(), C_next[i].begin() + K + 1,
            [&](Vertex *const v, Vertex *const u) {
              if (v == prioritized_vertex) return true;
              if (u == prioritized_vertex) return false;
              return D->get(i, v) + tie_breakers[v->id] <
                     D->get(i, u) + tie_breakers[u->id];
            });

  // emulate swap
  auto swap_agent = NO_AGENT;
  if (flg_swap) {
    swap_agent = is_swap_required_and_possible(i, Q_from, Q_to);
    if (swap_agent != NO_AGENT) {
      // reverse vertex scoring
      std::reverse(C_next[i].begin(), C_next[i].begin() + K + 1);
    }
  }

  auto swap_operation = [&]() {
    if (swap_agent != NO_AGENT &&                 // swap_agent exists
        Q_to[swap_agent] == nullptr &&            // not decided
        occupied_next[Q_from[i]->id] == NO_AGENT  // free
    ) {
      // pull swap_agent
      occupied_next[Q_from[i]->id] = swap_agent;
      Q_to[swap_agent] = Q_from[i];
    }
  };

  std::string log_content="";
  bool L_discard=(index==-1);
  // main loop
  for (size_t k = 0; k < K + 1; ++k) {
    auto u = C_next[i][k];

    // avoid vertex conflicts
    if (occupied_next[u->id] != NO_AGENT)
    {
      if(cut_constraint_level<2 || !L_discard) continue;
      log_content=log_content+"vertex conflicts  "; //测试用
      if(find(L->who.begin(),L->who.end(),occupied_next[u->id])!=L->who.end()){
        L_discard=L_discard && true;
        log_content=log_content+"<C>  "; //测试用
      }
      else L_discard=false;
      continue;
    }

    const auto j = occupied_now[u->id];

    // avoid swap conflicts with constraints
    if (j != NO_AGENT && Q_to[j] == Q_from[i])
    {
      if(cut_constraint_level<2 || !L_discard) continue;
      log_content=log_content+"swap conflicts  "; //测试用
      if(find(L->who.begin(),L->who.end(),j)!=L->who.end()){
        L_discard=L_discard && true;
        log_content=log_content+"<C>  "; //测试用
      }
      else L_discard=false;
      continue;
    }
    // reserve next location
    occupied_next[u->id] = i;
    Q_to[i] = u;

    // priority inheritance
    if (j != NO_AGENT && u != Q_from[i] && Q_to[j] == nullptr &&
        !funcPIBT(j, Q_from, Q_to, current_idx))
      {
        if(cut_constraint_level<2) continue;
        log_content=log_content+"PIBT fail  "; //测试用
        continue;
      }

    // success to plan next one step
    if (flg_swap && k == 0) swap_operation();
    L_discard=false;
    return true;
  }

  // failed to secure node
  occupied_next[Q_from[i]->id] = i;
  Q_to[i] = Q_from[i];
  if(cut_constraint_level==2 && index==-1) runtime_log(3,log_content); //测试用
  if(cut_constraint_level==2 && L_discard) L->feasibility.store(false);
  return false;
}

int PIBT::is_swap_required_and_possible(const int i, const Config &Q_from,
                                        Config &Q_to)
{
  // agent-j occupying the desired vertex for agent-i
  const auto j = occupied_now[C_next[i][0]->id];
  if (j != NO_AGENT && j != i &&  // j exists
      Q_to[j] == nullptr &&       // j does not decide next location
      is_swap_required(i, j, Q_from[i], Q_from[j]) &&  // swap required
      is_swap_possible(Q_from[j], Q_from[i])           // swap possible
  ) {
    return j;
  }

  // for clear operation, c.f., push & swap
  if (C_next[i][0] != Q_from[i]) {
    for (auto u : Q_from[i]->neighbor) {
      const auto k = occupied_now[u->id];
      if (k != NO_AGENT &&              // k exists
          C_next[i][0] != Q_from[k] &&  // this is for clear operation
          is_swap_required(k, i, Q_from[i],
                           C_next[i][0]) &&  // emulating from one step ahead
          is_swap_possible(C_next[i][0], Q_from[i])) {
        return k;
      }
    }
  }
  return NO_AGENT;
}

bool PIBT::is_swap_required(const int pusher, const int puller,
                            Vertex *v_pusher_origin, Vertex *v_puller_origin)
{
  auto v_pusher = v_pusher_origin;
  auto v_puller = v_puller_origin;
  Vertex *tmp = nullptr;
  while (D->get(pusher, v_puller) < D->get(pusher, v_pusher)) {
    auto n = v_puller->neighbor.size();
    // remove agents who need not to move
    for (auto u : v_puller->neighbor) {
      const auto i = occupied_now[u->id];
      if (u == v_pusher ||
          (u->neighbor.size() == 1 && i != NO_AGENT && ins->goals[i] == u)) {
        --n;
      } else {
        tmp = u;
      }
    }
    if (n >= 2) return false;  // able to swap at v_l
    if (n <= 0) break;
    v_pusher = v_puller;
    v_puller = tmp;
  }

  return (D->get(puller, v_pusher) < D->get(puller, v_puller)) &&
         (D->get(pusher, v_pusher) == 0 ||
          D->get(pusher, v_puller) < D->get(pusher, v_pusher));
}

bool PIBT::is_swap_possible(Vertex *v_pusher_origin, Vertex *v_puller_origin)
{
  // simulate pull
  auto v_pusher = v_pusher_origin;
  auto v_puller = v_puller_origin;
  Vertex *tmp = nullptr;
  while (v_puller != v_pusher_origin) {  // avoid loop
    auto n = v_puller->neighbor.size();
    for (auto u : v_puller->neighbor) {
      const auto i = occupied_now[u->id];
      if (u == v_pusher ||
          (u->neighbor.size() == 1 && i != NO_AGENT && ins->goals[i] == u)) {
        --n;
      } else {
        tmp = u;
      }
    }
    if (n >= 2) return true;  // able to swap at v_next
    if (n <= 0) return false;
    v_pusher = v_puller;
    v_puller = tmp;
  }
  return false;
}
