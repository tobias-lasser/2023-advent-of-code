#include <format>
#include <fstream>
#include <iostream>
#include <map>
#include <print>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

auto parseInput(std::string_view fileName) {
    std::ifstream infile(fileName);
    if (!infile)
        throw std::runtime_error("Failed to open " + std::string(fileName));

    std::vector<std::string> results;
    while (!infile.eof()) {
        std::string line;
        std::getline(infile, line);

        if (line.empty())
            continue;
        results.push_back(std::move(line));
    }

    return results;
}

auto solvePart1(const std::vector<std::string> &input) {
    int result{0};

    for (const auto &line: input) {
        auto first = std::find_if(line.begin(), line.end(), [](char c) { return std::isdigit(c); });
        auto last = std::find_if(line.rbegin(), line.rend(), [](char c) { return std::isdigit(c); });
        if (first == line.end() || last == line.rend())
            throw std::runtime_error("Failed to parse line: " + line);
        result += static_cast<int>(*first - '0') * 10 + static_cast<int>(*last - '0');
    }

    return result;
}

int getDigit(std::string_view line, bool first) {
    static std::unordered_map<std::string, int> digitMap = {
            {"one", 1},
            {"two", 2},
            {"three", 3},
            {"four", 4},
            {"five", 5},
            {"six", 6},
            {"seven", 7},
            {"eight", 8},
            {"nine", 9}};
    std::map<unsigned, int> digitOccurrences;

    for (const auto &[str, num]: digitMap) {
        if (auto found = line.find(str); found != std::string::npos)
            digitOccurrences[found] = num;
        if (auto found = line.rfind(str); found != std::string::npos)
            digitOccurrences[found] = num;
    }

    auto isDigit = [](char c) { return std::isdigit(c); };
    if (auto itFirst = std::find_if(line.begin(), line.end(), isDigit); itFirst != line.end())
        digitOccurrences[std::distance(line.begin(), itFirst)] = static_cast<int>(*itFirst - '0');
    if (auto itLast = std::find_if(line.rbegin(), line.rend(), isDigit); itLast != line.rend())
        digitOccurrences[std::distance(itLast, line.rend()) - 1] = static_cast<int>(*itLast - '0');

    if (digitOccurrences.empty())
        throw std::runtime_error("No digit found.");
    return (first) ? digitOccurrences.begin()->second : digitOccurrences.rbegin()->second;
}

auto solvePart2(const std::vector<std::string> &input) {
    int result{0};

    for (const auto &line: input) {
        auto first = getDigit(line, true);
        auto second = getDigit(line, false);
        result += first * 10 + second;
    }

    return result;
}

int main() {
    std::println("AoC 2023: 01");

    try {
        auto input = parseInput("../inputs/01.txt");
        std::println("Part 1: {}", solvePart1(input));
        std::println("Part 2: {}", solvePart2(input));
    } catch (std::exception &e) {
        std::cerr << e.what() << "\n";
    }

    return 0;
}