#include <print>
#include <string>
#include <string_view>
#include <vector>
#include <fstream>
#include <unordered_map>
#include <map>
#include <ranges>
#include <algorithm>

enum class Direction { up, down, left, right };

struct DigInstruction {
    Direction direction{Direction::up};
    int steps{0};
    std::string color;
};

auto parseInput(std::string_view fileName) {
    std::ifstream infile(fileName);
    if (!infile)
        throw std::runtime_error("Could not open file " + std::string(fileName));

    std::vector<DigInstruction> instructions;
    std::unordered_map<char, Direction> directionMap{ {'L', Direction::left}, {'R', Direction::right}, {'U', Direction::up}, {'D', Direction::down} };
    while (!infile.eof()) {
        std::string line;
        std::getline(infile, line);
        if (line.empty()) continue;

        auto parts = line | std::views::split(' ') | std::ranges::to<std::vector<std::string>>();
        DigInstruction instruction;
        instruction.direction = directionMap[parts[0][0]];
        instruction.steps = std::stoi(std::string(parts[1]));
        instruction.color = parts[2].substr(1, parts[2].size() - 2);
        instructions.push_back(instruction);
    }

    return instructions;
}


struct Coord {
    long x{0}, y{0};
};

enum class Orientation { horizontal, vertical };

struct HoleSequence {
    Orientation orientation{Orientation::horizontal};
    Coord start, end;

    [[nodiscard]] bool contains(Coord c) const { return start.x <= c.x && c.x <= end.x && start.y <= c.y && c.y <= end.y; }
};


auto computeHoleSequence(const std::vector<DigInstruction>& instructions) {
    std::vector<HoleSequence> sequences;
    sequences.reserve(instructions.size());
    Coord currentCoord{0, 0};

    for (const auto& instruction: instructions) {
        HoleSequence hs;
        switch (instruction.direction) {
            case Direction::up:
                hs.orientation = Orientation::vertical;
                hs.start = {currentCoord.x, currentCoord.y - instruction.steps};
                hs.end = currentCoord;
                currentCoord = hs.start;
                break;
            case Direction::down:
                hs.orientation = Orientation::vertical;
                hs.start = currentCoord;
                hs.end = {currentCoord.x, currentCoord.y + instruction.steps};
                currentCoord = hs.end;
                break;
            case Direction::left:
                hs.orientation = Orientation::horizontal;
                hs.start = {currentCoord.x - instruction.steps, currentCoord.y};
                hs.end = currentCoord;
                currentCoord = hs.start;
                break;
            case Direction::right:
                hs.orientation = Orientation::horizontal;
                hs.start = currentCoord;
                hs.end = {currentCoord.x + instruction.steps, currentCoord.y};
                currentCoord = hs.end;
                break;
        }

        if (hs.orientation == Orientation::vertical)
            sequences.push_back(hs);
    }
    return sequences;
}

auto transformSequence(const std::vector<HoleSequence>& sequences) {
    std::map<long, std::vector<HoleSequence>> seqMap;
    for (const auto& seq : sequences) {
        for (auto y{seq.start.y}; y <= seq.end.y; ++y)
            seqMap[y].push_back(seq);
    }
    return seqMap;
}

auto digAndCount(std::map<long, std::vector<HoleSequence>>& seqMap) {
    long count{0};
    for (auto& [y, seqs] : seqMap) {
        bool insideTop{false}, insideBottom{false};
        std::sort(seqs.begin(), seqs.end(), [](const HoleSequence& a, const HoleSequence& b) { return a.start.x < b.start.x; });
        long lastX{std::numeric_limits<long>::max()};
        for (const auto& s : seqs) {
            if (!insideTop && !insideBottom)
                lastX = s.start.x;
            bool flipTop{false}, flipBottom{false};
            if (s.start.y < y) flipBottom = true;
            if (s.end.y > y) flipTop = true;

            if (insideTop || insideBottom || flipBottom || flipTop)
                if (s.start.x - lastX >= 1)
                    count += s.start.x - lastX - 1;
            insideTop ^= flipTop;
            insideBottom ^= flipBottom;
            lastX = s.start.x;
        }
        count += static_cast<long>(seqs.size());
    }

    return count;
}

auto fixInstructions(const std::vector<DigInstruction>& instructions) {
    std::vector<DigInstruction> newInstructions;
    newInstructions.reserve(instructions.size());

    std::unordered_map<char, Direction> newDirMap{ {'0', Direction::right}, {'1', Direction::down}, {'2', Direction::left}, {'3', Direction::up} };
    for (const auto& i : instructions) {
        DigInstruction newI = i;
        newI.direction = newDirMap[i.color[6]];
        newI.steps = std::stoi(i.color.substr(1, 5), nullptr, 16);
        newInstructions.push_back(newI);
    }
    return newInstructions;
}

auto solvePart1(const std::vector<DigInstruction>& instructions) {
    auto sequences = computeHoleSequence(instructions);
    auto seqMap = transformSequence(sequences);
    return digAndCount(seqMap);
}

auto solvePart2(const std::vector<DigInstruction>& instructions) {
    auto newInstructions = fixInstructions(instructions);
    auto sequences = computeHoleSequence(newInstructions);
    auto seqMap = transformSequence(sequences);
    return digAndCount(seqMap);
}

int main() {
    std::println("AoC 2023: 18");

    try {
        auto instructions = parseInput("../inputs/18.txt");
        std::println("Part 1: {}", solvePart1(instructions));
        std::println("Part 2: {}", solvePart2(instructions));
    }
    catch (std::exception& e) {
        std::println("Exception: {}", e.what());
    }

    return 0;
}