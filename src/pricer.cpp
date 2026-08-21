//
// Created by ruairi on 21/08/2026.
//

#include "pricer.hpp"

#include <cmath>
#include <vector>

#include "exactGBM.hpp"
#include "util.hpp"

inline double blackScholesCall(const double currentPrice, const double strikePrice, const double riskFreeInterestRate,
                               const double volatility, const double timeToExercise) {
    const double d1 =
        (std::log(currentPrice/strikePrice)+timeToExercise*(riskFreeInterestRate + volatility*volatility/2)) /
            (volatility * std::sqrt(timeToExercise));
    const double d2 = d1 - volatility*std::sqrt(timeToExercise);
    return currentPrice*zDistCDF(d1) - strikePrice*std::exp(-riskFreeInterestRate*timeToExercise)*zDistCDF(d2);
}

inline double blackScholesPut(const double currentPrice, const double strikePrice, const double riskFreeInterestRate,
    const double volatility, const double timeToExercise) {
    const double d1 =
        (std::log(currentPrice/strikePrice)+timeToExercise*(riskFreeInterestRate + volatility*volatility/2)) /
            (volatility * std::sqrt(timeToExercise));
    const double d2 = d1 - volatility*std::sqrt(timeToExercise);
    return strikePrice*std::exp(-riskFreeInterestRate*timeToExercise)*zDistCDF(-d2) - currentPrice*zDistCDF(-d1);
}

inline double monteCarloCall(const double currentPrice, const double strikePrice, const double riskFreeInterestRate,
                               const double volatility, const double timeToExercise, const unsigned numberOfPaths) {
    const std::vector<double> possiblePrices = exactGBM::getTerminalPrices(numberOfPaths, currentPrice, riskFreeInterestRate,
        volatility, timeToExercise);
    double totalPayoff = 0;
    for (const double possiblePrice : possiblePrices) {
        totalPayoff+=(std::max(possiblePrice-strikePrice,0.0));
    }

    const double averagePayoff = totalPayoff/possiblePrices.size();

    const double discountedPayoff = averagePayoff*std::exp(-riskFreeInterestRate*timeToExercise);

    return discountedPayoff;
}

inline double monteCarloPut(const double currentPrice, const double strikePrice, const double riskFreeInterestRate,
                               const double volatility, const double timeToExercise, const unsigned numberOfPaths) {
    const std::vector<double> possiblePrices = exactGBM::getTerminalPrices(numberOfPaths, currentPrice, riskFreeInterestRate,
                                                                           volatility, timeToExercise);
    double totalPayoff = 0;
    for (const double possiblePrice : possiblePrices) {
        totalPayoff+=(std::max(strikePrice-possiblePrice,0.0));
    }

    const double averagePayoff = totalPayoff/possiblePrices.size();

    const double discountedPayoff = averagePayoff*std::exp(-riskFreeInterestRate*timeToExercise);

    return discountedPayoff;
}