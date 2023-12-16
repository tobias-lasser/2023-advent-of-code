#include <print>
#include <vector>
#include <string>
#include <string_view>
#include <fstream>
#include <unordered_map>
#include <unordered_set>
#include <mdspan> // try out mdspan for the first time!
#include <utility>
#include <stack>

enum class TileType { empty, mirrorSWtoNE, mirrorNWtoSE, splitterWE, splitterNS };
enum class Direction { up, down, left, right };

struct Tile {
    TileType type{TileType::empty};
    std::unordered_set<Direction> beams{};
};

using Coord = std::array<int, 2>;

struct Beam {
    Coord pos{0, -1};
    Direction dir{Direction::right};

    [[nodiscard]] Coord nextCoord() const {
        Coord goNext{pos};
        switch (dir) {
            case Direction::up: --goNext[0]; break;
            case Direction::down: ++goNext[0]; break;
            case Direction::left: --goNext[1]; break;
            case Direction::right: ++goNext[1]; break;
        }
        return goNext;
    }
};

bool isValidCoord(const Coord& coord, const auto& grid) {
    return coord[0] >= 0 && coord[1] >= 0 && coord[0] < grid.extent(0) && coord[1] < grid.extent(1);
}

using Grid = std::pair<std::vector<Tile>, std::dextents<int, 2>>;


auto parseInput(std::string_view fileName) {
    std::ifstream infile(fileName);
    if (!infile)
        throw std::runtime_error("Could not open file " + std::string(fileName));

    Grid contraption;
    int lineCounter{0}, colCounter{0};
    while (!infile.eof()) {
        std::string line;
        std::getline(infile, line);
        if (line.empty()) continue;

        ++lineCounter;
        colCounter = 0;
        using enum TileType;
        std::unordered_map<char, TileType> char2TileType{ {'.', empty}, {'/', mirrorSWtoNE}, {'\\', mirrorNWtoSE}, {'-', splitterWE}, {'|', splitterNS} };
        for (const char& c : line) {
            Tile t;
            t.type = char2TileType.at(c);
            contraption.first.push_back(t);
            ++colCounter;
        }
    }
    contraption.second = std::dextents<int, 2>{lineCounter, colCounter};

    return contraption;
}

auto printGrid(const Grid& contraption) {
    using enum TileType;
    std::unordered_map<TileType, char> tileType2Char{ {empty, '.'}, {mirrorSWtoNE, '/'}, {mirrorNWtoSE, '\\'}, {splitterWE, '-'}, {splitterNS, '|'} };
    using enum Direction;
    std::unordered_map<Direction, char> beam2Char{ {up, '^'}, {down, 'v'}, {left, '<'}, {right, '>'} };
    auto grid{std::mdspan(contraption.first.data(), contraption.second)};
    for (auto j{0uz}; j < grid.extent(0); ++j) {
        for (auto i{0uz}; i < grid.extent(1); ++i) {
            const auto& tb = grid[j, i].beams;
            if (tb.empty() || grid[j, i].type != TileType::empty)
                std::print("{}", tileType2Char.at(grid[j, i].type));
            else if (tb.size() == 1)
                std::print("{}", beam2Char.at(*tb.begin()));
            else
                std::print("{}", tb.size());
        }
        std::println("");
    }
}

void simulateBeam(const Beam& beamStart, Grid& contraption) {
    auto grid{std::mdspan(contraption.first.data(), contraption.second)};
    Beam beam{beamStart};
    std::stack<Beam> beamStack;
    beamStack.push(beam);
    while (!beamStack.empty()) {
        beam = beamStack.top();
        beamStack.pop();
        if (isValidCoord(beam.pos, grid)) {
            if (grid[beam.pos[0], beam.pos[1]].beams.contains(beam.dir))
                continue;
            else
                grid[beam.pos[0], beam.pos[1]].beams.insert(beam.dir);
        }

        auto nextCoord = beam.nextCoord();
        if (!isValidCoord(nextCoord, grid))
            continue;

        const auto& nextTile = grid[nextCoord[0], nextCoord[1]];
        if (nextTile.type == TileType::empty)
            beamStack.emplace(nextCoord, beam.dir);
        else if (nextTile.type == TileType::mirrorSWtoNE || nextTile.type == TileType::mirrorNWtoSE) {
            switch (beam.dir) {
                case Direction::up:
                    beamStack.emplace(nextCoord, (nextTile.type == TileType::mirrorSWtoNE) ? Direction::right : Direction::left);
                    break;
                case Direction::down:
                    beamStack.emplace(nextCoord, (nextTile.type == TileType::mirrorSWtoNE) ? Direction::left : Direction::right);
                    break;
                case Direction::right:
                    beamStack.emplace(nextCoord, (nextTile.type == TileType::mirrorSWtoNE) ? Direction::up : Direction::down);
                    break;
                case Direction::left:
                    beamStack.emplace(nextCoord, (nextTile.type == TileType::mirrorSWtoNE) ? Direction::down : Direction::up);
                    break;
            }
        }
        else { // splitter
            if ((nextTile.type == TileType::splitterWE && (beam.dir == Direction::left || beam.dir == Direction::right))
                || (nextTile.type == TileType::splitterNS && (beam.dir == Direction::up || beam.dir == Direction::down)))
                beamStack.emplace(nextCoord, beam.dir);
            else if (nextTile.type == TileType::splitterWE) {
                beamStack.emplace(nextCoord, Direction::left);
                beamStack.emplace(nextCoord, Direction::right);
            }
            else {
                beamStack.emplace(nextCoord, Direction::up);
                beamStack.emplace(nextCoord, Direction::down);
            }
        }
    }
}

int countEnergized(const Grid& contraption) {
    int energized{0};
    for (const auto& tile : contraption.first) {
        if (!tile.beams.empty())
            ++energized;
    }
    return energized;
}

auto solvePart1(Grid contraption) {
    simulateBeam({{0, -1}, Direction::right},contraption);
    return countEnergized(contraption);
}

auto solvePart2(const Grid& contraption) {
    std::vector<Beam> startingBeams;
    for (int j{0}; j < contraption.second.extent(0); ++j) {
        startingBeams.push_back({{j, -1}, Direction::right});
        startingBeams.push_back({{j, contraption.second.extent(1)}, Direction::left});
    }
    for (int i{0}; i < contraption.second.extent(1); ++i) {
        startingBeams.push_back({{-1, i}, Direction::down});
        startingBeams.push_back({{contraption.second.extent(0), i}, Direction::up});
    }

    int maxEnergized{0};
    for (const auto& startingBeam : startingBeams) {
        Grid currentGrid{contraption};
        simulateBeam(startingBeam, currentGrid);
        auto energized{countEnergized(currentGrid)};
        maxEnergized = std::max(energized, maxEnergized);
    }
    return maxEnergized;
}

int main() {
    std::println("AoC 2023: 16");

    try {
        auto contraption = parseInput("../inputs/16.txt");
        std::println("Part 1: {}", solvePart1(contraption));
        std::println("Part 2: {}", solvePart2(contraption));
    }
    catch (std::exception& e) {
        std::println("Exception: {}", e.what());
    }

    return 0;
}