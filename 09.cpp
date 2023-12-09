#include <print>
#include <ranges>
#include <vector>
#include <string>
#include <string_view>
#include <charconv>
#include <fstream>
#include <numeric>
#include <algorithm>

auto splitIntString(std::string_view sv, std::string_view sep) {
    return sv | std::views::split(sep) | std::views::filter([](auto v) { return !v.empty(); })
              | std::views::transform([](auto v) { long i{0}; std::from_chars(v.data(), v.data() + v.size(), i); return i; })
              | std::ranges::to<std::vector<long>>();
}

auto parseInput(std::string_view fileName) {
    std::ifstream infile(fileName);
    if (!infile)
        throw std::runtime_error("Could not open file " + std::string(fileName));

    std::vector<std::vector<long>> sequences;
    while (!infile.eof()) {
        std::string line;
        std::getline(infile, line);
        if (line.empty()) continue;

        sequences.push_back(splitIntString(line, " "));
    }

    return sequences;
}

auto predictSequence(const std::vector<long>& sequence, bool end = true) {
    std::vector<long> currentSequence, nextSequence{sequence};
    std::vector<long> startNumbers, endNumbers;
    startNumbers.emplace_back(nextSequence.front());
    endNumbers.emplace_back(nextSequence.back());

    do {
        currentSequence = nextSequence;
        nextSequence.clear();
        nextSequence.reserve(currentSequence.size()-1);
        for (std::size_t i{0}; i < currentSequence.size() - 1; ++i)
            nextSequence.emplace_back(currentSequence[i+1] - currentSequence[i]);
        endNumbers.emplace_back(nextSequence.back());
        startNumbers.emplace_back(nextSequence.front());
    } while (std::accumulate(nextSequence.begin(), nextSequence.end(), 0l,
             [](auto a, auto b) { return std::abs(a) + std::abs(b); }) > 0l);

    startNumbers.pop_back();
    endNumbers.pop_back();
    std::ranges::reverse(startNumbers);
    std::ranges::reverse(endNumbers);

    long prediction{0};
    for (const auto &number: (end) ? endNumbers : startNumbers) {
        prediction = number + ((end) ? prediction : -prediction);
    }

    return prediction;
}

auto solvePart1and2(const std::vector<std::vector<long>>& sequences, bool part2 = false) {
    long sum{0};
    for (const auto& sequence : sequences)
        sum += predictSequence(sequence, !part2);
    return sum;
}

int main() {
    std::println("AoC 2023: 09");

    try {
        auto sequences = parseInput("../inputs/09.txt");
        std::println("Part 1: {}", solvePart1and2(sequences));
        std::println("Part 2: {}", solvePart1and2(sequences, true));
    }
    catch (std::exception& e) {
        std::println("Exception: {}", e.what());
    }

    return 0;
}