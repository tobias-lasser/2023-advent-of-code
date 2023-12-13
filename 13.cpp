#include <print>
#include <string>
#include <string_view>
#include <vector>
#include <fstream>
#include <format>

struct Pattern {
    std::vector<std::string> lines;

    [[nodiscard]] int getHorizontalReflection(int ignoreCandidate = -1) const { // brute force
        // find reflection line candidates
        std::vector<int> reflectionOptions;
        for (std::size_t i{0}; i < lines.size() - 1; ++i) {
            if (lines[i] == lines[i + 1] && i != ignoreCandidate)
                reflectionOptions.emplace_back(i);
        }

        // check reflection candidates
        for (const auto& refl : reflectionOptions) {
            bool isReflection{true};
            for (int i{0}; refl + i + 1 < lines.size() && refl - i >= 0; ++i) {
                if (lines[refl - i] != lines[refl + i + 1]) {
                    isReflection = false;
                    break;
                }
            }
            if (isReflection)
                return refl + 1;
        }
        return 0;
    }

    [[nodiscard]] int getVerticalReflection(int ignoreCandidate = -1) const {
        if (lines.empty()) return 0;

        Pattern transposed; // transpose of current pattern
        transposed.lines.reserve(lines.front().size());
        for (std::size_t col{0}; col < lines.front().size(); ++col) {
            std::string line;
            line.reserve(lines.size());
            for (std::size_t row{0}; row < lines.size(); ++row)
                line.push_back(lines[row][col]);
            transposed.lines.push_back(std::move(line));
        }

        return transposed.getHorizontalReflection(ignoreCandidate);
    }

    [[nodiscard]] int getValue() const {
        auto horiz = getHorizontalReflection();
        auto vert = (horiz == 0) ? getVerticalReflection() : 0;
        return vert + 100 * horiz;
    }

    enum class Direction { Horizontal, Vertical, Undefined };

    [[nodiscard]] int getValueIgnoring(int ignoreCandidate, Direction direction) const {
        auto horiz = getHorizontalReflection( (direction == Direction::Horizontal) ? ignoreCandidate : -1);
        auto vert = (horiz == 0) ? getVerticalReflection( (direction == Direction::Vertical) ? ignoreCandidate : -1) : 0;
        return vert + 100 * horiz;
    }

    [[nodiscard]] std::pair<int, Direction> getReflection() const {
        auto horiz = getHorizontalReflection();
        if (horiz != 0)
            return {horiz-1, Direction::Horizontal};
        auto vert = getVerticalReflection();
        if (vert != 0)
            return {vert-1, Direction::Vertical};
        return {0, Direction::Undefined};
    }
};

auto parseInput(std::string_view fileName) {
    std::ifstream infile(fileName);
    if (!infile)
        throw std::runtime_error("Could not open file " + std::string(fileName));

    std::vector<Pattern> patterns;
    Pattern pattern;
    while (!infile.eof()) {
        std::string line;
        std::getline(infile, line);

        if (line.empty()) {
            patterns.push_back(pattern);
            pattern.lines.clear();
        }
        else
            pattern.lines.push_back(std::move(line));
    }
    return patterns;
}

auto solvePart1(const std::vector<Pattern>& patterns) {
    int sum{0};
    for (const auto& pattern : patterns) {
        sum += pattern.getValue();
    }
    return sum;
}

auto solvePart2(const std::vector<Pattern>& patterns) {
    int sum{0};
    for (const auto& pattern : patterns) {
        // original value of pattern
        auto [originalReflection, direction] = pattern.getReflection();

        // now brute force the smudge
        Pattern unSmudged = pattern;
        for (std::size_t j{0}; j < unSmudged.lines.size(); ++j) {
            bool foundNew{false};
            for (std::size_t i{0}; i < unSmudged.lines[j].size(); ++i) {
                auto &c = unSmudged.lines[j][i];
                c = (c == '#') ? '.' : '#';
                auto newValue = unSmudged.getValueIgnoring(originalReflection, direction);
                if (newValue != 0) {
                    sum += newValue;
                    foundNew = true;
                    break;
                }
                c = (c == '#') ? '.' : '#';
            }
            if (foundNew) break;
        }
    }
    return sum;
}

int main() {
    std::println("AoC 2023: 13");

    try {
        auto patterns = parseInput("../inputs/13.txt");
        std::println("Part 1: {}", solvePart1(patterns));
        std::println("Part 2: {}", solvePart2(patterns));
    }
    catch (std::exception& e) {
        std::println("Exception: {}", e.what());
    }

    return 0;
}