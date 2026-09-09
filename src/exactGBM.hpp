//
// Created by ruairi on 20/08/2026.
//

#ifndef GBM_HPP
#define GBM_HPP
#include <utility>
#include <vector>
namespace exactGBM {

    std::pair<double,double> analyticalTerminalAntitheticPrices( double startingPrice,  double riskFreeRate,
                                                                 double volatility,  double intervalLength,  unsigned seed);

    std::pair<std::vector<double>, std::vector<double>> simulateAntitheticVariatePaths(
        double startingPrice, double riskFreeRate, double volatility, double intervalLength,
        unsigned numberOfSubintervals, unsigned seed);

    std::vector<std::vector<double>> getPriceHistories(unsigned int numberOfPaths, double startingPrice, double riskFreeRate,
            double volatility, double intervalLength, unsigned numberOfSubIntervals);
    
    std::vector<double> getTerminalPrices(unsigned int numberOfPaths, double startingPrice, double riskFreeRate,
                                          double volatility, double intervalLength);
}

#endif //GBM_HPP
