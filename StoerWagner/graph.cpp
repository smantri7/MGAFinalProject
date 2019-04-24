#include <fstream>
#include <set>
#include <bits/stdc++.h>
#include <algorithm>
#include "edge.cpp"
#include <sys/timeb.h>
//opendl1
//#include <omp.h>

using namespace std;

int getms() {
    timeb current;
    ftime(&current);
    int ms = current.millitm + (current.time & 0xfffff) * 1000;
    return ms;
}

class Graph {
    vector<int> edgelist;
    map<int, vector<Edge> > edgemap;
    int num_vertices;
    int max_weight;

    public:
        int stoer_wagner(int);
        void reduce();
        void setup_graph(const char*, int);
        void remove_node(int);
        bool inMap(int, map<int,int>);
        bool inVector(int, vector<Edge>);
        vector<Edge> getitems(int);
};

void Graph::setup_graph(const char* filename, int m_weight) {
    ifstream file;
    //printf("Starting Setup...\n");
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
    vector<int> alist;
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
                if (int_value != 0) {

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
                        plist.push_back(pair<int,int>(source, dest));
                        bool srcinmap = (edgemap.find(source) != edgemap.end());
                        bool destinmap = (edgemap.find(dest) != edgemap.end());
                        if (!srcinmap) {
                            vector<Edge> temp;
                            edge.setother(dest);
                            temp.push_back(edge);
                            edgemap[source] = temp;
                        } else {
                            edge.setother(dest);
                            edgemap[source].push_back(edge);
                        }
                        if (!destinmap) {
                            vector<Edge> tempd;
                            edge.setother(source);
                            tempd.push_back(edge);
                            edgemap[dest] = tempd;
                        } else {
                            edge.setother(source);
                            edgemap[dest].push_back(edge);
                        }
                    }
                }
            }
            //printf("Parsing: %s\n", value.c_str());
        }
    }
    for (int l = 1; l <= num_vertices; l++) {
        edgelist.push_back(l);
    }
    //printf("Graph Loaded!\n");
    //for(Edge i: edgemap[2]) {
    //    printf("%i ", i.getother());
    //    printf("%i ", i.getdest());
    //    printf("%i \n", i.getweight());
    //}
    file.close();
    max_weight = m_weight;
}

int Graph::stoer_wagner(int give_partition) {
    //save edgemap for rebuild
    map<int, vector<Edge> > rebuild_graph = edgemap;
    int final_mincut_value = 2147483646; //Maximum possible value
    int best_phase = -1;
    vector<pair<int,int> > contractions;

    for(int idx = 0; idx < num_vertices - 1; idx++) {
        //printf("Bottleneck 1...\n");
        //int t0 = getms();
        int k = rand() % edgelist.size(); //pick a random idx i
        //int k = 0;
        int u = edgelist[k]; //Pick a random (vertex)
        //printf("Picked Vertex: %i %i\n", u, idx);
        //create a set. Add node to the set
        //u = 1;
        set <int, greater <int> > s;

        s.insert(u);

        priority_queue <Edge, vector<Edge>, greater<Edge> > heap;

        //heap elements list
        map<int,int> most_recent;

        //get edges connected to chosen node, and insert heap
        for(Edge e : getitems(u)) {
            e.setweight(e.getweight() * -1);
            if(e.getdest() == u) {
                most_recent.insert(pair<int,int>(e.getsrc(),e.getweight()));
                e.dest = e.getsrc();
                e.src = u;
                heap.push(e);
            } else {
            most_recent.insert(pair<int,int>(e.getdest(),e.getweight()));
            heap.push(e);
            }
        }

        //initial heap
        //priority_queue <Edge, vector<Edge>, greater<Edge> > loop;
        
        /**
        printf("Current Heap!\n");
        loop = heap;
        while(loop.size() > 0) {
            Edge pep = loop.top();
            printf("%i|", pep.getdest());
            loop.pop();
        }
        printf("\n");
        */

        //printf("Heap Size: %i\n", heap.size());
        //repeat until only 1 node is missing from S
        //printf("Bottleneck 2...\n");
        //int t0 = time(NULL);
        //printf(">>>%i\n", idx);
        //printf("\n");
        for(int k = 0; k < num_vertices - idx - 2; k++) {
            //pop off heap
            if(heap.size() > 0) {
                 //clean the heap
                //printf("Elements added: %i\n", add);
                //printf("Before clean: %i\n", heap.size());
                
                Edge edger = heap.top();
                while(heap.size() > 0) {
                    edger = heap.top();
                    int current_node = edger.getdest();
                    int current_weight = edger.getweight();
                    heap.pop();
                    //printf("Popped Off: %i, %i\n", current_node, current_weight);
                    if (most_recent.find(current_node) != most_recent.end() && most_recent[current_node] == current_weight) {
                        break;
                    } //otherwise we delete it

                }

                u = edger.getdest();
                //printf("Popped: %i\n", u);
                //if (u == 46) {
                    //printf("Current Heap!\n");
                    //loop = heap;
                //    map<int, int>::iterator itr;
                //    for(itr = most_recent.begin(); itr != most_recent.end(); ++itr) {
                //        printf("%i, %i| ", itr->first, itr->second);
                //        loop.pop();
                //    }           
                    //printf("\n"); 
                //}
                //printf("Heap Size: %i\n", heap.size());
                s.insert(u);

                //printf("Inserting: %i\n", u);
                //delete key
                map<int,int>::iterator iter = most_recent.find(u);
                if (iter != most_recent.end()) {
                    most_recent.erase(iter); //we "erased" the value now
                }
                vector<Edge> adj = getitems(u);
                for(Edge p : adj) {
                    int dest = p.getdest();
                    if(dest == u) {
                        dest = p.getsrc();
                    }

                    int edge_weight = p.getweight();

                    if(s.count(dest) == 0) { //V NOT IN A

                        int best_weight = 0;


                        //update heap by deleting older nodes of same dest
                        //heap = update_heap(heap, dest);
                        if(most_recent.find(dest) != most_recent.end()) { //another pair exists!
                            best_weight = most_recent[dest];
                            Edge newEdge;
                            newEdge.setvals(u, dest, best_weight - edge_weight);
                            int new_value = best_weight - edge_weight;
                            int old_value = most_recent[dest];
                            if(new_value < old_value) {
                                //if (dest == 117) {
                                //   printf("Added new vs old: %i %i\n", new_value, old_value); 
                                //}
                                //
                                most_recent[dest] = new_value;
                                //now add to heap elements
                                heap.push(newEdge);
                            }

                        } else {
                            Edge newEdge;
                            newEdge.setvals(u, dest, best_weight - edge_weight);
                            int new_value = best_weight - edge_weight;
                            most_recent[dest] = new_value;
                            //printf("Added new edge: %i %i\n", dest, new_value);
                            //now add to heap elements
                            heap.push(newEdge);
                        }

                        //printf("Heap SizeA: %i\n", heap.size());


                    }
                }
                //printf("Elements added: %i\n", add);
                /*
                //clean the heap
                priority_queue <Edge, vector<Edge>, greater<Edge> > newheap;
                
                //printf("Before clean: %i\n", heap.size());
                while(heap.size() > 0) {
                    Edge edger = heap.top();
                    int current_node = edger.getdest();
                    int current_weight = edger.getweight();
                    heap.pop();

                    if (most_recent[current_node] == current_weight) {
                        newheap.push(edger);
                    } //otherwise we delete it
                }
                heap = newheap;
                */

                /*
                printf("Current Heap!\n");
                loop = heap;
                while(loop.size() > 0) {
                    Edge pep = loop.top();
                    printf("%i |", pep.getdest());
                    loop.pop();
                }
                printf("\n");
                */
            }

        }
        //int t1 = getms();
        //printf ("time elapsed Heap Modification = %d milisecs\n", t1 - t0);

        //t0 = getms();

        if(heap.size() == 0) {
            return final_mincut_value;
        }

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

        map<int,int>::iterator iter = most_recent.find(v);
        if (iter != most_recent.end()) {
            most_recent.erase(iter); //we "erased" the value now
        }

        //MINCUT CHECKING
        int mincut = -1*finalEdge.getweight();
        //printf("Min Cut Val: %i\n", mincut);
        //return 0;
        if(mincut < final_mincut_value) {
            final_mincut_value = mincut;
            //printf("Updated mincut val! :%i\n", final_mincut_value);
            best_phase = idx;
            //return final_mincut_value;
        }
        pair<int,int> g_pair = make_pair(finalEdge.getsrc(), finalEdge.getdest());
        contractions.push_back(g_pair);
        //t1 = getms();
        //printf ("time elapsed Final Edge = %d secs\n", t1 - t0);
        //printf("Collapsing: %i %i\n", u,v);
        //printf("Bottleneck 3...\n");
        //Finally, we update the graph
        //t0 = getms();
        vector<Edge> dora = getitems(v);
        for (Edge pat : dora) {
            int w = pat.getother();
            int weight = pat.getweight();
            if(w != u) {
                if(!inVector(w, getitems(u))) {
                    Edge newEdge;
                    newEdge.setvals(u, w, weight);
                    newEdge.setother(u);
                    edgemap[w].push_back(newEdge);
                    newEdge.setother(w);
                    edgemap[u].push_back(newEdge);

                } else {

                    for(int z = 0; z < edgemap[u].size(); z++) {
                        if(edgemap[u][z].other == w) {
                            edgemap[u][z].setweight(weight + edgemap[u][z].getweight());
                        }
                    }

                    for(int m = 0; m < edgemap[w].size(); m++) {
                        if(edgemap[w][m].other == u) {
                            edgemap[w][m].setweight(weight + edgemap[w][m].getweight());
                        }
                    }
                }
            }
        }

        remove_node(v);
        //t1 = getms();
        //printf ("time elapsed Removal Phase = %d secs\n", t1 - t0);
    }


    int x = contractions[best_phase].second;

    if (give_partition == 1) {
        set<int> reachable;
        set<int> allnodes;
        for(int b = 1; b < num_vertices + 1; b++) {
            if(b != x) {
                allnodes.insert(b);
            }
        }

        //now, for the hard part.
        stack <int> futureNodes;
        futureNodes.push(x);
        while(futureNodes.size() != 0) {
            vector<Edge> hook = rebuild_graph[futureNodes.top()];
            futureNodes.pop();
            for(Edge ede : hook) {
                if(reachable.find(ede.getdest()) == reachable.end()) {
                    futureNodes.push(ede.getdest());
                    reachable.insert(ede.getdest());
                }
            }
        }
        std::set<int> result;
        std::set_difference(allnodes.begin(), allnodes.end(), reachable.begin(), reachable.end(),std::inserter(result, result.end()));

        if(result.size() == 0) {
            result.insert(x);
            reachable.erase(x);
        }

        for (int a : reachable) {
            printf("%i ", a);
        }
        printf(" | ");
        for (int b : result) {
            printf("%i ", b);
        }
        printf("\n");
    }
    return final_mincut_value;
}

vector<Edge> Graph::getitems(int node) {
    return edgemap[node];
}

void Graph::remove_node(int v) {
    list<int> toremove = {};
    edgelist.erase(std::remove(edgelist.begin(), edgelist.end(), v), edgelist.end());

    for(int j = 0; j < edgemap[v].size(); j++) {
        Edge e = edgemap[v][j];
        int r = e.getother();
        vector<Edge> temp = edgemap[r];

        for(int i = edgemap[r].size() - 1; i >= 0; i--) {
            if(edgemap[r][i].getother() == v) {
                temp.erase(temp.begin() + i);
            }
        }

        edgemap[r] = temp;
    }
}

bool Graph::inMap(int goal, map<int,int> mop) {
    return (mop.find(goal) != mop.end());
}

bool Graph::inVector(int goal, vector<Edge> elist) {
    for(Edge rip : elist) {
        if(goal == rip.getother()) {
            return true;
        }
    }
    return false;
}

int main () {

    char* arr[12] = {"data/2_5.txt", "data/2_6.txt", "data/2_7.txt",
                "data/2_8.txt", "data/2_9.txt", "data/2_10.txt",
                "data/2_11.txt", "data/2_12.txt", "data/2_13.txt",
                "data/2_14.txt", "data/2_15.txt", "data/2_16.txt"};

    for(int x = 0; x < 6; x++) {
        Graph g;
        g.setup_graph(arr[x], 10000);
        int t0 = getms();
        
        int mincut = g.stoer_wagner(1);
        int t1 = getms();
        printf("Graph size: %s\n", arr[x]);
        printf("Mincut: %i\n", mincut);
        printf ("time elapsed = %d millisecs\n", t1 - t0);
    }
    return 0;
  /*
  #pragma omp parallel
    {
    printf("Hello World");
    printf("Hello World from thread = %d of %d\n", omp_get_thread_num(), omp_get_num_threads());
    }
    return 0;
*/
}
