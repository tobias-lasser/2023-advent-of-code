#include <algorithm>
#include <fstream>
#include <map>
#include <print>
#include <ranges>
#include <set>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

enum Card { joker,
            two,
            three,
            four,
            five,
            six,
            seven,
            eight,
            nine,
            ten,
            jack,
            queen,
            king,
            ace };
enum Type { highCard,
            onePair,
            twoPair,
            threeOfAKind,
            fullHouse,
            fourOfAKind,
            fiveOfAKind };

struct Hand {
    std::array<Card, 5> cards;
    Type type;
    int bid;

    Hand(std::string_view line) {
        using enum Card;
        std::unordered_map<char, Card> c2c{{'2', two}, {'3', three}, {'4', four}, {'5', five}, {'6', six}, {'7', seven}, {'8', eight}, {'9', nine}, {'T', ten}, {'J', jack}, {'Q', queen}, {'K', king}, {'A', ace}};
        for (std::size_t i{0}; i < 5; ++i)
            cards[i] = c2c[line[i]];

        bid = std::stoi(std::string(line.substr(5)));

        type = determineType();
    }

    Type determineType(bool withJokers = false) {
        std::map<Card, int> numbers;
        for (auto &c: cards)
            numbers[c]++;

        std::multiset<int> types;
        for (auto &[card, num]: numbers) {
            if (withJokers && card == Card::joker)
                continue;
            types.insert(num);
        }

        Type t;
        using enum Type;
        if (types.empty())
            t = Type::highCard;
        else {
            auto typesIt = types.crbegin();
            switch (*typesIt) {
                case 5:
                    t = fiveOfAKind;
                    break;
                case 4:
                    t = fourOfAKind;
                    break;
                case 3:
                    t = threeOfAKind;
                    break;
                case 2:
                    t = onePair;
                    break;
                default:
                    t = highCard;
                    break;
            }

            if (typesIt != types.crend()) {
                ++typesIt;
                if (t == onePair && *typesIt == 2) t = twoPair;
                if (t == threeOfAKind && *typesIt == 2) t = fullHouse;
            }
        }

        if (withJokers) {
            int countJokers = numbers[Card::joker];

            if (countJokers >= 4) t = fiveOfAKind;
            if (countJokers == 3) {
                if (t >= onePair) t = fiveOfAKind;
                else
                    t = fourOfAKind;
            }
            if (countJokers == 2) {
                if (t >= threeOfAKind) t = fiveOfAKind;
                else if (t >= onePair)
                    t = fourOfAKind;
                else
                    t = threeOfAKind;
            }
            if (countJokers == 1) {
                if (t >= fourOfAKind) t = fiveOfAKind;
                else if (t >= threeOfAKind)
                    t = fourOfAKind;
                else if (t == twoPair)
                    t = fullHouse;
                else if (t == onePair)
                    t = threeOfAKind;
                else
                    t = onePair;
            }
        }

        return t;
    }
};

bool operator<(const Hand &lhs, const Hand &rhs) {
    if (lhs.type != rhs.type)
        return lhs.type < rhs.type;
    for (std::size_t i{0}; i < 5; ++i) {
        if (lhs.cards[i] != rhs.cards[i])
            return lhs.cards[i] < rhs.cards[i];
    }
    return false;
}

auto parseInput(std::string_view filename) {
    std::ifstream infile(filename);
    if (!infile)
        throw std::runtime_error("Could not open file " + std::string(filename));

    std::vector<Hand> hands;
    while (!infile.eof()) {
        std::string line;
        std::getline(infile, line);
        if (line.empty())
            continue;

        hands.emplace_back(line);
    }
    return hands;
}


auto solvePart1(std::vector<Hand> &hands) {
    std::sort(hands.begin(), hands.end());

    unsigned winnings{0};
    for (std::size_t i{0}; i < hands.size(); ++i) {
        winnings += (i + 1) * hands[i].bid;
    }

    return winnings;
}

auto solvePart2(std::vector<Hand> &hands) {
    for (auto &h: hands) {
        for (auto &c: h.cards) {
            if (c == Card::jack)
                c = Card::joker;
        }
        h.type = h.determineType(true);
    }

    return solvePart1(hands);
}

int main() {
    std::println("AoC 2023: 07");

    try {
        auto hands = parseInput("../inputs/07.txt");
        std::println("Part 1: {}", solvePart1(hands));
        std::println("Part 2: {}", solvePart2(hands));
    } catch (std::exception &e) {
        std::println("Exception: {}", e.what());
    }

    return 0;
}