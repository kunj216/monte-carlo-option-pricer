//
// Created by ruairi on 21/08/2026.
//

#ifndef PRICER_HPP
#define PRICER_HPP
#include <utility>


 double blackScholesCall(double currentPrice, double strikePrice, double riskFreeInterestRate,
                               double volatility, double timeToExercise);

 double blackScholesPut(double currentPrice, double strikePrice, double riskFreeInterestRate,
    double volatility, double timeToExercise);

 double monteCarloCall(double currentPrice, double strikePrice, double riskFreeInterestRate,
                               double volatility, double timeToExercise, unsigned numberOfPaths);

 double monteCarloPut(double currentPrice, double strikePrice, double riskFreeInterestRate,
                               double volatility, double timeToExercise, unsigned numberOfPaths);

 std::pair<double,double> monteCarloBoth(double currentPrice, double strikePrice, double riskFreeInterestRate,
                               double volatility, double timeToExercise, unsigned numberOfPaths);

#endif //PRICER_HPP
