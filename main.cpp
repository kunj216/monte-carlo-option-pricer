#include <iostream>
#include <ostream>
#include <random>
#include <vector>
std::vector<double> simulate_GBM_path(const double startingPrice, const double drift, const double volatility, const double T, const int N, std::mt19937& randomNumberGenerator) {
    std::vector<double> path;
    path.reserve(N+1);
    path.push_back(startingPrice);

    std::normal_distribution<double> zDist(0,1);

    double Y = startingPrice;
    const double dt = T/N;


    for (int n = 0; n < N; ++n) {
        const double dWt = zDist(randomNumberGenerator)*std::sqrt(dt);
        Y = Y*(1+drift*dt+volatility*dWt);
        path.push_back(Y);
    }
    return path;
}

int main() {
    std::mt19937 randomNumberGenerator(std::random_device{}());
    const auto path = simulate_GBM_path(100, 0.05, 0.2, 1, 365, randomNumberGenerator);
    for (uint i = 0; i < path.size()-1; ++i) {
        const auto price = path[i];
        std::cout << price << ",";
    }
    std::cout << path.back();
}