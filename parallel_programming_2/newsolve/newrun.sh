#!/bin/bash
mpicxx newsolve.cpp newmain.cpp
mpiexec ./a.out
