#include <print>
#include <string>
#include <string_view>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <numeric>

struct Node {
    std::string left, right;
};

struct Map {
    std::string instructions;
    std::unordered_map<std::string, Node> nodes;
};

auto parseInput(std::string_view fileName) {
    std::ifstream infile(fileName);
    if (!infile)
        throw std::runtime_error("Could not open file " + std::string(fileName));

    Map map;
    std::string line;
    std::getline(infile, line);
    map.instructions = std::move(line);

    while (!infile.eof()) {
        std::getline(infile, line);
        if (line.empty()) continue;

        Node node{line.substr(7, 3), line.substr(12, 3)}; // hardcoded, boo...
        map.nodes[line.substr(0, 3)] = std::move(node);
    }

    return map;
}

auto solvePart1(Map& map) {
    int noSteps{0};
    std::string currentNode{"AAA"};
    for (int i{0}; i < static_cast<int>(map.instructions.size()); ++i) {
        ++noSteps;
        if (map.instructions[i] == 'L')
            currentNode = map.nodes[currentNode].left;
        else
            currentNode = map.nodes[currentNode].right;
        if (currentNode == "ZZZ")
            break;
        if (i == map.instructions.size() - 1) // wrap around instructions
            i = -1;
    }

    return noSteps;
}

std::vector<std::string> findStartNodes(const Map& map) {
    std::vector<std::string> startNodes;
    for (const auto& [key, node]: map.nodes) {
        if (key.size() == 3 && key[2] == 'A')
            startNodes.push_back(key);
    }
    return startNodes;
}

auto getCycleLength(std::string startNode, Map& map) {
    std::string currentNode{std::move(startNode)};
    unsigned instructionIndex{0};
    // find end node with ??Z
    while (currentNode.size() == 3 && currentNode[2] != 'Z') {
        if (map.instructions[instructionIndex] == 'L')
            currentNode = map.nodes[currentNode].left;
        else
            currentNode = map.nodes[currentNode].right;
        instructionIndex = (instructionIndex + 1) % map.instructions.size();
    }
    std::string endNode{currentNode};

    // count number of steps to get to that end node again
    int noSteps{0};
    while (currentNode != endNode || noSteps == 0) {
        ++noSteps;
        if (map.instructions[instructionIndex] == 'L')
            currentNode = map.nodes[currentNode].left;
        else
            currentNode = map.nodes[currentNode].right;
        instructionIndex = (instructionIndex + 1) % map.instructions.size();
    }

    return noSteps;
}

auto lcm(const std::vector<int>& numbers) {
    if (numbers.size() < 2) return 0l;
    long lcm = std::lcm(numbers[0], numbers[1]);
    for (std::size_t i{2}; i < numbers.size(); ++i)
        lcm = std::lcm(lcm, numbers[i]);
    return lcm;
}

auto solvePart2(Map& map) {
    auto currentNodes = findStartNodes(map);
    std::vector<int> cycleLengths;
    cycleLengths.reserve(currentNodes.size());
    for (const auto& node : currentNodes)
        cycleLengths.emplace_back(getCycleLength(node, map));

    return lcm(cycleLengths);
}

int main() {
    std::println("AoC 2023: 08");

    try {
        auto map = parseInput("../inputs/08.txt");
        std::println("Part 1: {}", solvePart1(map));
        std::println("Part 2: {}", solvePart2(map));
    }
    catch (std::exception& e) {
        std::println("Exception: {}", e.what());
    }

    return 0;
}

