//
// Created by alexander on 6/11/20.
//

#ifndef PARALLEL_PROGRAMMING_2_MATRIX_H
#define PARALLEL_PROGRAMMING_2_MATRIX_H

#include <string>
#include <vector>

typedef long long ll;

class Matrix{
public:

    Matrix(int n = 0);
    Matrix(const std::string &path);
    Matrix(const Matrix &matrix);
    ~Matrix();

    std::vector<ll> getColumn(int colNum) const;
    std::vector<ll> getRow(int rowNum) const;
    std::pair<int, int> getSize() const;
    Matrix getTransposed() const;
    Matrix multiplyBy(const Matrix &matrix) const;

    std::vector<std::vector<ll>> asVectors() const;

private:
    bool transposed;
    int rows;
    int columns;
    std::vector<std::vector<ll>> matrix;

};

#endif //PARALLEL_PROGRAMMING_2_MATRIX_H
