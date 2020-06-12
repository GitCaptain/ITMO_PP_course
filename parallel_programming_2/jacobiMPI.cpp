//
// Created by alexander on 6/11/20.
//

#include "jacobiMPI.h"

JacobiMPI::JacobiMPI(const std::vector<std::vector<ll>> &matrix, const std::vector<ll> &free, double precision) {
    this->matrix = matrix;
    this->free = free;
    this->precision = precision;
}

void JacobiMPI::solve() {

}

JacobiMPI::~JacobiMPI() {

}

void JacobiMPI::prepareMPI() {

}

void JacobiMPI::stopMPI() {

}

