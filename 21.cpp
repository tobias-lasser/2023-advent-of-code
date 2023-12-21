#include <print>
#include <string>
#include <string_view>
#include <vector>
#include <fstream>
#include <queue>
#include <unordered_map>
#include <utility>
#include <unordered_set>

struct Coord {
    long x{0}, y{0};
};

bool operator==(const Coord& lhs, const Coord& rhs) {
    return lhs.x == rhs.x && lhs.y == rhs.y;
}

template<>
struct std::hash<Coord> {
    std::size_t operator()(const Coord& c) const noexcept {
        return c.x * 31 + c.y;
    }
};

struct Garden {
    std::vector<std::string> plots;

    [[nodiscard]] Coord findStart() const {
        for (long y{0}; y < plots.size(); ++y) {
            auto pos = plots[y].find('S');
            if (pos != std::string::npos)
                return {static_cast<long>(pos), y};
        }
        throw std::runtime_error("No start plot found in garden");
    }

    [[nodiscard]] std::vector<Coord> getNeighbors(const Coord& coord) const {
        std::vector<Coord> neighbors;
        if (coord.x > 0 && plots[coord.y][coord.x - 1] != '#')
            neighbors.emplace_back(coord.x - 1, coord.y);
        if (coord.x < plots[coord.y].size() - 1 && plots[coord.y][coord.x + 1] != '#')
            neighbors.emplace_back(coord.x + 1, coord.y);
        if (coord.y > 0 && plots[coord.y - 1][coord.x] != '#')
            neighbors.emplace_back(coord.x, coord.y - 1);
        if (coord.y < plots.size() - 1 && plots[coord.y + 1][coord.x] != '#')
            neighbors.emplace_back(coord.x, coord.y + 1);
        return neighbors;
    }

    [[nodiscard]] bool check(Coord c) const {
        static long sizeX{static_cast<long>(plots.front().size())};
        static long sizeY{static_cast<long>(plots.size())};
        long x = (c.x + std::abs( (c.x-sizeX+1) / sizeX) * sizeX) % sizeX;
        long y = (c.y + std::abs( (c.y-sizeY+1) / sizeY) * sizeY) % sizeY;
        return plots[y][x] != '#';

    }

    [[nodiscard]] std::vector<Coord> getNeighborsInfinite(const Coord& c) const {
        std::vector<Coord> neighbors;
        if (check({c.x - 1, c.y})) neighbors.emplace_back(c.x - 1, c.y);
        if (check({c.x + 1, c.y})) neighbors.emplace_back(c.x + 1, c.y);
        if (check({c.x, c.y - 1})) neighbors.emplace_back(c.x, c.y - 1);
        if (check({c.x, c.y + 1})) neighbors.emplace_back(c.x, c.y + 1);
        return neighbors;
    }

    void print(const std::unordered_set<Coord>& options) const {
        for (long y{0}; y < plots.size(); ++y) {
            for (long x{0}; x < plots[y].size(); ++x) {
                if (options.contains({x, y}))
                    std::print("O");
                else std::print("{}", plots[y][x]);
            }
            std::println("");
        }
    }
};

auto parseInput(std::string_view fileName) {
    std::ifstream infile(fileName);
    if (!infile)
        throw std::runtime_error("Could not open " + std::string(fileName));

    Garden garden;
    while (!infile.eof()) {
        std::string line;
        std::getline(infile, line);
        if (line.empty()) continue;
        garden.plots.push_back(std::move(line));
    }
    return garden;
}

auto solvePart1(const Garden& garden, long maxSteps, bool scaleStart = false) {
    Coord start = garden.findStart();
    if (scaleStart) {
        start.x += static_cast<long>(garden.plots.front().size()) * 2;
        start.y += static_cast<long>(garden.plots.size()) * 2;

    }
    std::unordered_set<Coord> plots;
    plots.insert(start);

    for (auto i{1}; i <= maxSteps; ++i) {
        std::unordered_set<Coord> newPlots;
        for (const auto& plot : plots) {
            for (const auto& neighbor : (scaleStart) ? garden.getNeighborsInfinite(plot) : garden.getNeighbors(plot))
                newPlots.insert(neighbor);
        }
        plots = std::move(newPlots);
    }

    return plots.size();
}


long quadraticPolynomialFit(Coord c0, Coord c1, Coord c2, long x) {
    // formula from https://en.wikipedia.org/wiki/Newton_polynomial
    return c0.y
           + (c1.y-c0.y) / (c1.x-c0.x) * (x-c0.x)
           + ( (c2.y-c1.y) / ((c2.x-c1.x)*(c2.x-c0.x)) - (c1.y-c0.y) / ((c1.x-c0.x)*(c2.x-c0.x))) * (x-c0.x) * (x-c1.x);
}

auto solvePart2(const Garden& garden, long maxSteps) {
    // magic quadratic polynomial interpolation idea stolen from Reddit...
    long size = static_cast<long>(garden.plots.size());
    long y0 = solvePart1(garden, size/2, true);
    long y1 = solvePart1(garden, size/2 + size*1, true);
    long y2 = solvePart1(garden, size/2 + size*2, true);
    return quadraticPolynomialFit({0, y0}, {1, y1}, {2, y2}, (maxSteps - size/2) / size);
}

int main() {
    std::println("AoC 2023: 21");

    try {
        auto garden = parseInput("../inputs/21.txt");
        std::println("Part 1: {}", solvePart1(garden, 64));
        std::println("Part 2: {}", solvePart2(garden, 26501365));
    }
    catch (std::exception& e) {
        std::println("Exception: {}", e.what());
    }

    return 0;
}