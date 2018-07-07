all: dpll csp
dpll:
	g++ -std=c++11 -Ofast dpll.cpp -o dpll
csp:
	g++ -std=c++11 -Ofast csp.cpp -o csp
clean:
	rm csp;rm dpll
