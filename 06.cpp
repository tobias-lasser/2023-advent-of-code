#include <charconv>
#include <fstream>
#include <print>
#include <ranges>
#include <string>
#include <string_view>
#include <vector>

struct Race {
    int time{0};
    int distance{0};
};

auto splitIntString(std::string_view sv, std::string_view sep) {
    return sv | std::views::split(sep) | std::views::filter([](auto v) { return !v.empty(); }) | std::views::transform([](auto v) { int i{0}; std::from_chars(v.data(), v.data() + v.size(), i); return i; }) | std::ranges::to<std::vector<int>>();
}

auto parseInput(std::string_view filename) {
    std::ifstream infile(filename);
    if (!infile)
        throw std::runtime_error("Could not open file " + std::string(filename));

    std::string line1, line2;
    std::getline(infile, line1);
    std::getline(infile, line2);

    auto times = splitIntString(line1.substr(9), " ");
    auto distances = splitIntString(line2.substr(9), " ");
    if (times.size() != distances.size())
        throw std::runtime_error("Mismatch in Time/Distance size.");

    std::vector<Race> races;// should be able to do this with std::ranges::zip_view...
    for (std::size_t i{0}; i < times.size(); ++i)
        races.emplace_back(times[i], distances[i]);


    return races;
}

std::pair<long, long> evaluateRace(long time, long distance) {
    long x1 = std::floor((0.5 * (time - std::sqrt(time * time - 4 * distance)) + 1.0));
    long x2 = std::ceil((0.5 * (time + std::sqrt(time * time - 4 * distance)) - 1.0));
    return {x1, x2};
}

auto solvePart1(const std::vector<Race> &races) {
    long product{1};
    for (const auto &race: races) {
        auto [x1, x2] = evaluateRace(race.time, race.distance);
        product *= (x2 - x1 + 1);
    }

    return product;
}

auto solvePart2(const std::vector<Race> &races) {
    std::string timeStr, distanceStr;// again, some fancy ranges thing here would be cool...
    for (const auto &r: races) {
        timeStr += std::to_string(r.time);
        distanceStr += std::to_string(r.distance);
    }
    long time = std::stol(timeStr);
    long distance = std::stol(distanceStr);

    auto [x1, x2] = evaluateRace(time, distance);
    return (x2 - x1 + 1);
}

int main() {
    std::println("AoC 2023: 06");

    try {
        auto races = parseInput("../inputs/06.txt");
        std::println("Part 1: {}", solvePart1(races));
        std::println("Part 2: {}", solvePart2(races));
    } catch (std::exception &e) {
        std::println("Exception: {}", e.what());
    }

    return 0;
}