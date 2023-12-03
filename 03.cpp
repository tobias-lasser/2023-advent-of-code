#include <cctype>
#include <fstream>
#include <print>
#include <string>
#include <string_view>
#include <unordered_set>
#include <utility>
#include <vector>

auto parseInput(std::string_view filename) {
    std::ifstream infile(filename);
    if (!infile)
        throw std::runtime_error("Could not read file " + std::string(filename));

    std::vector<std::string> schematic;
    while (!infile.eof()) {
        std::string line;
        std::getline(infile, line);
        if (line.empty()) continue;

        schematic.push_back(line);
    }

    return schematic;
}

auto solvePart1(const std::vector<std::string> &schematic) {
    int sumNumbers{0};

    for (std::size_t i{0}; i < schematic.size(); ++i) {
        for (std::size_t l{0}; l < schematic[i].size(); ++l) {
            if (!std::isdigit(schematic[i][l])) continue;
            std::size_t len{0};
            int number = std::stoi(schematic[i].substr(l), &len);

            // check if number has adjacent symbol
            bool hasSymbol{false};
            for (std::size_t itest{(i == 0) ? 0uz : i - 1}; itest <= std::min(i + 1uz, schematic.size() - 1); ++itest) {
                for (std::size_t ltest{(l == 0) ? 0uz : l - 1}; ltest <= std::min(l + len, schematic[i].size() - 1); ++ltest) {
                    auto c = schematic[itest][ltest];
                    if (c != '.' && !std::isdigit(c)) {
                        hasSymbol = true;
                        break;
                    }
                }
                if (hasSymbol) break;
            }
            if (hasSymbol)
                sumNumbers += number;

            l += len;
        }
    }

    return sumNumbers;
}

template<>
struct std::hash<std::pair<std::size_t, std::size_t>> {
    std::size_t operator()(const std::pair<std::size_t, std::size_t> &c) const noexcept {
        return c.first * 31 + c.second;
    }
};

auto getCandidates(const std::vector<std::string> &schematic, std::size_t i, std::size_t l) {
    std::unordered_set<std::pair<std::size_t, std::size_t>> candidates;
    std::vector<std::size_t> rows{i};
    if (i > 0) rows.push_back(i - 1);
    if (i + 1 < schematic.size()) rows.push_back(i + 1);

    for (const auto row: rows) {
        if (std::isdigit(schematic[row][l]))
            candidates.emplace(row, l);
        else {
            if (l > 0 && std::isdigit(schematic[row][l - 1]))
                candidates.emplace(row, l - 1);
            if (l + 1 < schematic[row].size() && std::isdigit(schematic[row][l + 1]))
                candidates.emplace(row, l + 1);
        }
    }

    return candidates;
}

auto getNumber(const std::vector<std::string> &schematic, std::size_t row, std::size_t col) {
    while (col > 0 && std::isdigit(schematic[row][col - 1]))
        --col;
    return std::stoi(schematic[row].substr(col));
}

auto solvePart2(const std::vector<std::string> &schematic) {
    int sumGearRatios{0};

    for (std::size_t i{0}; i < schematic.size(); ++i) {
        for (std::size_t l{0}; l < schematic[i].size(); ++l) {
            if (schematic[i][l] != '*') continue;
            auto candidates = getCandidates(schematic, i, l);
            if (candidates.size() != 2) continue;

            int product{1};
            for (const auto &[row, col]: candidates) {
                product *= getNumber(schematic, row, col);
            }
            sumGearRatios += product;
        }
    }

    return sumGearRatios;
}

int main() {
    std::println("AoC 2023: 03");

    try {
        auto schematic = parseInput("../inputs/03.txt");
        std::println("Part 1: {}", solvePart1(schematic));
        std::println("Part 2: {}", solvePart2(schematic));
    } catch (std::exception &e) {
        std::println("Exception: {}", e.what());
    }

    return 0;
}