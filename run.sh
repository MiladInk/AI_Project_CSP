#! /bin/bash
g++ -std=c++11 csp.cpp && ./a.out < tests/test3.txt | python3 display.py