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
    delete[] array_working_part;
}


void QuickSortMPI::prepareMPI() {
    MPI_Init(&argc, &argv);
}


void QuickSortMPI::stopMPI() {
    MPI_Finalize();
}


double QuickSortMPI::run() {

    MPI_Comm_size(MPI_COMM_WORLD, &MPI_initial_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &MPI_initial_rank);

    char num[3];
    sprintf(num, "%d", MPI_initial_rank);
    std::string outputs = "outputs";
    std::string path = outputs + std::string("/log") + std::string(num);
    log.open(path);

    if(__builtin_popcount(MPI_initial_rank) != 1){
        std::cout << "number of processes should be power of two.\n";
        exit(0);
    }

    if(MPI_initial_rank == MAIN_PROCESS){
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
    sendInitialData();
    waitInitialData();
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


void QuickSortMPI::startSolve() {
    while((1 << iteration) < MPI_initial_size ){
        int pivot = getPivot();
        broadcastPivot(pivot);
        rearrangePart();
        updateArr();
        split();
    }
    quickSort();
}


void QuickSortMPI::rearrangePart(int from, int to) {
    int pivot = getPivot(from, to);
    log << "iteration: " << iteration << ", pivot: " << pivot << std::endl;
    printArr(array_working_part, array_size, log, "before rearrange");
    for(int i = from, j = to - 1; i < j;){
        while(i < j && array_working_part[i] <= pivot) i++;
        while(i < j && array_working_part[j] > pivot) j--;
        std::swap(array_working_part[i], array_working_part[j]);
    }
    printArr(array_working_part, array_size, log, "after rearrange");
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
    std::sort(array_working_part + from, array_working_part + to);
}


void QuickSortMPI::sendResult() {
    MPI_Request r;
    log << "send array_working_part to parent process: " << parent_process << std::endl;
    printArr(array_working_part, array_size, log, "sorted part");
    MPI_Isend(array_working_part, array_size, MPI_INT, parent_process, ARRAY, MPI_COMM_WORLD, &r);
}


int QuickSortMPI::getPivot(int from, int to) {
    std::random_device rd;
    std::default_random_engine generator(rd());
    std::uniform_int_distribution<int> distribution(from,to-1);
    return array_working_part[distribution(generator)];

}


void QuickSortMPI::writeResult() {
    std::ofstream out(argv[2]);
    out << array_size << "\n";
    for(int i = 0; i < array_size; i++){
        out << array_working_part[i] << " ";
    }
    out << "\n";
}


void QuickSortMPI::printInfo() {
    log << "rank: " << MPI_initial_rank << " - started" << std::endl;
    log << "array_working_part size: " << array_size << std::endl;
    printArr(array_working_part, array_size, log, "array_working_part");
}


void QuickSortMPI::readInitialData() {
    std::ifstream in(argv[1]);
    iteration = 0;

    in >> array_size;

    full_array = new int[array_size];
    for(int i = 0; i < array_size; i++){
        in >> full_array[i];
    }
}


void QuickSortMPI::sendInitialData() {
    MPI_Request r;
    int part_size = array_size / MPI_initial_size;
    for(int process = 0; process < MPI_initial_size; process++){
        int first = MPI_initial_rank * part_size;
        int last = (process == MPI_initial_size - 1 ? full_array_size : first + part_size);
        int size = last-first+1;
        // cant Isend here, because we shouldn't rewrite or delete 'size' untill it is copied to MPI buffer
        MPI_Send(&size, 1, MPI_INT, process, SIZE, MPI_COMM_WORLD);
        MPI_Isend(full_array + first, size, MPI_INT, process, INIT, MPI_COMM_WORLD, &r);
    }
}


void QuickSortMPI::waitInitialData() {
    MPI_Recv(&array_size, 1, MPI_INT, MAIN_PROCESS, SIZE, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    array_working_part = new int[array_size];
    MPI_Recv(array_working_part, array_size, MPI_INT, MAIN_PROCESS, INIT, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
}
