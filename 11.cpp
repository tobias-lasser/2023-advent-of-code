#include <print>
#include <format>
#include <string>
#include <string_view>
#include <vector>
#include <fstream>
#include <utility>
#include <numeric>

auto parseInput(std::string_view filename) {
    std::ifstream infile(filename);
    if (!infile)
        throw std::runtime_error("Could not open file " + std::string(filename));

    std::vector<std::string> lines;
    while (!infile.eof()) {
        std::string line;
        std::getline(infile, line);
        if (line.empty()) continue;
        lines.push_back(std::move(line));
    }
    return lines;
}

void expandUniverse(std::vector<std::string>& universe) {
    if (universe.empty()) return;

    // mark columns for expansion
    for (std::size_t i{0}; i < universe.front().size(); ++i) {
        bool containsGalaxy{false};
        for (auto &row: universe)
            if (row[i] == '#') containsGalaxy = true;
        if (!containsGalaxy) {
            for (auto &row: universe)
                row[i] = 'e';
        }
    }

    // mark rows for expansion
    for (std::size_t j{0}; j < universe.size(); ++j) {
        bool containsGalaxy{universe[j].contains('#')};
        if (!containsGalaxy) {
            for (auto &c: universe[j])
                c = 'e';
        }
    }
}

using Coord = std::pair<long long, long long>;

auto findGalaxies(const std::vector<std::string>& universe) {
    std::vector<Coord> galaxies;
    for (std::size_t j{0}; j < universe.size(); ++j) {
        for (std::size_t i{0}; i < universe[j].size(); ++i) {
            if (universe[j][i] == '#')
                galaxies.emplace_back(j, i);
        }
    }
    return galaxies;
}

auto computeDistances(const std::vector<Coord>& galaxies, const std::vector<std::string>& universe, long long factor = 2) {
    std::vector<long long> distances;
    for (std::size_t i{0}; i < galaxies.size(); ++i) {
        for (std::size_t j{0}; j < galaxies.size(); ++j) {
            if (j >= i) break;

            long long countExpandedX{0};
            std::size_t startX = std::min(galaxies[i].second, galaxies[j].second);
            std::size_t endX = std::max(galaxies[i].second, galaxies[j].second);
            for (auto idx{startX + 1}; idx < endX; ++idx)
                if (universe[galaxies[i].first][idx] == 'e')
                    ++countExpandedX;

            long long countExpandedY{0};
            std::size_t startY = std::min(galaxies[i].first, galaxies[j].first);
            std::size_t endY = std::max(galaxies[i].first, galaxies[j].first);
            for (auto idx{startY + 1}; idx < endY; ++idx)
                if (universe[idx][galaxies[i].second] == 'e')
                    ++countExpandedY;

            long long distX = std::abs(galaxies[i].second - galaxies[j].second) - countExpandedX;
            long long distY = std::abs(galaxies[i].first - galaxies[j].first) - countExpandedY;

            long long distance = distX + distY + (countExpandedX + countExpandedY) * factor;
            distances.push_back(distance);
        }
    }
    return distances;
}

auto solvePart1(const std::vector<Coord>& galaxies, std::vector<std::string>& universe) {
    auto distances = computeDistances(galaxies, universe);
    return std::accumulate(distances.begin(), distances.end(), 0ll);
}

auto solvePart2(const std::vector<Coord>& galaxies, std::vector<std::string>& universe) {
    auto distances = computeDistances(galaxies, universe, 1'000'000);
    return std::accumulate(distances.begin(), distances.end(), 0ll);
}

int main() {
    std::println("AoC 2023: 11");

    try {
        auto universe = parseInput("../inputs/11.txt");
        expandUniverse(universe);
        auto galaxies = findGalaxies(universe);
        std::println("Part 1: {}", solvePart1(galaxies, universe));
        std::println("Part 2: {}", solvePart2(galaxies, universe));
    }
    catch (std::exception& e) {
        std::println("Exception: {}", e.what());
    }

    return 0;
}