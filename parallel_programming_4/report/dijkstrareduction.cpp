#include <iostream>
#include <vector>
#include <omp.h>
#include <string>
#include <fstream>
using namespace std;

#define INF INT32_MAX

vector<vector<int>> graph;
vector<int> dist;
vector<int> used;
int n;

struct node {
    int distance;
    int vertex;
};

void dijkstraReduction(int start){
    dist[start] = 0;

#pragma omp declare reduction(min : node: \
	omp_out.distance = omp_in.distance > omp_out.distance ? omp_out.distance:omp_in.distance,\
	omp_out.vertex   = omp_in.distance > omp_out.distance ? omp_out.vertex:omp_in.vertex) \
	initializer(omp_priv={INF,0})

    for(int i = 0; i < n; i++){

        node nearest = {INF, start};

        #pragma omp parallel for shared(dist, used, n) reduction(min: nearest)
        for (int v = 0; v < n; v++) {
            if (!used[v] && nearest.distance > dist[v]) {
                nearest.distance = dist[v];
                nearest.vertex = v;
            }
        }

        used[nearest.vertex] = 1;

        #pragma omp parallel for shared(graph, dist, nearest, n)
        for (int v = 0; v < n; v++) {
            if (graph[nearest.vertex][v] >= 0) {
                dist[v] = min(dist[v], dist[nearest.vertex] + graph[nearest.vertex][v]);
            }
        }
    }
}



void readGraph(const string &in_path){
    ifstream in(in_path);
    in >> n;
    graph.resize(n, vector<int>(n));
    dist.resize(n, INF);
    used.resize(n, 0);
    for(int i = 0; i < n; i++){
        for(int j = 0; j < n; j++){
            in >> graph[i][j];
        }
    }
}

void writeDist(const string &out_path){
    ofstream out(out_path);
    for(int i = 0; i < n; i++){
        if(dist[i] == INF){
            out << "INF ";
        }
        else{
            out << dist[i] << " ";
        }
    }
}

int main(int argc, char **argv) {

    if(argc < 4){
        cout << "usage: path1 start path2\n"
                "path1 - path to graph\n"
                "start - first vertex for dijkstra\n"
                "path2 - path to output distance matrix\n";
        return 0;
    }

    string in(argv[1]);
    string out(argv[3]);
    int start = atoi(argv[2]);

    readGraph(in);
    double start_time = omp_get_wtime();
    dijkstraReduction(start);
    double end_time = omp_get_wtime();
    writeDist(out);
    cerr << n << " " << end_time - start_time << endl;
    return 0;
}

