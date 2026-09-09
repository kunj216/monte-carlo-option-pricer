//
// Created by ruairi on 21/08/2026.
//


//These functions have been kept for historical purposes, but are not used, due to them being both less efficient, and
//less accurate than the exactGBM versions.

#ifndef EULERMARYAMA_HPP
#define EULERMARYAMA_HPP
#include <vector>

namespace eulerMaruyama {
    std::pair<double,double> simulateAntitheticVariatePaths(double startingPrice, double riskFreeRate,
    double volatility, double intervalLength, int numberOfSubIntervals, unsigned seed,
    std::vector<double>* historyRegular = nullptr, std::vector<double>* historyAntithetic = nullptr);

    std::vector<std::vector<double>> getPriceHistories(unsigned int numberOfPaths, double startingPrice, double riskFreeRate,
        double volatility, double intervalLength, int numberOfSubIntervals);
    
}


#endif //EULERMARYAMA_HPP
