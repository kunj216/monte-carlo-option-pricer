//
// Created by ruairi on 21/08/2026.
//

#ifndef WIENERSIMULATION_EULERMARYAMA_HPP
#define WIENERSIMULATION_EULERMARYAMA_HPP
#include <vector>

namespace eulerMaruyama {
    std::pair<double,double> simulateAntitheticVariatePaths(double startingPrice, double riskFreeRate,
    double volatility, double intervalLength, int numberOfSubIntervals, unsigned seed,
    std::vector<double>* historyRegular = nullptr, std::vector<double>* historyAntithetic = nullptr);

    std::vector<std::vector<double>> getPriceHistories(unsigned int numberOfPaths, double startingPrice, double riskFreeRate,
        double volatility, double intervalLength, int numberOfSubIntervals);
    
}


#endif //WIENERSIMULATION_EULERMARYAMA_HPP
