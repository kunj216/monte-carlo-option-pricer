#include <iostream>
#include <ostream>
#include <random>
#include <vector>
#include <future>
#include <cmath>

std::vector<unsigned> generateSeeds(const unsigned int numberOfPaths) {
    std::random_device random_device;
    std::vector<unsigned> seeds;
    seeds.reserve(numberOfPaths);
    for (unsigned int i = 0; i < numberOfPaths; ++i) {
        seeds.push_back(random_device());
    }
    return seeds;
}

inline double zDistCDF(const double x) {
    constexpr double SQRT_HALF = 0.70710678118654752440;
    return 0.5 * std::erfc(-x * SQRT_HALF);
}

namespace eulerMaryama {
    std::pair<double,double> simulateAntitheticVariatePaths(const double startingPrice, const double riskFreeRate,
        const double volatility, const double intervalLength, const int numberOfSubIntervals, const unsigned seed,
        std::vector<double>* historyRegular = nullptr, std::vector<double>* historyAntithetic = nullptr) {
        if  (historyRegular != nullptr) {
            historyRegular -> reserve(numberOfSubIntervals+1);
            historyRegular -> push_back(startingPrice);
        }
        if (historyAntithetic != nullptr) {
            historyAntithetic -> reserve(numberOfSubIntervals+1);
            historyAntithetic -> push_back(startingPrice);
        }

        std::mt19937 randomNumberGenerator(seed);
        std::normal_distribution<double> zDist(0,1);

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
            futures.push_back(std::async(std::launch::async, eulerMaryama::simulateAntitheticVariatePaths,
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

    const double averagePayoff = totalPayoff/numberOfPaths;

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

    const double averagePayoff = totalPayoff/numberOfPaths;

    const double discountedPayoff = averagePayoff*std::exp(-riskFreeInterestRate*timeToExercise);

    return discountedPayoff;
}

int main() {
    std::cout << monteCarloCall(100, 100, 0.05, 0.02, 1, 1000) << std::endl;
    std::cout << blackScholesCall(100,100,0.05,0.02,1) << std::endl;
    std::cout << monteCarloPut(100, 100, 0.05, 0.02, 1, 100000) << std::endl;
    std::cout << blackScholesPut(100,100,0.05,0.02,1);
}

