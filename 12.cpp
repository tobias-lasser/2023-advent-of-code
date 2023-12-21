#include <print>
#include <string>
#include <vector>
#include <string_view>
#include <fstream>
#include <ranges>
#include <charconv>
#include <format>
#include <map>

struct Springs {
    std::string condition;
    std::vector<int> damagedGroups;

    [[nodiscard]] bool valid(std::string_view arrangement) const {
        for (const auto i : damagedGroups) {
            std::string group(i, '#');
            auto pos = arrangement.find(group);
            if (pos == std::string::npos) return false;
            arrangement.remove_prefix(pos + group.size());
            if (!arrangement.empty() && arrangement[0] != '.') return false;
        }
        return true;
    }

    enum class DFAstates { okSprings, damagedSprings, damagedSpringsEnd, end };

    [[nodiscard]] auto getDFA() const {
        std::map<int, DFAstates> dfa;
        int i{0};
        for (const auto group : damagedGroups) {
            dfa[i++] = DFAstates::okSprings;
            for (int j{1}; j < group; ++j)
                dfa[i++] = DFAstates::damagedSprings;
            dfa[i++] = DFAstates::damagedSpringsEnd;
        }
        dfa[i-1] = DFAstates::end;
        return dfa;
    }

    [[nodiscard]] long long getNoArrangements() const {
        // DFA solution thanks to Reddit...
        auto dfa = getDFA();

        std::map<int, long long> states{ {0, 1} };
        for (const auto c : condition) {
            std::map<int, long long> newStates;
            for (const auto& [state, number] : states) {
                switch (dfa[state]) {
                    case DFAstates::okSprings:
                        if (c == '?') {
                            newStates[state] += number;
                            newStates[state+1] += number;
                        }
                        else if (c == '.')
                            newStates[state] += number;
                        else if (c == '#')
                            newStates[state+1] += number;
                        break;
                    case DFAstates::damagedSprings:
                        if (c == '?')
                            newStates[state+1] += number;
                        else if (c == '#')
                            newStates[state+1] += number;
                        break;
                    case DFAstates::damagedSpringsEnd:
                        if (c == '?')
                            newStates[state+1] += number;
                        else if (c == '.')
                            newStates[state+1] += number;
                        break;
                    case DFAstates::end:
                        if (c == '?')
                            newStates[state] += number;
                        else if (c == '.')
                            newStates[state] += number;
                        break;
                }
            }
            std::swap(states, newStates);
        }

        return states[static_cast<int>(dfa.size())  - 1];
    }

    void unfold() {
        auto origCondition = condition;
        for (int i{0}; i < 4; ++i)
            condition += '?' + origCondition;

        auto origGroups = damagedGroups;
        for (int i{0}; i < 4; ++i)
            damagedGroups.insert(damagedGroups.end(), origGroups.begin(), origGroups.end());
    }
};

auto splitIntString(std::string_view sv, std::string_view sep) {
    return sv | std::views::split(sep) | std::views::filter([](auto v) { return !v.empty(); })
           | std::views::transform([](auto v) { int i{0}; std::from_chars(v.data(), v.data() + v.size(), i); return i; })
           | std::ranges::to<std::vector<int>>();
}

auto parseInput(std::string_view fileName) {
    std::ifstream infile(fileName);
    if (!infile)
        throw std::runtime_error("Could not open file " + std::string(fileName));

    std::vector<Springs> listOfSprings;
    while (!infile.eof()) {
        std::string line;
        std::getline(infile, line);
        if (line.empty()) continue;

        auto parts = line | std::views::split(' ') | std::ranges::to<std::vector<std::string>>();
        if (parts.size() != 2) continue;

        Springs springs;
        springs.condition = parts[0];
        springs.damagedGroups = splitIntString(parts[1], ",");
        listOfSprings.push_back(std::move(springs));
    }

    return listOfSprings;
}

auto solvePart1(const std::vector<Springs>& listOfSprings) {
    long long noArrangements{0};
    for (const auto& springs : listOfSprings)
        noArrangements += springs.getNoArrangements();

    return noArrangements;
}

auto solvePart2(std::vector<Springs> listOfSprings) {
    long long noArrangements{0};
    for (auto& springs : listOfSprings) {
        springs.unfold();
        noArrangements += springs.getNoArrangements();
    }

    return noArrangements;
}

int main() {
    std::println("AoC 2023: 12");

    try {
        auto springs = parseInput("../inputs/12.txt");
        std::println("Part 1: {}", solvePart1(springs));
        std::println("Part 2: {}", solvePart2(springs));
    }
    catch (std::exception& e) {
        std::println("Exception: {}", e.what());
    }

    return 0;
}