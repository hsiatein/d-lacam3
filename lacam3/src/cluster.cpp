#include "../include/cluster.hpp"
std::vector<int> Cluster::occupied;

Cluster::Cluster(const Config& Q,const std::vector<int>& occupied){
    int Qsize=Q.size();
    for(int i=0;i<Qsize;i++){
        parents[i]=i;
        groups[i].insert(i);
    }
    for(int i=0;i<Qsize;i++){
        for(const auto v:Q[i]->two_ring){
            // std::cout<<v->id;
            if(occupied[v->id]==Qsize) continue;
            int agent=occupied[v->id];
            mergeGroup(i,agent);
        }
    }
}

Cluster::Cluster(const Config& Q){
    int Qsize=Q.size();
    for(int i=0;i<Qsize;i++){
        parents[i]=i;
        groups[i].insert(i);
        occupied[Q[i]->id]=i;
    }
    for(int i=0;i<Qsize;i++){
        for(const auto v:Q[i]->two_ring){
            // std::cout<<v->id;
            if(occupied[v->id]==Qsize) continue;
            int agent=occupied[v->id];
            mergeGroup(i,agent);
        }
    }
    for(int i=0;i<Qsize;i++){
        occupied[Q[i]->id]=Qsize;
    }
}

int Cluster::findGroup(int child){
    if(parents[child]==child) return child;
    else{
        int group=findGroup(parents[child]);
        parents[child]=group;
        return group;
    } 
}

bool Cluster::inSameGroup(int a,int b){
    return findGroup(a)==findGroup(b);
}

void Cluster::mergeGroup(int a,int b){
    if(inSameGroup(a,b)) return;
    int min_id=std::min(findGroup(a),findGroup(b));
    int max_id=std::max(findGroup(a),findGroup(b));
    parents[max_id]=findGroup(min_id);
    // std::cout<<"开始合并"<<std::endl;;
    for(auto i=groups[max_id].begin();i!=groups[max_id].end();i++){
        groups[min_id].insert(*i);
    }
    // groups[min_id].merge(groups[max_id]);
    // std::cout<<"开始擦除"<<std::endl;;
    groups.erase(max_id);
}

std::string Cluster::toString(){
    std::ostringstream oss;
    oss << "size: " << groups.size();

    oss << "{\n"; // 开始大括号

    for (const auto& pair : groups) {
        oss << "  " << pair.first << ": { ";
        for (const auto& value : pair.second) {
            oss << value << ", ";
        }
        oss << "},\n"; // 每个 set 后跟逗号
    }

    oss << "}"; // 结束大括号
    return oss.str();
    
}