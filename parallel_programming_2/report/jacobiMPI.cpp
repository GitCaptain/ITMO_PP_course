//
// Created by alexander on 6/11/20.
//

#include "jacobiMPI.h"
#include <vector>
#include <cmath>
#include <iostream>


std::pair<int, int> readMatrixAndFree(ld **&matrix, ld *&free, const std::string &path){
    std::ifstream in(path);
    int n, m;
    in >> n >> m;
    matrix = new ld*[n];
    free = new ld[n];
    m--;
    for(int i = 0; i < n; ++i){
        matrix[i] = new ld[m];
        for(int j = 0; j < m; j++){
            in >> matrix[i][j];
        }
        in >> free[i];
    }
    return {n, m};
}


void readInitial(ld *&init, const std::string &path){
    std::ifstream in(path);
    int n;
    in >> n;
    init = new ld[n];
    for(int i = 0; i < n; ++i){
        in >> init[i];
    }
}


template <typename T>
void printArr(T* arr, int size, const std::string arrname="", int start=0){
    if(arrname != ""){
        std::cout << arrname << std::endl;
    }
    for(int i = start; i < start + size; i++){
        std::cout << arr[i] << " ";
    }
    std::cout << std::endl;
}


template <typename T>
void printArr(T* arr, int size, std::ofstream &out, const std::string arrname="", int start=0){
    if(arrname != ""){
        out << arrname << std::endl;
    }
    out << size << std::endl;
    for(int i = start; i < start + size; i++){
        out << arr[i] << " ";
    }
    out << std::endl << std::endl;
}


JacobiMPI::JacobiMPI(int argc, char **argv) {
    this->argc = argc;
    this->argv = argv;
    prepareMPI(argc, argv);
}


JacobiMPI::~JacobiMPI() {
    stopMPI();
    delete[] result;
    delete[] free;
    delete[] new_result;
    for(int i = 0; i < matrix_rows; i++){
        delete[] matrix[i];
    }
    delete[] matrix;
}


void JacobiMPI::prepareMPI(int argc, char **argv) {
    MPI_Init(&argc, &argv);
}


void JacobiMPI::stopMPI() {
//    log << "finalize\n";
    MPI_Finalize();
}


double JacobiMPI::run() {

    MPI_Comm_size(MPI_COMM_WORLD, &MPI_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &MPI_rank);
    if(MPI_rank == MAIN_PROCESS) {
//        std::cout << "MPI size: " << MPI_size << std::endl;
        if (argc < 5) {
            std::cout << "usage: path1 path2 double path3\n"
                         "path1 - path to matrix\n"
                         "path2 - path to initial approximation\n"
                         "double - precision value\n"
                         "path3 - path for output.\n";
            exit(0);
        }
        readData();
    }

//    char num[3];
//    sprintf(num, "%d", MPI_rank);
//    std::string outputs = "outputs";
//    std::string path = outputs + std::string("/log") + std::string(num);
//    log.open(path);


    int matrix_info[] = {matrix_rows, matrix_cols};
    MPI_Bcast(matrix_info, 2, MPI_INT, MAIN_PROCESS, MPI_COMM_WORLD);
    matrix_rows = matrix_info[0];
    matrix_cols = matrix_info[1];

    matrix_part = matrix_rows / MPI_size;

    auto bounds = countProcessBounds(MPI_rank);
    processPartStart = bounds.first;
    processPartEnd = bounds.second;

    if(MPI_rank != MAIN_PROCESS){
        initOthers();
    }

    broadcastInitial();

    if(MPI_rank == MAIN_PROCESS){
        mainProcessRun();
        std::cerr << matrix_rows << " " << MPI_size << " " << calc_time << std::endl;
    }
    else{
        otherProcessRun();
    }

    return calc_time;
}


void JacobiMPI::mainProcessRun(){
//    printInfo();
    // calculation starts
    double start_time = MPI_Wtime();
    startSolve();
    double end_time = MPI_Wtime();
    calc_time = end_time - start_time;

    if(!failed) {
        writeResult();
    }
    else{
        std::cout << "iterations limit exceeded, calculation failed\n";
    }
}


void JacobiMPI::otherProcessRun(){

//    printInfo();
    startSolve();
}


ld JacobiMPI::getMaxVectorsDiff() {
    ld res = 0;
    for(int i = 0; i < matrix_rows; i++){
        res = std::max(res, std::fabs(result[i]-new_result[i]));
    }
    return res;
}


void JacobiMPI::writeResult() {
    std::ofstream out(output);
    out << matrix_rows << "\n";
    for(int i = 0; i < matrix_rows; i++){
        out << result[i] << "\n";
    }
}


void JacobiMPI::mergeResult() {
    for(int process = 0; process < MPI_size; process++) {
        auto bounds = countProcessBounds(process);
//        log << "broadcast: [" << bounds.first << ", " << bounds.second << "), sz: " << bounds.second-bounds.first
//            << ", from process: " << process << std::endl;
        MPI_Bcast(new_result + bounds.first,  bounds.second-bounds.first, MPI_LONG_DOUBLE, process, MPI_COMM_WORLD);
//        log << "process: " << process << " broadcast success\n";
    }
}


void JacobiMPI::readData() {
    auto sizes = readMatrixAndFree(matrix, free, argv[1]);
    matrix_rows = sizes.first;
    matrix_cols = sizes.second;
    readInitial(result, argv[2]);
    new_result = new ld[matrix_rows];
    precision = atof(argv[3]);
    output = argv[4];
}


void JacobiMPI::initOthers() {
    new_result = new ld[matrix_rows];
    result = new ld[matrix_rows];
    free = new ld[matrix_rows];
    matrix = new ld*[matrix_rows];
    for(int i = 0; i < matrix_rows; ++i){
        matrix[i] = new ld[matrix_cols];
    }
//    log << "init success\n";
}


void JacobiMPI::broadcastInitial() {
    // broadcasting initial info
    MPI_Bcast(&precision, 1, MPI_LONG_DOUBLE, MAIN_PROCESS, MPI_COMM_WORLD);
    MPI_Bcast(free, matrix_rows, MPI_LONG_DOUBLE, MAIN_PROCESS, MPI_COMM_WORLD);
    for(int i = 0; i < matrix_rows; i++){
        MPI_Bcast(matrix[i], matrix_cols, MPI_LONG_DOUBLE, MAIN_PROCESS, MPI_COMM_WORLD);
    }
    MPI_Bcast(result, matrix_rows, MPI_LONG_DOUBLE, MAIN_PROCESS, MPI_COMM_WORLD);
//    log << "initial broadcast success\n";
}


void JacobiMPI::startSolve() {
    while(true){
        //solve our part
        bool over = solvePart(processPartStart, processPartEnd);
//        printArr(new_result, matrix_rows, log,  "new result before merge");
        // get and merge new result
        mergeResult();
//        printArr(new_result, matrix_rows, log,  "new result after merge");

        ld diff = getMaxVectorsDiff();
//        std::cout << diff << " " << precision << " " << (diff < precision) << std::endl;
        bool done = diff < precision;

        for(int i = 0; i < matrix_rows; i++){
            result[i] = new_result[i];
        }

        if(over && !done){
            failed = true;
        }

        if(done || failed) {
//            log << "done\n";
            break;
        }
    }
}


bool JacobiMPI::solvePart(int index_from, int index_to) {
    static int iteration = 0;
    static const int MAX_ITERATIONS = 1000;
//    std::cout << "MPI rank: " << MPI_rank << " solves from: " << index_from << ", to: " << index_to << std::endl
//              <<"iteration: " << iteration++ << std::endl;
    for (int i = index_from; i < index_to; i++) {
        ld sum = 0;
        for (int j = 0; j < matrix_cols; j++) {
            if (i == j) {
                continue;
            }
//            std::cout << "i, j: " << i << ", " << j << std::endl;
//            std::cout << matrix[i][j] << " " << result[j] << " " << matrix[i][j] * result[j] << " " << sum << " ";
            sum += matrix[i][j] * result[j];
//            std::cout << sum << "\n";
        }
        new_result[i] = (free[i] - sum) / matrix[i][i];
    }
    iteration++;
    return iteration > MAX_ITERATIONS;
}


void JacobiMPI::printInfo() {
    log << "rank: " << MPI_rank << " - started" << std::endl;

    log << "matrix rows: " << matrix_rows << ", matrix cols: " << matrix_cols << std::endl;

    log << "get precision: \n";
    log << precision << std::endl;

    log << "get free:\n";
    printArr(free, matrix_rows, log, "free");

    log << "get matrix:\n";
    for(int i = 0; i < matrix_rows; i++){
        for(int j = 0; j < matrix_cols; j++){
            log << matrix[i][j] << " ";
        }
        log << std::endl;
    }
    log << "get result:\n";
    for(int i = 0; i < matrix_rows; i++) {
        log << result[i] << " ";
    }
    log << std::endl;
}

std::pair<int, int> JacobiMPI::countProcessBounds(int process) {
    int rem = matrix_rows % MPI_size;
    int start;
    int end;
    if(rem > process){
        start = (matrix_part + 1) * process;
        end = start + (matrix_part + 1);
    }
    else{
        start = (matrix_part + 1) * rem + matrix_part * (process - rem);
        end = start + matrix_part;
    }
    return {start, end};
}