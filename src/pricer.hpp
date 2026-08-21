//
// Created by ruairi on 21/08/2026.
//

#ifndef WIENERSIMULATION_PRICER_HPP
#define WIENERSIMULATION_PRICER_HPP


inline double blackScholesCall(double currentPrice, double strikePrice, double riskFreeInterestRate,
                               double volatility, double timeToExercise);

inline double blackScholesPut(double currentPrice, double strikePrice, double riskFreeInterestRate,
    double volatility, double timeToExercise);

inline double monteCarloCall(double currentPrice, double strikePrice, double riskFreeInterestRate,
                               double volatility, double timeToExercise, unsigned numberOfPaths);

inline double monteCarloPut(double currentPrice, double strikePrice, double riskFreeInterestRate,
                               double volatility, double timeToExercise, unsigned numberOfPaths);

#endif //WIENERSIMULATION_PRICER_HPP
