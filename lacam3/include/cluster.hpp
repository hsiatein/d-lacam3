#pragma once
#include "graph.hpp"

struct Cluster{
    public:
    std::unordered_map<int, std::set<int>> groups;
    std::unordered_map<int, int> parents;
    static std::vector<int> occupied;
    Cluster(const Config& Q,const std::vector<int>& occupied);
    Cluster(const Config& Q);
    int findGroup(int child);
    bool inSameGroup(int a,int b);
    void mergeGroup(int a,int b);
    std::string toString();
};