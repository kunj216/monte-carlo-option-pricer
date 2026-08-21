#include <future>
#include <vector>
#include "exactGBM.hpp"
#include "util.hpp"
//
// Created by ruairi on 21/08/2026.
//
namespace exactGBM {

    std::pair<double,double> analyticalTerminalAntitheticPrices(const double startingPrice, const double riskFreeRate,
        const double volatility, const double intervalLength, const unsigned seed) {

        std::mt19937 randomNumberGenerator(seed);
        std::normal_distribution<double> zDist(0,1);

        const double Wt = zDist(randomNumberGenerator) * std::sqrt(intervalLength);

        const double Y1 = startingPrice * std::exp( (riskFreeRate - 0.5 * volatility*volatility)*intervalLength + volatility * Wt);
        const double Y2 = startingPrice * std::exp( (riskFreeRate - 0.5 * volatility*volatility)*intervalLength - volatility * Wt);

        return {Y1,Y2};
    }

    std::pair<std::vector<double>, std::vector<double>> simulateAntitheticVariatePaths(
        const double startingPrice, const double riskFreeRate, const double volatility, const double intervalLength,
        const unsigned numberOfSubintervals, const unsigned seed) {

        std::mt19937 randomNumberGenerator(seed);
        std::normal_distribution<double> zDist(0,1);

        const double subintervalLength = intervalLength/numberOfSubintervals;
        const double sqrtDeltaT = std::sqrt(subintervalLength);

        std::vector<double> regularPrices;
        std::vector<double> antitheticPrices;
        regularPrices.push_back(startingPrice);
        antitheticPrices.push_back(startingPrice);

        for (int subinterval = 0; subinterval < numberOfSubintervals; subinterval++) {
            const double dWt = zDist(randomNumberGenerator) * sqrtDeltaT;
            regularPrices.push_back(regularPrices.back() * std::exp( (riskFreeRate - 0.5 * volatility*volatility)*subintervalLength + volatility * dWt));
            antitheticPrices.push_back(antitheticPrices.back() *  std::exp( (riskFreeRate - 0.5 * volatility*volatility)*subintervalLength - volatility * dWt));
        }

        return {regularPrices, antitheticPrices};
    }

    std::vector<std::vector<double>> getPriceHistories(unsigned int numberOfPaths, const double startingPrice, const double riskFreeRate,
        const double volatility, const double intervalLength, const int numberOfSubIntervals) {
        if (numberOfPaths%2==1) numberOfPaths--; //Confiscate the extra path if someone has the gall to call this function with an odd number of paths
        auto seeds = generateSeeds(numberOfPaths/2);

        std::vector<std::vector<double>> histories;
        histories.resize(numberOfPaths);
        std::vector<std::future<std::pair<std::vector<double>,std::vector<double>>>> futures;
        futures.reserve(numberOfPaths/2);

        for (unsigned i = 0; i < numberOfPaths/2; ++i) {
            futures.push_back(
            std::async(std::launch::async, exactGBM::simulateAntitheticVariatePaths,
            startingPrice, riskFreeRate, volatility,
            intervalLength, numberOfSubIntervals, seeds[i]));
        }

        std::vector<std::vector<double>> output;
        output.reserve(numberOfPaths);

        for (unsigned i = 0; i < numberOfPaths; i +=2 ) {
             auto [fst, snd] = futures[i/2].get();
            output.push_back(fst);
            output.push_back(snd);
        }
        return output;
    }

    std::vector<double> getTerminalPrices(unsigned int numberOfPaths, const double startingPrice, const double riskFreeRate,
                                          const double volatility, const double intervalLength) {

        if (numberOfPaths%2==1) numberOfPaths--; //Confiscate the extra path if someone has the gall to call this function with an odd number of paths
        auto seeds = generateSeeds(numberOfPaths/2);

        std::vector<std::future<std::pair<double,double>>> futures;
        futures.reserve(numberOfPaths/2);

        for (unsigned i = 0; i<numberOfPaths/2; ++i) {
            futures.push_back(std::async(std::launch::async, exactGBM::analyticalTerminalAntitheticPrices, startingPrice, riskFreeRate, volatility, intervalLength, seeds[i]));
        }

        std::vector<double> output;
        output.reserve(numberOfPaths);

        for (unsigned i = 0; i<numberOfPaths; i+=2) {
            auto [fst, snd] = futures[i/2].get();
            output.push_back(fst);
            output.push_back(snd);
        }
        return output;
    }
}