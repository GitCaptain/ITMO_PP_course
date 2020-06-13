//
// Created by alexander on 6/11/20.
//

#ifndef PARALLEL_PROGRAMMING_2_MATRIX_H
#define PARALLEL_PROGRAMMING_2_MATRIX_H

#include <string>
#include <vector>

typedef long double ld;

class Matrix{
public:

    Matrix(int n = 0);
    Matrix(const std::string &path, bool is_vector=false, bool transposed_vector=false);
    Matrix(const Matrix &matrix);
    Matrix(const std::vector<ld> &vec, bool transposed_vector);
    Matrix(const std::vector<std::vector<ld>> &matrix);
    ~Matrix();

    std::vector<ld> getColumnAsVector(int col_num) const;
    std::vector<ld> getRowAsVector(int row_num) const;

    Matrix getColumnAsMatrix(int colNum) const;
    Matrix getRowAsMatrix(int rowNum) const;

    std::pair<int, int> getSize() const;
    Matrix getTransposed() const;
    Matrix multiplyBy(const Matrix &matrix) const;

    const std::vector<std::vector<ld>>& asVectors() const;

    const std::vector<ld>& operator[](int index) const;
    std::vector<ld>& operator[](int index);

    ld operator()(int i, int j) const;
    ld& operator()(int i, int j);

    void print(bool less_info=false, bool ignore_transposed=false);
    void print(const std::string &to, bool less_info=false, bool ignore_transposed=false);

    std::vector<ld> popColumn(int col_index=-1);
    std::vector<ld> popRow(int row_index=-1);

private:
    bool transposed_vector = false; //used only for vectors
    int rows;
    int columns;
    std::vector<std::vector<ld>> matrix;

    void readMatrix(std::ifstream &input);
    int prepare_index(int index, bool is_row);
};

#endif //PARALLEL_PROGRAMMING_2_MATRIX_H
