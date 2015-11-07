#!/bin/bash
cd ../data
g++ temp.cpp
./a.out $1 > in
shuf in > out
