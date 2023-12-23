#include <print>
#include <vector>
#include <string>
#include <string_view>
#include <fstream>

struct Coord {
    int x{0}, y{0};
};

auto parseInput(std::string_view fileName) {
    std::ifstream infile(fileName);
    if (!infile)
        throw std::runtime_error("Could not open file " + std::string(fileName));

    std::vector<std::string> map;
    while (!infile.eof()) {
        std::string line;
        std::getline(infile, line);
        if (line.empty()) continue;
        map.push_back(std::move(line));
    }

    return map;
}

void findPath(std::vector<std::string>& map, Coord pos, int pathLength, std::vector<int>& pathLengths) {
    if (pos.y == map.size() - 1) {
        pathLengths.push_back(pathLength);
        return;
    }

    if (pos.x > 0 && (map[pos.y][pos.x-1] == '.' || map[pos.y][pos.x-1] == '<')) {
        auto save = map[pos.y][pos.x-1];
        map[pos.y][pos.x-1] = 'O';
        findPath(map, Coord{pos.x-1, pos.y}, pathLength + 1, pathLengths);
        map[pos.y][pos.x-1] = save;
    }
    if (pos.x < map[pos.y].size()-1 && (map[pos.y][pos.x+1] == '.' || map[pos.y][pos.x+1] == '>')) {
        auto save = map[pos.y][pos.x+1];
        map[pos.y][pos.x+1] = 'O';
        findPath(map, Coord{pos.x+1, pos.y}, pathLength + 1, pathLengths);
        map[pos.y][pos.x+1] = save;
    }
    if (pos.y > 0 && (map[pos.y-1][pos.x] == '.' || map[pos.y-1][pos.x] == '^')) {
        auto save = map[pos.y-1][pos.x];
        map[pos.y-1][pos.x] = 'O';
        findPath(map, Coord{pos.x, pos.y-1}, pathLength + 1, pathLengths);
        map[pos.y-1][pos.x] = save;
    }
    if (pos.y < map.size()-1 && (map[pos.y+1][pos.x] == '.' || map[pos.y+1][pos.x] == 'v')) {
        auto save = map[pos.y+1][pos.x];
        map[pos.y+1][pos.x] = 'O';
        findPath(map, Coord{pos.x, pos.y+1}, pathLength + 1, pathLengths);
        map[pos.y+1][pos.x] = save;
    }
}

auto solvePart1(std::vector<std::string> map) {
    if (map.empty()) return -1;

    auto pos = map.front().find('.');
    if (pos == std::string::npos)
        throw std::runtime_error("Could not find start position");
    Coord start{static_cast<int>(pos), 0};
    map[start.y][start.x] = 'O';

    std::vector<int> pathLengths;
    findPath(map, start, 0, pathLengths);
    return *std::max_element(pathLengths.begin(), pathLengths.end());
}

auto solvePart2(std::vector<std::string> map) {
    if (map.empty()) return -1;

    auto pos = map.front().find('.');
    if (pos == std::string::npos)
        throw std::runtime_error("Could not find start position");
    Coord start{static_cast<int>(pos), 0};
    map[start.y][start.x] = 'O';

    for (auto& s : map) {
        for (auto& c : s)
            if (c != '#') c = '.';
    }

    std::vector<int> pathLengths;
    findPath(map, start, 0, pathLengths);
    return *std::max_element(pathLengths.begin(), pathLengths.end());
}

int main() {
    std::println("AoC 2023: 23");

    try {
        auto map = parseInput("../inputs/23.txt");
        std::println("Part 1: {}", solvePart1(map));
        std::println("Part 2: {}", solvePart2(map));
    }
    catch (std::exception& e) {
        std::println("Exception: {}", e.what());
    }

    return 0;
}
