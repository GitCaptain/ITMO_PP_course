#include <iostream>
#include <omp.h>
#include <vector>
#include <string>
#include <fstream>
#include <sys/time.h>

using namespace std;

typedef unsigned long long ull;
typedef long long ll;
typedef vector<vector<ll>> Matrix;

unsigned long long get_time(){
    timeval t;
    gettimeofday(&t, NULL);
    return t.tv_sec*1e6 + t.tv_usec;
}

#define MIN_SIZE 512

pair<int, int> read_matrix(const string &path, Matrix &matrix){
    ifstream input(path);
    int n, m;
    input >> n >> m;
    matrix.resize(n, vector<ll>(m, 0));
    for(int i = 0; i < n; ++i)
        for(int j = 0; j < m; ++j)
            input >> matrix[i][j];
    return {n, m};
}


Matrix getResizedMatrix(Matrix &matrix, int new_size){
    Matrix res(new_size, vector<ll>(new_size, 0));
    for(int i = 0; i < matrix.size(); i++){
        for(int j = 0; j < matrix[i].size(); j++){
            res[i][j] = matrix[i][j];
        }
    }
    return res;
}


void write_matrix(const string &path, const Matrix &matrix, pair<int, int> sizes){
    ofstream output(path);
    output << sizes.first << " " << sizes.second << endl;
    for(int i = 0; i < sizes.first; i++) {
        for (int j = 0; j < sizes.second; j++) {
            output << matrix[i][j] << " ";
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


void MatrixAdd(const Matrix &a, const Matrix &b, Matrix &c){
    int n = a.size();
    for(int i = 0; i < n; i++){
        for(int j = 0; j < n; j++){
            c[i][j] = a[i][j] + b[i][j];
        }
    }
}


void MatrixSubstract(const Matrix &a, const Matrix &b, Matrix &c) {
    int n = a.size();
    for(int i = 0; i < n; i++){
        for(int j = 0; j < n; j++){
            c[i][j] = a[i][j] - b[i][j];
        }
    }
}


void matrix_cube_multiply(const Matrix &a, const Matrix &b, Matrix &c){
    ull operations_cnt = a.size() * (1ull) * a[0].size() * b[0].size();
    for (ull iteration = 0, i, j, k; iteration < operations_cnt; iteration++) {
        k = iteration % a[0].size();
        j = (iteration / a[0].size()) % b[0].size();
        i = iteration / a[0].size() / b[0].size();
        c[i][j] += a[i][k] * b[k][j];
    }
}


void matrix_strassen_multiply(const Matrix &a, const Matrix &b, Matrix &c){

    int size = c.size();


    if (size <= MIN_SIZE){
        matrix_cube_multiply(a, b, c);
        return;
    }

    unsigned long half_size = size / 2;
    Matrix P1(half_size, vector<ll>(half_size));
    Matrix P2(half_size, vector<ll>(half_size));
    Matrix P3(half_size, vector<ll>(half_size));
    Matrix P4(half_size, vector<ll>(half_size));
    Matrix P5(half_size, vector<ll>(half_size));
    Matrix P6(half_size, vector<ll>(half_size));
    Matrix P7(half_size, vector<ll>(half_size));

    Matrix A11(half_size, vector<ll>(half_size));
    Matrix A12(half_size, vector<ll>(half_size));
    Matrix A21(half_size, vector<ll>(half_size));
    Matrix A22(half_size, vector<ll>(half_size));

    Matrix B11(half_size, vector<ll>(half_size));
    Matrix B12(half_size, vector<ll>(half_size));
    Matrix B21(half_size, vector<ll>(half_size));
    Matrix B22(half_size, vector<ll>(half_size));


    int operations_cnt = half_size * half_size;
    #pragma omp parallel for default(none)\
    shared(operations_cnt, half_size, a, b,\
            A11, A12, A21, A22,\
            B11, B12, B21, B22)\
            schedule(runtime)
    for(int op = 0; op < operations_cnt; op++){
        int i = op / half_size;
        int j = op % half_size;

        A11[i][j] = a[i][j];
        A12[i][j] = a[i][j+half_size];
        A21[i][j] = a[i + half_size][j];
        A22[i][j] = a[i+half_size][j+half_size];

        B11[i][j] = b[i][j];
        B12[i][j] = b[i][j+half_size];
        B21[i][j] = b[i + half_size][j];
        B22[i][j] = b[i+half_size][j+half_size];
    }


    #pragma omp parallel sections default(none) \
    shared(half_size, \
    P1, P2, P3, P4, P5, P6, P7,\
    A11, A12, A21, A22,\
    B11, B12, B21, B22)
    {

        #pragma omp section
        {
            // P1 = (A11+A22)(B11+B22)
            Matrix sum1(half_size, vector<ll>(half_size));
            Matrix sum2(half_size, vector<ll>(half_size));
            MatrixAdd(A11, A22, sum1);
            MatrixAdd(B11, B22, sum2);
            matrix_strassen_multiply(sum1, sum2, P1);
        }

        #pragma omp section
        {
            // P2 = (A21+A22)B11
            Matrix sum(half_size, vector<ll>(half_size));
            MatrixAdd(A21, A22, sum);
            matrix_strassen_multiply(sum, B11, P2);
        }

        #pragma omp section
        {
            //P3 = A11(B12-B22)
            Matrix diff(half_size, vector<ll>(half_size));
            MatrixSubstract(B12, B22, diff);
            matrix_strassen_multiply(A11, diff, P3);
        }

        #pragma omp section
        {
            //P4 = A22(B21-B11)
            Matrix diff(half_size, vector<ll>(half_size));
            MatrixSubstract(B21, B11, diff);
            matrix_strassen_multiply(A22, diff, P4);
        }

        #pragma omp section
        {
            //P5 = (A11+A12)B22
            Matrix sum(half_size, vector<ll>(half_size));
            MatrixAdd(A11, A12, sum);
            matrix_strassen_multiply(sum, B22, P5);
        }

        #pragma omp section
        {
            //P6 = (A21-A11)(B11+B12)
            Matrix diff(half_size, vector<ll>(half_size));
            Matrix sum(half_size, vector<ll>(half_size));
            MatrixSubstract(A21, A11, diff);
            MatrixAdd(B11, B12, sum);
            matrix_strassen_multiply(diff, sum, P6);
        }

        #pragma omp section
        {
            //P7 = (A12-A22)(B21+B22)
            Matrix diff(half_size, vector<ll>(half_size));
            Matrix sum(half_size, vector<ll>(half_size));
            MatrixSubstract(A12, A22, diff);
            MatrixAdd(B21, B22, sum);
            matrix_strassen_multiply(diff, sum, P7);
        }
    }

    Matrix C11(half_size, vector<ll>(half_size));
    Matrix C12(half_size, vector<ll>(half_size));
    Matrix C21(half_size, vector<ll>(half_size));
    Matrix C22(half_size, vector<ll>(half_size));

    #pragma omp parallel sections default(none) shared(P1, P2, P3, P4, P5, P6, P7, C11, C12, C21, C22)
    {

        #pragma omp section
        {
            // C11 = P1 + P4 - P5 + P7
            MatrixAdd(P1, P4, C11);
            MatrixSubstract(C11, P5, C11);
            MatrixAdd(C11, P7, C11);
        }

        #pragma omp section
        // C12 = P3+P5
        MatrixAdd(P3, P5, C12);

        #pragma omp section
        // C21 = P2+P4
        MatrixAdd(P2, P4, C21);

        #pragma omp section
        {
            // C22 = P1-P2+P3+P6
            MatrixSubstract(P1, P2,  C22);
            MatrixAdd(C22, P3, C22);
            MatrixAdd(C22, P6, C22);
        }
    }

    #pragma omp parallel for default(none) shared(c, C11, C12, C21, C22, operations_cnt, half_size)\
    schedule(runtime)
    for(int op = 0; op < operations_cnt; op++)  {
        int i = op / half_size;
        int j = op % half_size;
        c[i][j] = C11[i][j];
        c[i][j + half_size] = C12[i][j];
        c[i + half_size][j] = C21[i][j];
        c[i + half_size][j+half_size] = C22[i][j];
    }

}


int main(int argc, char **argv) {

    Matrix matrixes[3];

    vector<string> pathes = {argv[1], argv[2], argv[3]};

    for (auto &s: pathes) {
        s = strip(s);
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

    pair<int, int> sizes[3];
    for (int i = 0; i < 2; ++i) {
        sizes[i] = read_matrix(pathes[i], matrixes[i]);
//        cout << sizes[i].first << " " << sizes[i].second << endl;
    }
    sizes[2] = {sizes[0].first, sizes[1].second};

    int max_size = max(max(sizes[0].first, sizes[0].second), max(sizes[1].first, sizes[1].second));

    int new_size;
    for(new_size = 1; new_size < max_size; new_size <<=1);
    for(int i = 0; i < 3; i++) {
        matrixes[i] = getResizedMatrix(matrixes[i], new_size);
    }
    auto &a = matrixes[0], &b = matrixes[1], &c = matrixes[2];

    auto start = get_time();
    matrix_strassen_multiply(a, b, c);
    auto finish = get_time();

    if(pathes[2] != "-") {
        write_matrix(pathes[2], c, sizes[2]);
    }

    auto diff = finish - start;
    int _1k = 1000;
//    cout << "calculation time: " << diff / (_1k*_1k) << " sec, "
//                                 << (diff % (_1k*_1k)) / _1k << " millisec, "
//                                 << (diff % _1k) << " microsec;\n";
    cout << diff << endl;
    return 0;
}
// 303221128
// 307227341
//1005268121