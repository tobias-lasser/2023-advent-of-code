#include <array>
#include <charconv>
#include <format>
#include <fstream>
#include <limits>
#include <map>
#include <print>
#include <ranges>
#include <string>
#include <string_view>
#include <vector>

struct AlmanacMap {
    void addRange(long dest, long source, long len) {
        source2range[source] = dest;
        if (!source2range.contains(source + len))// don't overwrite stuff
            source2range[source + len] = source + len;
    }

    [[nodiscard]] long getMapping(long source) const {
        auto itNotLess = source2range.lower_bound(source);
        auto itGreater = source2range.upper_bound(source);

        if (itNotLess == itGreater && itNotLess != source2range.begin())// both bigger than source, go to previous
            --itNotLess;

        if (itNotLess == source2range.end())// no entry
            return source;
        else {
            auto &[key, value] = *itNotLess;
            return value + (source - key);
        }
    }

    std::string name;
    std::map<long, long> source2range;
};

struct Almanac {
    std::vector<long> seeds{};
    std::vector<AlmanacMap> maps{};
};

auto splitIntString(std::string_view sv, std::string_view sep) {
    return sv | std::views::split(sep) | std::views::transform([](auto v) {
                    long i{0}; std::from_chars(v.data(), v.data() + v.size(), i); return i; }) | std::ranges::to<std::vector<long>>();
}

auto parseInput(std::string_view filename) {
    std::ifstream infile(filename);
    if (!infile)
        throw std::runtime_error("Could not open file " + std::string(filename));

    Almanac almanac;
    while (!infile.eof()) {
        std::string line;
        std::getline(infile, line);

        if (line.starts_with("seeds:")) {
            std::string_view sv(line.begin() + 7, line.end());
            almanac.seeds = splitIntString(sv, " ");
            continue;
        }

        if (line.empty())
            continue;

        if (!std::isdigit(line[0])) {
            almanac.maps.emplace_back();
            almanac.maps.back().name = line;
            almanac.maps.back().addRange(0, 0, 0);
            continue;
        }

        if (!almanac.maps.empty()) {
            auto ints = splitIntString(line, " ");
            if (ints.size() != 3) continue;
            almanac.maps.back().addRange(ints[0], ints[1], ints[2]);
        }
    }

    return almanac;
}

auto solvePart1(const Almanac &almanac) {
    long locationNumber{std::numeric_limits<long>::max()};

    for (auto s: almanac.seeds) {
        long number{s};
        for (const auto &m: almanac.maps)
            number = m.getMapping(number);

        locationNumber = std::min(locationNumber, number);
    }

    return locationNumber;
}

auto solvePart2(const Almanac &almanac) {
    // note: really slow -> should implement path compression through the maps, but alas, no time
    long locationNumber{std::numeric_limits<long>::max()};

    for (std::size_t i{0}; i < almanac.seeds.size(); i += 2) {
        for (long s{almanac.seeds[i]}; s < almanac.seeds[i] + almanac.seeds[i + 1]; ++s) {
            long number{s};
            for (const auto &m: almanac.maps)
                number = m.getMapping(number);

            locationNumber = std::min(locationNumber, number);
        }
    }

    return locationNumber;
}

int main() {
    std::println("AoC 2023: 05");

    try {
        auto almanac = parseInput("../inputs/05.txt");
        std::println("Part 1: {}", solvePart1(almanac));
        std::println("Part 2: {}", solvePart2(almanac));
    } catch (std::exception &e) {
        std::println("Exception: {}", e.what());
    }

    return 0;
}