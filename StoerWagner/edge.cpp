using namespace std;

class Edge {
    public:
        int src;
        int dest;
        int other;
        int weight;
        int getsrc();
        int getdest();
        int getweight();
        int getother();
        void setother(int);
        void setweight(int);
        void setvals(int,int,int);
};


bool operator>(Edge const & e1, Edge const & e2) {
    // edges ordered by weight
    if (e1.weight == e2.weight) {
        return e1.dest > e2.dest;
    }
    return e1.weight > e2.weight;
}

bool operator<(Edge const & e1, Edge const & e2) {
    // edges ordered by weight
    if(e1.weight == e2.weight) {
        return e1.dest > e2.dest;
    }
    return e1.weight < e2.weight;
}

void Edge::setvals(int source, int destination, int w) {
    src = source;
    dest = destination;
    weight = w;
}

int Edge::getsrc() {return src;}
int Edge::getdest() {return dest;}
int Edge::getweight() {return weight;}
int Edge::getother() {return other;}
void Edge::setweight(int w) {weight = w;}
void Edge::setother(int o) {other = o;}

