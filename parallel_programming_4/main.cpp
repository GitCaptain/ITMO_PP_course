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

void dijkstra(int start){

    dist[start] = 0;

    for(int i = 0; i < n; i++){

        vector<int> min_dist_per_thread(omp_get_max_threads(), INF);
        vector<int> nearest_v_per_thread(omp_get_max_threads(), -1);

        #pragma omp parallel for default(none) shared(n, used, dist, min_dist_per_thread, nearest_v_per_thread)
        for (int v = 0; v < n; v++) {
            int thread_num = omp_get_thread_num();
            if (!used[v] && (nearest_v_per_thread[thread_num] < 0 || min_dist_per_thread[thread_num] > dist[v])) {
                min_dist_per_thread[thread_num] = dist[v];
                nearest_v_per_thread[thread_num] = v;
            }
        }

        int nearest_v = nearest_v_per_thread[0];
        int min_dist = min_dist_per_thread[0];
        for(int j = 0; j < min_dist_per_thread.size(); j++){
            if(min_dist > min_dist_per_thread[j]){
                min_dist = min_dist_per_thread[j];
                nearest_v = nearest_v_per_thread[j];
            }
        }
        used[nearest_v] = 1;

        #pragma omp parallel for default(none) shared(n, used, dist, graph, nearest_v)
        for (int v = 0; v < n; v++) {
            if (graph[nearest_v][v] >= 0) {
                dist[v] = min(dist[v], dist[nearest_v] + graph[nearest_v][v]);
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
    dijkstra(start);
    double end_time = omp_get_wtime();

    writeDist(out);

    cerr << n << " " << end_time - start_time << endl;
    return 0;
}
