//
// Created by alexander on 6/11/20.
//

#include "jacobiMPI.h"
#include <vector>
#include <cmath>
#include <iostream>
#include <fstream>


std::vector<ld> readVector(const std::string &path){
    std::ifstream in(path);
    int sz;
    in >> sz;
    std::vector<ld> readed(sz);
    for(auto &e: readed){
        in >> e;
    }
    return readed;
}


void writeVector(const std::string &path, const std::vector<ld> &vec){
    std::ofstream out;
    out << vec.size() << std::endl;
    for(auto e: vec){
        out << e << std::endl;
    }
}


bool check_received_success(int status){
    return status == MPI_SUCCESS;
}




JacobiMPI::JacobiMPI(int argc, char **argv) {
    this->argc = argc;
    this->argv = argv;
    prepareMPI(argc, argv);
}


void JacobiMPI::solve(int index_from, int index_to) {
    std::vector<ld> new_result;
    for (int i = index_from; i < index_to; i++) {
        ld sum = 0;
        for (int j = 0; j < matrix[i].size(); j++) {
            if (i == j) {
                continue;
            }
            sum += matrix(i, j) * result[j];
        }
        new_result[i] = (free[i] - sum) / matrix(i, i);
    }
}


JacobiMPI::~JacobiMPI() {
    stopMPI();
}


void JacobiMPI::prepareMPI(int argc, char **argv) {
    MPI_Init(&argc, &argv);
}


void JacobiMPI::stopMPI() {
    MPI_Finalize();
}


void JacobiMPI::mainProcessRun(){
    if(argc < 5){
        std::cout << "usage: path1 path2 double path3\n"
                     "path1 - path to matrix\n"
                     "path2 - path to result approximation\n"
                     "double - precision value\n"
                     "path3 - path for output.\n";
        exit(0);
    }

    matrix = Matrix(argv[1]);
    free = matrix.popColumn();

    result = readVector(argv[2]);
    precision = atof(argv[3]);
    std::string output(argv[4]);

    auto sizes = matrix.getSize();
    int matrix_rows = sizes.first;
    int matrix_cols = sizes.second;

    // sending initial info
    std::vector<int> matrix_info = {matrix_rows, matrix_cols};
    for(int process_index = 0; process_index < MPI_size; process_index++){
        if(process_index == MAIN_PROCESS){
            continue;
        }
        MPI_Isend(&matrix_info, 2, MPI_INT, process_index, INFO, MPI_COMM_WORLD, nullptr);
        MPI_Isend(&precision, 1, MPI_DOUBLE, process_index, PRECISION, MPI_COMM_WORLD, nullptr);
        MPI_Isend(&free, free.size(), MPI_DOUBLE, process_index, FREE, MPI_COMM_WORLD, nullptr);
        for(int i = 0; i < matrix_rows; i++){
            MPI_Isend(&matrix[i], matrix_cols, MPI_DOUBLE, process_index, MATRIX, MPI_COMM_WORLD, nullptr);
        }
    }
    sendResult();

    int part = matrix_rows / MPI_size;
    int solve_from = part * MPI_rank;
    int solve_to = (MPI_rank == MPI_size - 1 ? matrix_rows : solve_from + part);

    // calculation starts
    double start_time = MPI_Wtime();
    while(true){
        //solve our part
        solve(solve_from, solve_to);
        // get and merge new result
        std::vector<ld> new_result = getResult();
        bool done = getMaxVectorsDiff(new_result, result) < precision;
        result = new_result;
        // send merged result
        if(done || failed) {
            sendResult(DONE);
            break;
        }
        else{
            sendResult();
        }
    }
    double end_time = MPI_Wtime();
    calc_time = end_time - start_time;

    if(!failed) {
        writeVector(argv[5], result);
    }
}


void JacobiMPI::otherProcessRun(){

    std::vector<int> matrix_info(2);
    int err_status;

    err_status = MPI_Recv(&matrix_info, 2, MPI_INT, MAIN_PROCESS, INFO, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    failed |= !check_received_success(err_status);

    int matrix_rows = matrix_info[0];
    int matrix_cols = matrix_info[1];

    err_status = MPI_Recv(&precision, 1, MPI_DOUBLE, MAIN_PROCESS, PRECISION, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    failed |= !check_received_success(err_status);

    free.resize(matrix_rows);
    err_status = MPI_Recv(&free, matrix_rows, MPI_DOUBLE, MAIN_PROCESS, FREE, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    failed |= !check_received_success(err_status);

    std::vector<std::vector<ld>> matrix(matrix_rows, std::vector<ld>(matrix_cols));
    for(int i = 0; i < matrix_rows; i++){
        err_status = MPI_Recv(&matrix[i], matrix_cols, MPI_DOUBLE, MAIN_PROCESS, MATRIX, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        failed |= check_received_success(err_status);
    }
    this->matrix = Matrix(matrix);

    result.resize(matrix_rows);
    err_status = MPI_Recv(&result, matrix_rows, MPI_DOUBLE, MAIN_PROCESS, RESULT, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    failed |= !check_received_success(err_status);

    int part = matrix_rows / MPI_size;
    int solve_from = part * MPI_rank;
    int solve_to = (MPI_rank == MPI_size - 1 ? matrix_rows : solve_from + part);

    while(true){
        solve(solve_from, solve_to);
        int tag = failed ? FAIL : RESULT;
        MPI_Send(&result, result.size(), MPI_DOUBLE, MAIN_PROCESS, tag, MPI_COMM_WORLD);
        if(tag == failed){
            break;
        }
        err_status = MPI_Recv(&result, result.size(), MPI_DOUBLE, MAIN_PROCESS, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        failed |= check_received_success(err_status);
        if(tag == DONE){
            break;
        }
    }
}


double JacobiMPI::run() {

    MPI_Comm_size(MPI_COMM_WORLD, &MPI_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &MPI_rank);


    if(MPI_rank == MAIN_PROCESS){
        mainProcessRun();
    }
    else{
        otherProcessRun();
    }

    return calc_time;
}


ld JacobiMPI::getMaxVectorsDiff(const std::vector<ld> &a, const std::vector<ld> &b) {
    ld res = 0;
    for(int i = 0; i < a.size(); i++){
        res = std::max(res, std::fabs(a[i]-b[i]));
    }
    return res;
}


std::vector<ld> JacobiMPI::getResult() {

    int matrix_rows = matrix.getSize().first;
    int part = matrix_rows / MPI_size;

    std::vector<ld> new_result(matrix_rows);

    for(int process_index = 0; process_index < MPI_size; process_index++) {
        if(process_index == MAIN_PROCESS){
            continue;
        }

        int from = part * process_index;
        int count = part;
        if (process_index == MPI_size - 1) {
            count = matrix_rows - from + 1;
        }
        MPI_Status status;
        MPI_Recv(&new_result + from, count, MPI_DOUBLE, process_index, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

        if (status.MPI_TAG == FAIL) {
            std::cerr << "process: " << process_index << " failed on data receiving\n";
        }

        if (status.MPI_ERROR != MPI_SUCCESS) {
            std::cerr << "process: " << process_index << " failed with status: " << status.MPI_ERROR << std::endl;
        }
    }
    return new_result;
}

void JacobiMPI::sendResult(int tag) {
    for(int process_index = 0; process_index < MPI_size; process_index++) {
        if(process_index == MAIN_PROCESS){
            continue;
        }
        MPI_Isend(&result, result.size(), MPI_DOUBLE, process_index, tag, MPI_COMM_WORLD, nullptr);
    }
}
