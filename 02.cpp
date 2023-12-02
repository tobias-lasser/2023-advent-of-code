#include <fstream>
#include <print>
#include <ranges>
#include <string_view>
#include <vector>

struct Cubes {
    int red{0}, green{0}, blue{0};
};

struct Game {
    int id{0};
    std::vector<Cubes> reveals{};
};

auto splitString(std::string_view sv, std::string_view sep) {
    auto parts = std::views::split(sv, sep);
    auto partsVec = std::ranges::to<std::vector<std::string>>(parts);
    return partsVec;
}

auto parseInput(std::string_view fileName) {
    std::ifstream infile(fileName);
    if (!infile)
        throw std::runtime_error("Could not open file " + std::string(fileName));

    std::vector<Game> games;
    while (!infile.eof()) {
        std::string line;
        std::getline(infile, line);
        if (!line.starts_with("Game")) continue;

        Game g;
        g.id = std::stoi(std::string(line.substr(5)));

        auto pv = splitString(line, ":");
        if (pv.size() != 2) continue;

        auto cubesVec = splitString(pv[1], ";");
        for (const auto &clist: cubesVec) {
            Cubes cubes;
            auto colorsVec = splitString(clist, ",");
            for (const auto &color: colorsVec) {
                auto entries = splitString(color, " ");
                if (entries.size() != 3) continue;

                if (entries[2].starts_with("green"))
                    cubes.green = std::stoi(entries[1]);
                else if (entries[2].starts_with("blue"))
                    cubes.blue = std::stoi(entries[1]);
                else if (entries[2].starts_with("red"))
                    cubes.red = std::stoi(entries[1]);
            }
            g.reveals.push_back(cubes);
        }
        games.push_back(std::move(g));
    }

    return games;
}

bool isImpossible(const Cubes &cubes, const Cubes &availableCubes) {
    return (cubes.red > availableCubes.red) || (cubes.blue > availableCubes.blue) || (cubes.green > availableCubes.green);
}

auto solvePart1(const std::vector<Game> &games) {
    Cubes availableCubes{12, 13, 14};

    int sumIDs{0};
    for (const auto &g: games) {
        int gamePossible{true};
        for (const auto &cubes: g.reveals) {
            if (isImpossible(cubes, availableCubes)) {
                gamePossible = false;
                break;
            }
        }
        if (gamePossible)
            sumIDs += g.id;
    }

    return sumIDs;
}

auto solvePart2(const std::vector<Game> &games) {
    int sumPower{0};
    for (const auto &g: games) {
        Cubes minimumCubes;
        for (const auto &cubes: g.reveals) {
            if (cubes.red > minimumCubes.red)
                minimumCubes.red = cubes.red;
            if (cubes.green > minimumCubes.green)
                minimumCubes.green = cubes.green;
            if (cubes.blue > minimumCubes.blue)
                minimumCubes.blue = cubes.blue;
        }
        sumPower += minimumCubes.red * minimumCubes.green * minimumCubes.blue;
    }

    return sumPower;
}

int main() {
    std::println("AoC 2023: 02");

    try {
        auto games = parseInput("../inputs/02.txt");
        std::println("Part 1: {}", solvePart1(games));
        std::println("Part 2: {}", solvePart2(games));
    } catch (std::exception &e) {
        std::println("Exception: {}", e.what());
    }

    return 0;
}