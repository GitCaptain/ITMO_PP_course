#include <iostream>
#include <omp.h>
#include <vector>
#include <string>
#include <fstream>
#include <sys/time.h>

using namespace std;

typedef unsigned long long ull;
typedef long long ll;

unsigned long long get_time(){
    timeval t;
    gettimeofday(&t, NULL);
    return t.tv_sec*1e6 + t.tv_usec;
}

void read_matrix(const string &path, vector<vector<ll>> &matrix){
    ifstream input(path);
    int n, m;
    input >> n >> m;
    matrix.resize(n, vector<ll>(m));
    for(int i = 0; i < n; ++i)
        for(int j = 0; j < m; ++j)
            input >> matrix[i][j];
}

void write_matrix(const string &path, const vector<vector<ll>> &matrix){
    ofstream output(path);
    output << matrix.size() << " " << matrix[0].size() << endl;
    for(auto &row: matrix) {
        for (auto e: row) {
            output << e << " ";
        }
        output << endl;
    }
}

string strip(const string &s){
    int l = 0, r = s.length() - 1;
    while(s[l] == ' ') l++;
    while(s[r] == ' ') r--;
    return s.substr(l, r-l+1);
}

int main(int argc, char **argv) {

    vector<vector<ll>> matrixes[3];

    vector<string> pathes = {argv[1], argv[2], argv[3]};

    for (auto &s: pathes) {
        s = strip(s);
    }

    for (int i = 0; i < 2; ++i) {
        read_matrix(pathes[i], matrixes[i]);
    }
    string schedule = argv[4];

    int chunk_size = atoi(argv[5]);
    omp_sched_t sched_type;

    if (schedule == "dynamic") {
        sched_type = omp_sched_dynamic;
    } else if (schedule == "static") {
        sched_type = omp_sched_static;
    } else {
        sched_type = omp_sched_guided;
    }
    omp_set_schedule(sched_type, chunk_size);

    auto &a = matrixes[0], &b = matrixes[1], &c = matrixes[2];

    ull i, j, k;
    int res_rows = a.size(), res_cols = b[0].size();
    ull operations_cnt = a.size() * (1ull) * a[0].size() * b[0].size();

    c.resize(res_rows, vector<ll>(res_cols, 0));
    vector<vector<ll>> tempRes;
    auto start = get_time();

#pragma omp parallel default(none) shared(operations_cnt, a, b, c, res_rows, res_cols, cout) private(i, j, k, tempRes)
{
    tempRes.resize(res_rows, vector<ll>(res_cols, 0));
    #pragma omp for schedule(runtime) nowait
    for (ull iteration = 0; iteration < operations_cnt; iteration++) {
        k = iteration % a[0].size();
        j = (iteration / a[0].size()) % b[0].size();
        i = iteration / a[0].size() / b[0].size();
        tempRes[i][j] += a[i][k] * b[k][j];
    }

    #pragma omp critical
    {
        for (int i = 0; i < res_rows; ++i) {
            for (int j = 0; j < res_cols; ++j) {
                c[i][j] += tempRes[i][j];
            }
        }
    }
}
    auto finish = get_time();

    if(pathes[2] != "-") {
        write_matrix(pathes[2], c);
    }

    auto diff = finish - start;
    int _1k = 1000;
//    cout << "calculation time: " << diff / (_1k*_1k) << " sec, "
//                                 << (diff % (_1k*_1k)) / _1k << " millisec, "
//                                 << (diff % _1k) << " microsec;\n";
    cout << diff << endl;
    return 0;
}

//not paralleled
//calculation time: 154 sec, 168 millisec, 106 microsec;
//154168106