all: dpll csp
dpll: dpll.cpp dpll.h
	g++ -std=c++11 -Ofast dpll.cpp -o dpll
csp: csp.cpp csp.h
	g++ -std=c++11 -Ofast csp.cpp -o csp
clean:
	rm csp;rm dpll;
