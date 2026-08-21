#include <iostream>
#include <ostream>

#include "util.hpp"
#include "eulerMaruyama.hpp"
#include "exactGBM.hpp"
#include "pricer.hpp"



int main() {
    std::cout << monteCarloCall(100, 100, 0.05, 0.02, 1, 1000) << std::endl;
    std::cout << blackScholesCall(100,100,0.05,0.02,1) << std::endl;
    std::cout << monteCarloPut(100, 100, 0.05, 0.02, 1, 100000) << std::endl;
    std::cout << blackScholesPut(100,100,0.05,0.02,1);
}

