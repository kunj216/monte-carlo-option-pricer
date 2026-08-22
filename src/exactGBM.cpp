#include <future>
#include <thread>
#include <vector>
#include "exactGBM.hpp"
#include "util.hpp"

namespace exactGBM {

std::pair<double,double> analyticalTerminalAntitheticPrices(const double startingPrice, const double riskFreeRate,
    const double volatility, const double intervalLength, const unsigned seed) {

    std::mt19937 randomNumberGenerator(seed);
    std::normal_distribution<double> zDist(0,1);

    const double Wt = zDist(randomNumberGenerator) * std::sqrt(intervalLength);
    const double driftTerm = (riskFreeRate - 0.5 * volatility * volatility) * intervalLength;

    const double Y1 = startingPrice * std::exp(driftTerm + volatility * Wt);
    const double Y2 = startingPrice * std::exp(driftTerm - volatility * Wt);
    return {Y1, Y2};
}

std::pair<std::vector<double>, std::vector<double>> simulateAntitheticVariatePaths(
    const double startingPrice, const double riskFreeRate, const double volatility,
    const double intervalLength, const unsigned numberOfSubintervals, const unsigned seed) {

    std::mt19937 randomNumberGenerator(seed);
    std::normal_distribution<double> zDist(0,1);

    const double subintervalLength = intervalLength / numberOfSubintervals;
    const double sqrtDeltaT = std::sqrt(subintervalLength);
    const double driftTerm = (riskFreeRate - 0.5 * volatility * volatility) * subintervalLength;

    std::vector<double> regularPrices;
    std::vector<double> antitheticPrices;
    regularPrices.reserve(numberOfSubintervals + 1);
    antitheticPrices.reserve(numberOfSubintervals + 1);
    regularPrices.push_back(startingPrice);
    antitheticPrices.push_back(startingPrice);

    for (unsigned subinterval = 0; subinterval < numberOfSubintervals; ++subinterval) {
        const double dWt = zDist(randomNumberGenerator) * sqrtDeltaT;
        regularPrices.push_back(regularPrices.back() * std::exp(driftTerm + volatility * dWt));
        antitheticPrices.push_back(antitheticPrices.back() * std::exp(driftTerm - volatility * dWt));
    }
    return {regularPrices, antitheticPrices};
}

// Chunk workers — each runs a slice of path pairs on one thread.

static std::vector<double> simulateTerminalChunk(
    const double startingPrice, const double riskFreeRate,
    const double volatility, const double intervalLength,
    const std::vector<unsigned>& seeds) {

    std::vector<double> results;
    results.reserve(seeds.size() * 2);
    for (const unsigned seed : seeds) {
        auto [Y1, Y2] = analyticalTerminalAntitheticPrices(
            startingPrice, riskFreeRate, volatility, intervalLength, seed);
        results.push_back(Y1);
        results.push_back(Y2);
    }
    return results;
}

static std::vector<std::vector<double>> simulatePathChunk(
    const double startingPrice, const double riskFreeRate,
    const double volatility, const double intervalLength,
    const unsigned numberOfSubIntervals,
    const std::vector<unsigned>& seeds) {

    std::vector<std::vector<double>> results;
    results.reserve(seeds.size() * 2);
    for (const unsigned seed : seeds) {
        auto [regular, antithetic] = simulateAntitheticVariatePaths(
            startingPrice, riskFreeRate, volatility, intervalLength, numberOfSubIntervals, seed);
        results.push_back(std::move(regular));
        results.push_back(std::move(antithetic));
    }
    return results;
}

// Splits a seed vector into numThreads roughly equal slices and
// returns the slice for thread t.
static std::vector<unsigned> seedSlice(const std::vector<unsigned>& seeds,
    const unsigned int numThreads, const unsigned int t) {

    const unsigned int total     = seeds.size();
    const unsigned int chunkSize = total / numThreads;
    const unsigned int start     = t * chunkSize;
    const unsigned int end       = (t == numThreads - 1) ? total : start + chunkSize;
    return std::vector<unsigned>(seeds.begin() + start, seeds.begin() + end);
}

std::vector<std::vector<double>> getPriceHistories(unsigned int numberOfPaths, const double startingPrice,
    const double riskFreeRate, const double volatility, const double intervalLength,
    const unsigned numberOfSubIntervals) {

    if (numberOfPaths % 2 == 1) numberOfPaths--;
    const auto seeds = generateSeeds(numberOfPaths / 2);

    const unsigned int numThreads = std::min(
        std::thread::hardware_concurrency(),
        numberOfPaths / 2);

    std::vector<std::future<std::vector<std::vector<double>>>> futures;
    futures.reserve(numThreads);
    for (unsigned int t = 0; t < numThreads; ++t) {
        futures.push_back(std::async(std::launch::async, simulatePathChunk,
            startingPrice, riskFreeRate, volatility, intervalLength,
            numberOfSubIntervals, seedSlice(seeds, numThreads, t)));
    }

    std::vector<std::vector<double>> output;
    output.reserve(numberOfPaths);
    for (auto& future : futures) {
        auto chunk = future.get();
        for (auto& path : chunk) {
            output.push_back(std::move(path));
        }
    }
    return output;
}

std::vector<double> getTerminalPrices(unsigned int numberOfPaths, const double startingPrice,
    const double riskFreeRate, const double volatility, const double intervalLength) {

    if (numberOfPaths % 2 == 1) numberOfPaths--;
    const auto seeds = generateSeeds(numberOfPaths / 2);

    const unsigned int numThreads = std::min(
        std::thread::hardware_concurrency(),
        numberOfPaths / 2);

    std::vector<std::future<std::vector<double>>> futures;
    futures.reserve(numThreads);
    for (unsigned int t = 0; t < numThreads; ++t) {
        futures.push_back(std::async(std::launch::async, simulateTerminalChunk,
            startingPrice, riskFreeRate, volatility, intervalLength,
            seedSlice(seeds, numThreads, t)));
    }

    std::vector<double> output;
    output.reserve(numberOfPaths);
    for (auto& future : futures) {
        auto chunk = future.get();
        output.insert(output.end(), chunk.begin(), chunk.end());
    }
    return output;
}

} // namespace exactGBM