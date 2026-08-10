#include <iostream>
#include <ostream>
#include <random>
#include <vector>
#include <future>

double simulatePath(const double startingPrice, const double drift, const double volatility, const double intervalLength, const int numberOfSubIntervals, const unsigned seed, std::vector<double>* history = nullptr) {
    if (history != nullptr) {
        history -> reserve(numberOfSubIntervals+1);
        history -> push_back(startingPrice);
    }

    std::mt19937 randomNumberGenerator(seed);
    std::normal_distribution<double> zDist(0,1);

    double Y = startingPrice;
    const double deltaTime = intervalLength/numberOfSubIntervals;


    for (int subinterval = 0; subinterval < numberOfSubIntervals; ++subinterval) {
        const double dWt = zDist(randomNumberGenerator)*std::sqrt(deltaTime);
        Y = Y*(1+drift*deltaTime+volatility*dWt);
        if (history != nullptr) history -> push_back(Y);
    }
    return Y;
}



std::vector<unsigned> generateSeeds(const unsigned int numberOfPaths) {
    std::random_device random_device;
    std::vector<unsigned> seeds;
    seeds.reserve(numberOfPaths);
    for (unsigned int i = 0; i < numberOfPaths; ++i) {
        seeds.push_back(random_device());
    }
    return seeds;
}

std::string getPriceHistories(const unsigned int numberOfPaths) {
    auto seeds = generateSeeds(numberOfPaths);

    std::vector<std::vector<double>> histories(numberOfPaths);
    std::vector<std::future<double>> futures;
    futures.reserve(numberOfPaths);
    for (int i = 0; i < numberOfPaths; ++i) {
        futures.push_back(std::async(std::launch::async, simulatePath, 100, 0.05, 0.2, 1, 365, seeds[i], &histories[i]));
    }

    std::string output;

    for (int i = 0; i < numberOfPaths; ++i) {
        futures[i].get();
        for (size_t j = 0; j < histories[i].size()-1; ++j) {
            const auto price = histories[i][j];
            output += std::to_string(price) +  ",";
        }
        output += std::to_string(histories[i].back()) + "\n";
    }
    return output;
}

std::vector<double> getTerminalPrices(const unsigned int numberOfPaths) {
    auto seeds = generateSeeds(numberOfPaths);

    std::vector<std::vector<double>> histories(numberOfPaths);
    std::vector<std::future<double>> futures;
    futures.reserve(numberOfPaths);
    for (int i = 0; i < numberOfPaths; ++i) {
        futures.push_back(std::async(std::launch::async, simulatePath, 100, 0.05, 0.2, 1, 365, seeds[i], nullptr));
    }

    std::vector<double> output;

    for (int i = 0; i < numberOfPaths; ++i) {
        output.push_back(futures[i].get());
    }
    return output;
}

double blackScholesCall(const double currentPrice, const double strikePrice, const double riskFreeInterestRate, const double volatility, const double timeToExcercise) {
    return 0;
}

int main() {
    std::cout << getPriceHistories(10) << std::endl;
}
