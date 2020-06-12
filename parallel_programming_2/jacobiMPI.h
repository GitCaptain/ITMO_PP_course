//
// Created by alexander on 6/11/20.
//

#ifndef PARALLEL_PROGRAMMING_2_JACOBIMPI_H
#define PARALLEL_PROGRAMMING_2_JACOBIMPI_H

#include <vector>

typedef long long ll;

class JacobiMPI{
public:
    JacobiMPI(const std::vector<std::vector<ll>> &matrix, const std::vector<ll> &free, double precision);
    void solve();
    ~JacobiMPI();

private:
    std::vector<std::vector<ll>> matrix;
    std::vector<ll> free;
    double precision;

    void prepareMPI();
    void stopMPI();

};

#endif //PARALLEL_PROGRAMMING_2_JACOBIMPI_H
