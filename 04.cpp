#include <format>
#include <fstream>
#include <print>
#include <ranges>
#include <string>
#include <string_view>
#include <unordered_set>
#include <vector>

struct ScratchCard {
    std::unordered_set<int> winningNumbers;
    std::vector<int> numbers;
    int count{1};
};

auto splitString(std::string_view sv, std::string_view sep) {
    auto parts = std::views::split(sv, sep);
    auto partsVec = std::ranges::to<std::vector<std::string>>(parts);
    return partsVec;
}

auto parseInput(std::string_view filename) {
    std::ifstream infile(filename);
    if (!infile)
        throw std::runtime_error("Could not open file " + std::string(filename));

    std::vector<ScratchCard> cards;
    while (!infile.eof()) {
        std::string line;
        std::getline(infile, line);
        if (line.empty()) continue;

        ScratchCard card;
        std::string_view lv{line};
        lv.remove_prefix(lv.find(": ") + 1);
        auto parts = splitString(lv, " | ");
        if (parts.size() != 2) continue;
        auto winNums = splitString(parts[0], " ");
        for (const auto &num: winNums) {
            if (num.empty()) continue;
            card.winningNumbers.insert(std::stoi(num));
        }

        auto numbers = splitString(parts[1], " ");
        for (const auto &num: numbers) {
            if (num.empty()) continue;
            card.numbers.push_back(std::stoi(num));
        }

        cards.push_back(std::move(card));
    }

    return cards;
}

auto solvePart1(const std::vector<ScratchCard> &cards) {
    int points{0};
    for (const auto &card: cards) {
        int score{0};
        for (const auto &num: card.numbers) {
            if (card.winningNumbers.contains(num)) {
                if (score != 0) score *= 2;
                else
                    score = 1;
            }
        }
        points += score;
    }

    return points;
}

auto getNumberOfMatchingNumbers(const ScratchCard &card) {
    int noMatching{0};
    for (const auto &num: card.numbers) {
        if (card.winningNumbers.contains(num))
            ++noMatching;
    }
    return noMatching;
}


auto solvePart2(std::vector<ScratchCard> &cards) {
    for (std::size_t i{0}; i < cards.size(); ++i) {
        auto matches = getNumberOfMatchingNumbers(cards[i]);
        for (std::size_t m{1}; m <= matches; ++m) {
            if (i + m < cards.size())
                cards[i + m].count += cards[i].count;
        }
    }

    int noCards{0};
    for (const auto &card: cards)
        noCards += card.count;
    return noCards;
}

int main() {
    std::println("AoC 2023: 04");

    try {
        auto cards = parseInput("../inputs/04.txt");
        std::println("Part 1: {}", solvePart1(cards));
        std::println("Part 2: {}", solvePart2(cards));
    } catch (std::exception &e) {
        std::println("Exception: {}", e.what());
    }

    return 0;
}