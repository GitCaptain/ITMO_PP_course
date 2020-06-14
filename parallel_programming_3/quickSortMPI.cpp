//
// Created by alexander on 6/11/20.
//

#include "quickSortMPI.h"
#include <iostream>
#include <random>
#include <algorithm>

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
    for(int i = start; i < start + size; i++){
        out << arr[i] << " ";
    }
    out << std::endl << std::endl;
}


QuickSortMPI::QuickSortMPI(int argc, char **argv) {
    this->argc = argc;
    this->argv = argv;
    prepareMPI();
}


QuickSortMPI::~QuickSortMPI() {
    stopMPI();
    delete[] array;
}


void QuickSortMPI::prepareMPI() {
    MPI_Init(&argc, &argv);
}


void QuickSortMPI::stopMPI() {
    MPI_Finalize();
}


double QuickSortMPI::run() {

    MPI_Comm_size(MPI_COMM_WORLD, &MPI_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &MPI_rank);

    char num[3];
    sprintf(num, "%d", MPI_rank);
    std::string outputs = "outputs";
    std::string path = outputs + std::string("/log") + std::string(num);
    log.open(path);

    if(MPI_rank == MAIN_PROCESS){
        if (argc < 3) {
            std::cout << "usage: path1 path2 [d_type]\n"
                         "path1 - path to file which data has to be sorted,\n"
                         "path2 - path to write sorted data,\n";
            exit(0);
        }
        mainProcessRun();
    }
    else{
        otherProcessRun();
    }
    return calc_time;
}


void QuickSortMPI::mainProcessRun(){
    readInitialData();
    printInfo();
    // calculation starts
    double start_time = MPI_Wtime();
    startSolve();
    double end_time = MPI_Wtime();
    calc_time = end_time - start_time;
    writeResult();
}


void QuickSortMPI::otherProcessRun(){
    waitInitialData();
    printInfo();
    startSolve();
}


void QuickSortMPI::readInitialData() {
    std::ifstream in(argv[1]);
    iteration = 0;
    parent_process = -1;

    in >> array_size;
    array = new int[array_size];
    for(int i = 0; i < array_size; i++){
        in >> array[i];
    }
}


void QuickSortMPI::waitInitialData() {
    int proc = MPI_rank + 1;
    for(iteration = 0; (1<<iteration) < proc; iteration++);
    int previous_iteration_max_process = 1 << (iteration-1);
    parent_process = proc - previous_iteration_max_process - 1;
//    std::cout << "process: " << MPI_rank << " - waits data from parent process: " << parent_process << std::endl;
    MPI_Recv(&array_size, 1 * INT_SEND_COEF, MPI_INT, parent_process, SIZE, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    array = new int[array_size];
    MPI_Recv(array, array_size * INT_SEND_COEF, MPI_INT, parent_process, ARRAY, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
}


void QuickSortMPI::startSolve() {

    int current_iteration_max_process = 1 << iteration;
    int child_process = MPI_rank + current_iteration_max_process + 1;

    if(child_process > MPI_size){ // cant divide anymore
        quickSort(0, array_size);
        doOrder();
        if(MPI_rank != MAIN_PROCESS) {
            sendResult();
        }
        return;
    }
    else{
        rearrangePart(0, array_size);
        spawnProcess(child_process);
        iteration++;
        previous_array_size = array_size;
        array_size /= 2;
        startSolve();
        array_size = previous_array_size;
        iteration--;
        return;
    }
}


void QuickSortMPI::rearrangePart(int from, int to) {
    std::random_device rd;
    std::default_random_engine generator(rd());
    std::uniform_int_distribution<int> distribution(from,to-1);
    int x = array[distribution(generator)];
    log << "iteration: " << iteration << ", pivot: " << x << std::endl;
    printArr(array, array_size, log, "before rearrange");
    for(int i = from, j = to - 1; i < j;){
        while(i < j && array[i] <= x) i++;
        while(i < j && array[j] > x) j--;
        std::swap(array[i], array[j]);
    }
    printArr(array, array_size, log, "after rearrange");
}


void QuickSortMPI::spawnProcess(int child_process) {
    MPI_Request r;
    int first_half = array_size/2;
    int second_half = array_size - first_half;
    child_process--;
    log << "send data to child process: " << child_process << std::endl;
    log << "start index: " << first_half << ", size: " << second_half << ", fullsize: " << array_size << std::endl;
    printArr(array, second_half, log, "send part", first_half);
    MPI_Isend(&second_half, 1*INT_SEND_COEF, MPI_INT, child_process, SIZE, MPI_COMM_WORLD, &r);
    MPI_Isend(array + first_half, second_half * INT_SEND_COEF, MPI_INT, child_process, ARRAY, MPI_COMM_WORLD, &r);
    MPI_Irecv(array + first_half, second_half * INT_SEND_COEF, MPI_INT, child_process, ARRAY, MPI_COMM_WORLD, &r);
    requests.emplace_back(r);
}


void QuickSortMPI::quickSort(int from, int to) {
    int size = to - from;
    log << "sort " << size << "elements\n";
//    if(size == 1){
//        return;
//    }
//    rearrangePart(from, to);
//    quickSort(from, from + size/2);
//    quickSort(from + size/2, to);
    std::sort(array + from, array + to);
}


void QuickSortMPI::sendResult() {
    MPI_Request r;
    log << "send array to parent process: " << parent_process << std::endl;
    printArr(array, array_size, log, "sorted part");
    MPI_Isend(array, array_size * INT_SEND_COEF, MPI_INT, parent_process, ARRAY, MPI_COMM_WORLD, &r);
}


void QuickSortMPI::writeResult() {
    std::ofstream out(argv[2]);
    out << array_size << "\n";
    for(int i = 0; i < array_size; i++){
        out << array[i] << " ";
    }
    out << "\n";
}


void QuickSortMPI::printInfo() {
    log << "rank: " << MPI_rank << " - started" << std::endl;
    log << "send coefs:\n--int: " << INT_SEND_COEF << std::endl;
    log << "array size: " << array_size << std::endl;
    log << "iteration: " << iteration << ", parent: " << parent_process << std::endl;
    printArr(array, array_size, log, "array");
}

void QuickSortMPI::doOrder() {

    if(requests.empty()){
        return;
    }
    MPI_Request r = requests.back();
    requests.pop_back();
    MPI_Wait(&r, MPI_STATUS_IGNORE); // wait for neighbour process to return data and reorder it;

}
