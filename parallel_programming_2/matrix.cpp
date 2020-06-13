//
// Created by alexander on 6/11/20.
//

#include "matrix.h"
#include <fstream>
#include <iostream>

Matrix::Matrix(const Matrix &other) {
    auto dims = other.getSize();
    rows = dims.first;
    columns = dims.second;
    matrix = other.asVectors();
}


Matrix::Matrix(int n) {
    rows = n;
    columns = n;
    matrix.resize(n, std::vector<ld>(n, 0));
}


Matrix::Matrix(const std::vector<ld> &vec, bool transposed_vector) {

    this->transposed_vector = transposed_vector;
    if(transposed_vector){
        rows = vec.size();
        columns = 1;
        matrix.resize(rows, std::vector<ld> (columns, 0));
        for(int i = 0; i < rows; ++i){
            matrix[i][0] = vec[i];
        }
    }
    else{
        rows = 1;
        columns = vec.size();
        matrix.resize(rows, vec);
    }
}


Matrix::Matrix(const std::string &path, bool is_vector, bool transposed_vector) {
    std::ifstream input(path);
    this->transposed_vector = transposed_vector;

    if(is_vector){
        input >> rows;
        columns = 1;
    }
    else{
        input >> rows >> columns;
    }

    if(transposed_vector){
        std::swap(rows, columns);
    }


    readMatrix(input);
}


Matrix::Matrix(const std::vector<std::vector<ld>> &matrix) {
    this->matrix = matrix;
    rows = matrix.size();
    if(rows > 0){
        columns = matrix[0].size();
    }
    else{
        columns = 0;
    }
}


void Matrix::readMatrix(std::ifstream &input) {
    std::cout << "here\n";
    std::cout << rows << " " << columns << std::endl;
    matrix.resize(rows, std::vector<ld>(columns));
    for(int i = 0; i < rows; i++) {
        for (int j = 0; j < columns; j++) {
            input >> matrix[i][j];
        }
    }
}


Matrix::~Matrix() {}


std::vector<ld> Matrix::getColumnAsVector(int col_num) const{
    if(col_num < 0){
        col_num = columns - col_num;
    }
    std::vector<ld> column(rows, 0);
    auto m = asVectors();
    for(int i = 0; i < rows; i++){
        column[i] = m[i][col_num];
    }
    return std::vector<ld>();
}


std::vector<ld> Matrix::getRowAsVector(int row_num) const{
    if(row_num < 0){
        row_num = rows - row_num;
    }
    return matrix.at(row_num);
}


std::pair<int, int> Matrix::getSize() const{
    return {rows, columns};
}

const std::vector<std::vector<ld>>& Matrix::asVectors() const {
    return matrix;
}


Matrix Matrix::getColumnAsMatrix(int colNum) const {
    return Matrix(getColumnAsVector(colNum), true);
}


Matrix Matrix::getRowAsMatrix(int rowNum) const {
    return Matrix(getRowAsVector(rowNum), false);
}


Matrix Matrix::getTransposed() const{
    // TODO implement this
    return Matrix();
}


Matrix Matrix::multiplyBy(const Matrix &matrix) const{
    // TODO implement this
    return Matrix();
}


std::vector<ld>& Matrix::operator[](int index) {
    if(index < 0){
        index = rows - index;
    }
    return matrix.at(index);
}


const std::vector<ld>& Matrix::operator[](int index) const{
    if(index < 0){
        index = rows - index;
    }
    return matrix.at(index);
}


ld Matrix::operator()(int i, int j) const {
    if(i < 0){
        i = rows - i;
    }
    if(j < 0) {
        j = columns - j;
    }
    return matrix.at(i).at(j);
}


ld &Matrix::operator()(int i, int j) {
    if(i < 0){
        i = rows - i;
    }
    if(j < 0) {
        j = columns - j;
    }
    return matrix.at(i).at(j);
}

void Matrix::print(bool less_info, bool ignore_transposed) {
    // TODO merge with another print
    bool transposed = !ignore_transposed && this->transposed_vector;

    if(less_info && (rows == 1 || columns == 1)) {
        std::cout << std::max(rows, columns) << std::endl;
    }
    else{
        std::cout << rows << " " << columns << std::endl;
    }

    for(int i = 0; i < rows; i++){
        for(int j = 0; j < columns; j++){
            std::cout << matrix[i][j];
            if(transposed){
                std::cout << std::endl;
            }
            else{
                std::cout << " ";
            }
        }
        if(!transposed) {
            std::cout << std::endl;
        }
    }
}

void Matrix::print(const std::string &to, bool less_info, bool ignore_transposed) {
    std::ofstream out(to);
    bool transposed = !ignore_transposed && this->transposed_vector;
    if(less_info && (rows == 1 || columns == 1)) {
        out << std::max(rows, columns) << std::endl;
    }
    else{
        out << rows << " " << columns << std::endl;
    }

    for(int i = 0; i < rows; i++){
        for(int j = 0; j < columns; j++){
            out << matrix[i][j];
            if(transposed){
                out << std::endl;
            }
            else{
                out << " ";
            }
        }
        if(!transposed) {
            out << std::endl;
        }
    }
}

std::vector<ld> Matrix::popColumn(int col_index) {
    prepare_index(col_index, false);
    std::vector<ld> popped(columns, 0);
    for(int i = 0; i < rows; i++){
        popped[i] = matrix[i][col_index];
        for(int j = col_index; j < columns-1; j++){
            matrix[i][j] = matrix[i][j+1];
        }
        matrix[i].pop_back();
    }
    columns--;
    return popped;
}

std::vector<ld> Matrix::popRow(int index) {
    // TODO implement this
    return std::vector<ld>();
}

int Matrix::prepare_index(int index, bool is_row) {
    //TODO replace all checks with this method
    if(index >= 0){
        return index;
    }
    if(is_row){
        return rows - index;
    }
    else{ //column
        return columns - index;
    }
}
