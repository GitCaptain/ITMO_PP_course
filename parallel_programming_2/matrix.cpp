//
// Created by alexander on 6/11/20.
//

#include "matrix.h"
#include "fstream"

Matrix::Matrix(const std::string &path) {
    std::ifstream input(path);
    input >> rows >> columns;
    matrix.resize(rows, std::vector<ll>(columns));
    for(int i = 0; i < rows; ++i)
        for(int j = 0; j < columns; ++j)
            input >> matrix[i][j];
    transposed = false;
}

Matrix::Matrix(const Matrix &other) {
    auto dims = other.getSize();
    rows = dims.first;
    columns = dims.second;
    transposed = false;
    for(auto &row: other.asVectors()){
        matrix.emplace_back(row);
    }
}

Matrix::Matrix(int n) {
    rows = n;
    columns = n;
    transposed = false;
    matrix.resize(n, std::vector<ll>(n, 0));
}

std::vector<ll> Matrix::getColumn(int colNum) const{
    return std::vector<ll>();
}

std::vector<ll> Matrix::getRow(int rowNum) const{
    return matrix.at(rowNum);
}

Matrix Matrix::getTransposed() const{
    return Matrix();
}

Matrix Matrix::multiplyBy(const Matrix &matrix) const{
    return Matrix();
}

Matrix::~Matrix() {}

std::pair<int, int> Matrix::getSize() const{
    return {rows, columns};
}

std::vector<std::vector<ll>> Matrix::asVectors() const {
    return matrix;
}

