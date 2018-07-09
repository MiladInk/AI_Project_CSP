all: dpll csp csp_fast
dpll: dpll.cpp dpll.h
	g++ -std=c++11 -Ofast dpll.cpp -o dpll
csp: csp.cpp csp.h
	g++ -std=c++11 -Ofast csp.cpp -o csp
csp_fast: csp_fast.cpp
	g++ -std=c++11 -Ofast csp_fast.cpp -o csp_fast
clean:
	rm csp;rm dpll;

