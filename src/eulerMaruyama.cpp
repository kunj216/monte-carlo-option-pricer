//
// Created by ruairi on 21/08/2026.
//
#include "eulerMaruyama.hpp"

#include <future>

#include "util.hpp"


//These functions have been kept for historical purposes, but are not used, due to them being both less efficient, and
//less accurate than the exactGBM versions.
namespace eulerMaruyama {
    std::pair<double,double> simulateAntitheticVariatePaths(const double startingPrice, const double riskFreeRate,
        const double volatility, const double intervalLength, const int numberOfSubIntervals, const unsigned seed,
        std::vector<double>* historyRegular, std::vector<double>* historyAntithetic) {
        if  (historyRegular != nullptr) {
            historyRegular -> reserve(numberOfSubIntervals+1);
            historyRegular -> push_back(startingPrice);
        }
        if (historyAntithetic != nullptr) {
            historyAntithetic -> reserve(numberOfSubIntervals+1);
            historyAntithetic -> push_back(startingPrice);
        }

        std::mt19937 randomNumberGenerator(seed);
        std::normal_distribution<> zDist(0,1);

        double Y1 = startingPrice;
        double Y2 = startingPrice;
        const double deltaTime = intervalLength/numberOfSubIntervals;
        const double sqrtDeltaTime = std::sqrt(deltaTime);

        for (int subinterval = 0; subinterval < numberOfSubIntervals; subinterval++) {
            const double dWt = zDist(randomNumberGenerator)*sqrtDeltaTime;
            Y1 = Y1*(1+riskFreeRate*deltaTime+volatility*dWt);
            Y2 = Y2*(1+riskFreeRate*deltaTime-volatility*dWt);

            if (historyRegular != nullptr) historyRegular -> push_back(Y1);
            if (historyAntithetic != nullptr) historyAntithetic -> push_back(Y2);
        }

        return {Y1,Y2};
    }



    std::vector<std::vector<double>> getPriceHistories(unsigned int numberOfPaths, const double startingPrice, const double riskFreeRate,
        const double volatility, const double intervalLength, const int numberOfSubIntervals) {
        auto seeds = generateSeeds(numberOfPaths);

        if (numberOfPaths % 2 == 1) numberOfPaths--; //If someone tries to call this function with an odd number of paths, confiscate the extra one and don't give it back

        std::vector<std::vector<double>> histories;
        histories.resize(numberOfPaths);
        std::vector<std::future<std::pair<double,double>>> futures;
        futures.reserve(numberOfPaths/2);

        for (int i = 0; i < numberOfPaths; i+=2) {
            futures.push_back(std::async(std::launch::async, eulerMaruyama::simulateAntitheticVariatePaths,
                startingPrice, riskFreeRate, volatility,
                intervalLength, numberOfSubIntervals, seeds[i], &histories[i], &histories[i+1]));
        }

        std::vector<std::vector<double>> output;
        output.reserve(numberOfPaths);

        for (int i = 0; i < numberOfPaths; i+=2) {
            futures[i/2].get();
            output.push_back(histories[i]);
            output.push_back(histories[i+1]);
        }

        return output;

    }


}