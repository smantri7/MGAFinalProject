#include <iostream>
#include <list>
#include <vector>
#include <fstream>
#include <sstream>
#include <set>
#include <bits/stdc++.h>
#include "edge.cpp"

using namespace std;

class Graph {
    vector<Edge> edgelist;
    int num_vertices;
    int max_weight;

    public:
        int stoer_wagner();
        void reduce();
        void setup_graph(const char*, int);
        void remove_node(int);
        bool inMap(int, map<int,int>);
        priority_queue <Edge, vector<Edge>, greater<Edge> > update_heap(priority_queue <Edge, vector<Edge>, greater<Edge> >, int);
        map<int,int> getitems(int);
};

void Graph::setup_graph(const char* filename, int m_weight) {
    ifstream file;
    printf("Starting Setup...\n");
    file.open(filename);
    string line;
    string value;
    string temp;
    num_vertices = 0;

    for(int i = 0; i < 8; i++) {
        if (getline(file, line) && i == 7) {
            int first = 0;
            istringstream iss(line);
            //printf("Parsing: %s\n", line.c_str());
            while (iss >> value) {
                if (first == 2) {
                    std::string::size_type sz;
                    int v = std::stoi (value,&sz);
                    num_vertices = v;
                    first += 1;
                }
                first += 1;
            }
        }
    }
    vector<pair<int,int> > plist;
    vector<Edge> alist;
    while (getline(file, line)) {
        istringstream iss(line);
        int first = 0;
        int source = -1;
        int dest = -1;
        int weight = -1;
        string value;
        //printf("Parsing: %s\n", line.c_str());
        while (iss >> value) {
            if (first == 0) {
                first += 1;
            }
            else if (first == 1) {
                std::string::size_type sz;
                int int_value = std::stoi (value,&sz);
                source = int_value;
                first += 1;
            } else if(first == 2) {
                std::string::size_type sz;
                int int_value = std::stoi (value,&sz);
                dest = int_value;
                first += 1;
            } else {
                std::string::size_type sz;
                int int_value = std::stoi (value,&sz);
                weight = int_value;
                Edge edge;
                edge.setvals(source, dest, weight);

                bool exists = false;
                for(int a = 0; a < plist.size(); a++) {
                    if((plist[a].first == source && plist[a].second == dest)) {
                        exists = true;
                    } else if(plist[a].first == dest && plist[a].second == source) {
                        exists = true;
                    }
                }
                if(!exists) {
                    alist.push_back(edge);
                    plist.push_back(pair<int,int>(source, dest));
                }
            }
            //printf("Parsing: %s\n", value.c_str());
        }
    }
    edgelist = alist;
    printf("Graph Loaded!\n");
    //for(Edge i: edgelist) {
    //    printf("%i ", i.getsrc());
    //    printf("%i ", i.getdest());
    //    printf("%i \n", i.getweight());
    //}
    file.close();
    max_weight = m_weight;
}

int Graph::stoer_wagner() {
    int final_mincut_value = max_weight * num_vertices * num_vertices; //Maximum possible value
    printf("Final Mincut Val: %i\n", final_mincut_value);
    for(int idx = 0; idx < num_vertices - 1; idx++) {
        //printf("Current Graph Size: %i\n", edgelist.size());
        int k = rand() % edgelist.size(); //pick a random idx
        int u = edgelist[k].getsrc(); //Pick a random edge (vertex)
        u = 1;
        //int u = idx;
        //printf("Picked random node: %i\n", u);

        //create a set. Add node to the set
        set <int, greater <int> > s;
        s.insert(u);

        priority_queue <Edge, vector<Edge>, greater<Edge> > heap;

        //heap elements list
        map<int,int> heap_elements;
        map<int,int> most_recent;

        //get edges connected to chosen node, and insert heap
        for(pair<int,int> pa : getitems(u)) {
            Edge e;
            e.setvals(u,pa.first,-pa.second);
            //printf("Pushed edge: %i, %i, %i\n", e.getsrc(), e.getdest(), e.getweight());
            heap.push(e);
            heap_elements.insert(pair<int,int>(e.getdest(),e.getweight()));
            most_recent[e.getdest()] = e.getweight();
        }

        //repeat until only 1 node is missing from S
        for(int k = 0; k < num_vertices - idx - 2; k++) {
            //pop off heap
            if(heap.size() > 0) {
                //check if value is the most recent, ignore older values
                while(true) {
                    Edge edger = heap.top();
                    u = edger.getdest();
                    int current_weight = edger.getweight();
                    heap.pop();
                    if(most_recent.find(u) != most_recent.end() && current_weight == most_recent[u]) {break;}
                }
                //printf("%i %i\n", u, edger.getweight());
                s.insert(u);
                //delete key
                heap_elements.erase(u);
                most_recent.erase(u);
                //printf("Inserted %i \n", u);
                map<int,int> adj = getitems(u);
                for(pair<int,int> p : adj) {

                    int dest = p.first;
                    int edge_weight = p.second;

                    if(s.count(dest) == 0) { //V NOT IN A
                        //printf("Current v: %i\n", dest);
                        int best_weight = 0;
                        map<int, int>::iterator it = heap_elements.find(dest);

                        if (heap_elements.end() != it) {
                            best_weight = heap_elements[dest];
                        }

                        //printf("Best Weight: %i\n", heap_elements[dest]);
                        Edge newEdge;
                        //printf("%i %i %i %i\n", u, dest, best_weight, edge_weight);
                        newEdge.setvals(u, dest, best_weight - edge_weight);

                        //update heap by deleting older nodes of same dest
                        //heap = update_heap(heap, dest);
                        int new_value = best_weight - edge_weight;
                        if(most_recent.find(dest) != most_recent.end()) {
                            int old_value = most_recent[dest];
                            if(new_value < old_value) {
                                most_recent[dest] = best_weight - edge_weight;
                                //now add to heap elements
                                heap.push(newEdge);
                                heap_elements[dest] = newEdge.getweight();
                            }

                        } else {
                            most_recent[dest] = best_weight - edge_weight;
                            //now add to heap elements
                            heap.push(newEdge);
                            heap_elements[dest] = newEdge.getweight();
                        }
                    }
                }
            }
        }

        if(heap.size() == 0) {
            return final_mincut_value;
        }
        //printf("Current Heap Vals: \n");
        //priority_queue <Edge, vector<Edge>, greater<Edge> > temp = heap;
        //for(int q = 0; q < temp.size(); q++) {
        //    Edge cur = temp.top();
        //    temp.pop();
        //    printf("Edge: %i, %i, %i\n", cur.getsrc(), cur.getdest(), cur.getweight());
        //}
        Edge finalEdge = heap.top();
        int v = finalEdge.getdest();
        int current_weight = finalEdge.getweight();
        heap.pop();
        while(true) {
            if(most_recent.find(v) != most_recent.end() && current_weight == most_recent[v]) {break;}
            finalEdge = heap.top();
            v = finalEdge.getdest();
            current_weight = finalEdge.getweight();
            heap.pop();
        }
        most_recent.erase(v);

        int mincut = -1*finalEdge.getweight();
        //printf("Min Cut Val: %i\n", mincut);
        //return 0;
        if(mincut < final_mincut_value) {
            final_mincut_value = mincut;
        }

        //printf("Collapsing: %i %i\n", u,v);

        //Finally, we update the graph
        map<int,int> dora = getitems(v);
        for (pair<int,int> pat : dora) {
            int w = pat.first;
            int weight = pat.second;
            if(w != u) {
                if(!inMap(w, getitems(u))) {
                    Edge newEdge;
                    newEdge.setvals(u, w, weight);
                    //printf("Adding new edge: %i, %i\n", u,w);
                    edgelist.push_back(newEdge);
                } else {
                    for(int z = 0; z < edgelist.size(); z++) {
                        Edge pd = edgelist[z];
                        if((pd.getsrc() == u && pd.getdest() == w) || (pd.getsrc() == w && pd.getdest() == u)) {
                            //printf("Updating weight for: %i,%i,%i + %i\n", u, w, weight, pd.getweight());
                            pd.setweight(weight + pd.getweight());
                            edgelist[z] = pd; //update the value in the actual list
                        }
                    }
                }
            }
        }
        printf("Removed Node\n");
        remove_node(v);

        //current edges
        //printf("Current Edges with weight: \n");
        //for(int m = 0; m < edgelist.size(); m++) {
        //   Edge temp = edgelist[m];
        //    printf("Edge: %i,%i,%i\n", temp.getsrc(), temp.getdest(), temp.getweight());
        //}
    }

    return final_mincut_value;
}

priority_queue <Edge, vector<Edge>, greater<Edge> > Graph::update_heap(priority_queue <Edge, vector<Edge>, greater<Edge> > oldheap, int d) {
    priority_queue <Edge, vector<Edge>, greater<Edge> > newQ;
    while(oldheap.size() > 0) {
        Edge toAdd = oldheap.top();
        oldheap.pop();
        if(toAdd.dest != d) {
            newQ.push(toAdd);
        }
    }
    return newQ;
}

map<int,int> Graph::getitems(int node) {
    map<int,int> edge_info;
    for(Edge e : edgelist) {
        if(e.getsrc() == node) {
            edge_info.insert(pair<int,int>(e.getdest(),e.getweight()));
        } else if(e.getdest() == node) {
            edge_info.insert(pair<int,int>(e.getsrc(), e.getweight()));
        }
    }
    return edge_info;
}

void Graph::remove_node(int v) {
    list<int> toremove = {};
    for(int idx = edgelist.size() - 1; idx >= 0; idx--) {
        Edge e = edgelist[idx];
        if(e.getsrc() == v || e.getdest() == v) {
            //printf("Removing edge: %i, %i\n", v, e.getsrc());
            //printf("Removing edge: %i, %i\n", v, e.getdest());
            toremove.push_back(idx);
        }
    }
    for(int r : toremove) {
        edgelist.erase(edgelist.begin() + r);
    }
}

bool Graph::inMap(int goal, map<int,int> mop) {
    return (mop.find(goal) != mop.end());
}

int main () {
  Graph g;
  g.setup_graph("sample.gr", 100);
  int mincut = g.stoer_wagner();
  return mincut;
}
