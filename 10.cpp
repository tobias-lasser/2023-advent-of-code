#include <print>
#include <vector>
#include <fstream>
#include <string_view>
#include <unordered_map>
#include <array>
#include <stack>
#include <set>

struct Coord {
    int x, y;
    bool operator==(const Coord& other) const { return (x == other.x) && (y == other.y); }
};

Coord operator+(const Coord& a, const Coord& b) {
    return Coord{a.x + b.x, a.y + b.y};
}

bool operator<(const Coord& a, const Coord& b) {
    return a.x < b.x || (a.x == b.x && a.y < b.y);
}

static std::vector<Coord> neighbors{ {0, -1}, {-1, 0}, {1, 0}, {0, 1} };
static std::unordered_map<char, std::array<std::string, 4>> mapNeighbors {
        {'|', { "|F7", "", "", "|LJ" } },
        {'-', { "", "-LF", "-J7", "" } },
        {'L', { "|F7", "", "-J7", "" } },
        {'J', { "|F7", "-LF", "", "" } },
        {'7', { "", "-LF", "", "|LJ" } },
        {'F', { "", "", "-J7", "|LJ" } }
};

struct Pipes {
    std::vector<std::string> pipes;
    std::vector<Coord> loop;
    Coord start{-1, -1};

    [[nodiscard]] char at(const Coord& c) const { return at(c.y, c.x); }
    [[nodiscard]] char at(int y, int x) const {
        if (x < 0 || y < 0 || y >= static_cast<int>(pipes.size()) || x >= static_cast<int>(pipes[y].size()))
            return ' ';
        else
            return pipes[y][x];
    }

    void initStart() {
        findStart();
        auto startSymbol = getSymbolStart();
        if (startSymbol == '.')
            throw std::runtime_error("Invalid start configuration");
        pipes[start.y][start.x] = startSymbol;
    }

    void computeLoop() {
        std::stack<Coord> currentStack;
        currentStack.emplace(start);
        while (!currentStack.empty()) {
            auto current = currentStack.top();
            currentStack.pop();
            loop.emplace_back(current);

            auto validNeighbors = getValidNeighbors(current);
            if (validNeighbors.empty())
                break;

            for (const auto& neighbor : validNeighbors) {
                currentStack.emplace(neighbor);
            }
        }
    }

private:
    void findStart() {
        for (std::size_t j{0}; j < pipes.size(); ++j) {
            auto pos = pipes[j].find('S');
            if (pos != std::string::npos) {
                start = Coord{static_cast<int>(pos), static_cast<int>(j)};
                return;
            }
        }
    }

    [[nodiscard]] char getSymbolStart() const {
        for (const auto& [key, arr] : mapNeighbors) {
            int connections{0};
            for (std::size_t i{0}; i < arr.size(); ++i) {
                auto neighborPipe = at(start + neighbors[i]);
                if (arr[i].contains(neighborPipe))
                    ++connections;
            }
            if (connections == 2)
                return key;
        }
        return '.';
    }

    [[nodiscard]] std::vector<Coord> getValidNeighbors(const Coord& current) const {
        auto currentChar = at(current);
        std::vector<Coord> validNeighbors;
        for (std::size_t i{0}; i < neighbors.size(); ++i) {
            if (std::find(loop.begin(), loop.end(), current + neighbors[i]) != loop.end())
                continue;
            auto neighborPipe = at(current + neighbors[i]);
            auto validOptions = mapNeighbors[currentChar][i];
            if (validOptions.contains(neighborPipe))
                validNeighbors.emplace_back(current + neighbors[i]);
        }
        return validNeighbors;
    }
};

auto parseInput(std::string_view fileName) {
    std::ifstream infile(fileName);
    if (!infile)
        throw std::runtime_error("Could not open file " + std::string(fileName));

    Pipes pipes;
    while (!infile.eof()) {
        std::string line;
        std::getline(infile, line);
        if (line.empty()) continue;
        pipes.pipes.push_back(std::move(line));
    }
    pipes.initStart();
    pipes.computeLoop();
    return pipes;
}



auto solvePart1(const Pipes& pipes) {
    return ((pipes.loop.size() + 1) / 2);
}

enum class Direction { undefined, fromUp, fromDown };

auto solvePart2(const Pipes& pipes) {
    std::set<Coord> insidePoints;
    auto maxX = std::max_element(pipes.loop.begin(), pipes.loop.end(), [](const auto& c1, const auto& c2) { return c1.x < c2.x; })->x;
    auto maxY = std::max_element(pipes.loop.begin(), pipes.loop.end(), [](const auto& c1, const auto& c2) { return c1.y < c2.y; })->y;
    for (int y{0}; y <= maxY; ++y) {
        bool inside{false};
        using enum  Direction;
        Direction currentDir{undefined};
        for (int x{0}; x <= maxX; ++x) {
            auto loopEntry = std::find(pipes.loop.begin(), pipes.loop.end(), Coord{x, y});
            if (loopEntry == pipes.loop.end() && inside)
                insidePoints.emplace(Coord{x, y});
            else { // current coord is a loop entry
                switch (pipes.at(*loopEntry)) {
                    case '|': inside = !inside; break; // encounter horizontal pipe: inside toggles
                    case 'L': currentDir = fromUp; break; // note that we came from upstairs
                    case 'F': currentDir = fromDown; break; // note that we came from downstairs
                    case 'J': if (currentDir != fromUp) inside = !inside; // if we did not come from upstairs, toggle inside
                              currentDir = undefined; break;
                    case '7': if (currentDir != fromDown) inside = !inside; // if we did not come from downstairs, toggle inside
                              currentDir = undefined; break;
                    default: break; // do nothing in other cases
                }
            }
        }
    }

    return insidePoints.size();
}

int main() {
    std::println("AoC 2023: 10");

    try {
        auto pipes = parseInput("../inputs/10.txt");
        std::println("Part 1: {}", solvePart1(pipes));
        std::println("Part 2: {}", solvePart2(pipes));
    }
    catch (std::exception& e) {
        std::println("Exception: {}", e.what());
    }

    return 0;
}