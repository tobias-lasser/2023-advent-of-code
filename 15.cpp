#include <print>
#include <string>
#include <string_view>
#include <vector>
#include <fstream>
#include <ranges>
#include <array>
#include <list>

enum class Operation { remove, assign, undefined };

struct Lens {
    std::string label;
    Operation op{Operation::undefined};
    int focalLen{0};
};

auto parseInput(std::string_view fileName) {
    std::ifstream infile(fileName);
    if (!infile)
        throw std::runtime_error("Could not open file " + std::string(fileName));

    std::string line;
    std::getline(infile, line);

    std::vector<std::string> initSequence;
    initSequence = line | std::views::split(',') | std::ranges::to<std::vector<std::string>>();
    return initSequence;
}

long computeHash(std::string_view str) {
    long hash{0};
    for (const char& c : str) {
        hash += static_cast<long>(c);
        hash *= 17;
        hash %= 256;
    }
    return hash;
}

auto solvePart1(const std::vector<std::string>& initSequence) {
    long hashSum{0};
    for (const auto& s : initSequence)
        hashSum += computeHash(s);
    return hashSum;
}


auto parseSequence(const std::vector<std::string>& initSequence) {
    std::vector<Lens> lensSequence;
    lensSequence.reserve(initSequence.size());
    for (const auto& s : initSequence) {
        Lens l;
        if (auto itEqual = s.find('='); itEqual != std::string::npos) {
            l.label = s.substr(0, itEqual);
            l.op = Operation::assign;
            l.focalLen = std::stoi(s.substr(itEqual + 1));
        }
        if (auto itMinus = s.find('-'); itMinus != std::string::npos) {
            l.label = s.substr(0, itMinus);
            l.op = Operation::remove;
        }
        lensSequence.push_back(std::move(l));
    }
    return lensSequence;
}

auto processBoxes(const std::vector<Lens>& lensSequence) {
    std::array<std::list<Lens>, 256> boxes;
    for (const auto& l : lensSequence) {
        auto boxId = computeHash(l.label) % 256;
        switch (l.op) {
            case Operation::remove:
                boxes[boxId].remove_if([&l](const Lens& lens) { return l.label == lens.label; });
                break;
            case Operation::assign:
            {
                auto it = std::find_if(boxes[boxId].begin(), boxes[boxId].end(), [&l](const Lens& lens) { return l.label == lens.label; });
                if (it == boxes[boxId].end())
                    boxes[boxId].push_back(l);
                else *it = l;
            }
            break;
            default: break;
        }
    }
    return boxes;
}

auto solvePart2(const std::vector<std::string>& initSequence) {
    auto lensSequence = parseSequence(initSequence);
    auto boxes = processBoxes(lensSequence);

    long focusingPower{0};
    for (int i{0}; i < 256; ++i) {
        int slot{1};
        for (const auto& lens : boxes[i])
            focusingPower += (i+1) * slot++ * lens.focalLen;
    }

    return focusingPower;
}

int main() {
    std::println("AoC 2023: 15");

    try {
        auto initSequence = parseInput("../inputs/15.txt");
        std::println("Part 1: {}", solvePart1(initSequence));
        std::println("Part 2: {}", solvePart2(initSequence));
    }
    catch (std::exception& e) {
        std::println("Exception: {}", e.what());
    }

    return 0;
}