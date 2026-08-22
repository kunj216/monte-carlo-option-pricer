#include <fstream>
#include <iostream>
#include <map>
#include <ostream>

#include "util.hpp"
#include "eulerMaruyama.hpp"
#include "exactGBM.hpp"
#include "pricer.hpp"

/*
 * --spot currentPrice default 100
 * --vol volatility default 0.2
 * --strike strikePrice default 100
 * --expiry timeToExercise default 1
 * --rate riskFreeInterestRate default 0.05
 * --paths numberOfPaths default 1000
 */

std::map<std::string, double> parseFlags(const int argc, char *argv[], const int startIndex) {
    std::map<std::string, double> output;
    for (int i = startIndex; i + 1 < argc; i+=2) {
        output[argv[i]] = std::stod(argv[i+1]);
    }
    return output;
}

static double getFlag(const std::map<std::string, double>& flags, const std::string& key, const double defaultValue) {
    auto iterator = flags.find(key);

    if (iterator == flags.end()) return defaultValue;

    return iterator->second;
}

void price(const std::map<std::string, double> & flags) {
    const double currentPrice = getFlag(flags, "--spot", 100);
    const double strikePrice = getFlag(flags, "--strike", 100);
    const double volatility = getFlag(flags, "--vol", 0.2);
    const double timeToExercise = getFlag(flags, "--expiry", 1);
    const double riskFreeInterestRate = getFlag(flags, "--rate", 0.05);
    const unsigned numberOfPaths = static_cast<unsigned>(getFlag(flags, "--paths", 1000));

    auto [monteCarloCallPrice, monteCarloPutPrice] = monteCarloBoth(currentPrice, strikePrice,
        riskFreeInterestRate, volatility, timeToExercise, numberOfPaths);

    const double blackScholesCallPrice = blackScholesCall(currentPrice, strikePrice, riskFreeInterestRate,
        volatility, timeToExercise);
    const double blackScholesPutPrice = blackScholesPut(currentPrice, strikePrice, riskFreeInterestRate,
        volatility, timeToExercise);

    std::cout << "Black-Scholes Call Price: " << blackScholesCallPrice << std::endl;

    std::cout << "Monte Carlo Call Price: " << monteCarloCallPrice << std::endl;

    std::cout << "Relative error: " << std::abs((monteCarloCallPrice-blackScholesCallPrice)/blackScholesCallPrice)*100 << "%";

    std::cout << "\n\n";

    std::cout << "Black-Scholes Put Price: " << blackScholesPutPrice  << std::endl;

    std::cout << "Monte Carlo Put Price: " << monteCarloPutPrice << std::endl;

    std::cout << "Relative error: " << std::abs((monteCarloPutPrice-blackScholesPutPrice)/blackScholesPutPrice)*100 << "%";

}

static void path(const std::map<std::string, double>& flags) {
    const double currentPrice = getFlag(flags, "--spot", 100);
    const double volatility = getFlag(flags, "--vol", 0.2);
    const double timeToExercise = getFlag(flags, "--expiry", 1);
    const double riskFreeInterestRate = getFlag(flags, "--rate", 0.05);
    const unsigned numberOfPaths = static_cast<unsigned>(getFlag(flags, "--paths", 1000));

    std::vector<std::vector<double>> paths = exactGBM::getPriceHistories(numberOfPaths, currentPrice,
        riskFreeInterestRate, volatility, timeToExercise, timeToExercise*365);

    std::ofstream file ("/tmp/WienerSimulationPaths.csv");

    for (const std::vector<double>& priceHistory : paths) {
        for (int i = 0; i < priceHistory.size()-1; ++i) {
            file << priceHistory[i] << ",";
        }
        file << priceHistory.back() << std::endl;
    }
    std::cout << "Outputted paths CSV to /tmp/WienerSimulationPaths.csv";
}

int main(const int argc, char* argv[]) { //argv is an array of pointers to null-terminated c-style strings
    if (argc <2) {
        std::cout << "Usage: Wiener Simulation <mode>" << std::endl;
        return 1;
    }
    std::string mode = argv[1];

    auto flags = parseFlags(argc,argv, 2);

    if (mode == "price") price(flags);
    else if (mode =="path") path(flags);

    else {
        std::cout << mode << " is not a valid mode.";
        return 1;
    }

    return 0;
}