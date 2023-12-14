#include <print>
#include <string>
#include <string_view>
#include <fstream>
#include <vector>
#include <limits>
#include <unordered_map>
#include <numeric>

auto parseInput(std::string_view fileName) {
    std::ifstream infile(fileName);
    if (!infile)
        throw std::runtime_error("Could not open file " + std::string(fileName));

    std::vector<std::string> lines;
    while (!infile.eof()) {
        std::string line;
        std::getline(infile, line);
        if (line.empty()) continue;
        lines.push_back(std::move(line));
    }
    return lines;
}

void tiltPlatformNorth(std::vector<std::string>& platform) {
    for (int i{0}; i < platform[0].size(); ++i) {
        int lastFreeIdx{(platform[0][i] == '.') ? 0 : std::numeric_limits<int>::max()};
        for (int j{1}; j < platform.size(); ++j) {
            if (platform[j][i] == 'O' && lastFreeIdx < j) {
                platform[j][i] = '.';
                platform[lastFreeIdx][i] = 'O';
                ++lastFreeIdx;
            }
            else if (platform[j][i] == '.')
                lastFreeIdx = std::min(j, lastFreeIdx);
            else
                lastFreeIdx = std::numeric_limits<int>::max();
        }
    }
}

void tiltPlatformSouth(std::vector<std::string>& platform) { // too tired to refactor this...
    for (int i{0}; i < platform[0].size(); ++i) {
        const int psize = static_cast<int>(platform.size());
        int lastFreeIdx{(platform[psize - 1][i] == '.') ? psize - 1 : std::numeric_limits<int>::min()};
        for (int j{psize - 2}; j >= 0; --j) {
            if (platform[j][i] == 'O' && lastFreeIdx > j) {
                platform[j][i] = '.';
                platform[lastFreeIdx][i] = 'O';
                --lastFreeIdx;
            }
            else if (platform[j][i] == '.')
                lastFreeIdx = std::max(j, lastFreeIdx);
            else
                lastFreeIdx = std::numeric_limits<int>::min();
        }
    }
}

void tiltPlatformWest(std::vector<std::string>& platform) {
    for (int j{0}; j < platform.size(); ++j) {
        int lastFreeIdx{(platform[j][0] == '.') ? 0 : std::numeric_limits<int>::max()};
        for (int i{1}; i < platform[j].size(); ++i) {
            if (platform[j][i] == 'O' && lastFreeIdx < i) {
                platform[j][i] = '.';
                platform[j][lastFreeIdx] = 'O';
                ++lastFreeIdx;
            }
            else if (platform[j][i] == '.')
                lastFreeIdx = std::min(i, lastFreeIdx);
            else
                lastFreeIdx = std::numeric_limits<int>::max();
        }
    }
}

void tiltPlatformEast(std::vector<std::string>& platform) {
    for (int j{0}; j < platform.size(); ++j) {
        int psize = static_cast<int>(platform[j].size());
        int lastFreeIdx{platform[j][psize - 1] == '.' ? psize - 1 : std::numeric_limits<int>::min()};
        for (int i{psize - 2}; i >= 0; --i) {
            if (platform[j][i] == 'O' && lastFreeIdx > i) {
                platform[j][i] = '.';
                platform[j][lastFreeIdx] = 'O';
                --lastFreeIdx;
            }
            else if (platform[j][i] == '.')
                lastFreeIdx = std::max(i, lastFreeIdx);
            else
                lastFreeIdx = std::numeric_limits<int>::min();

        }
    }}


auto cyclePlatform(std::vector<std::string>& platform) {
    tiltPlatformNorth(platform);
    tiltPlatformWest(platform);
    tiltPlatformSouth(platform);
    tiltPlatformEast(platform);
}

auto computeLoad(const std::vector<std::string>& platform) {
    long load{0};
    for (std::size_t i{0}; i < platform.size(); ++i) {
        long countO{0};
        for (const auto& c : platform[i])
            if (c == 'O') ++countO;
        load += countO * static_cast<long>(platform.size() - i);
    }
    return load;
}

auto solvePart1(std::vector<std::string> platform) {
    tiltPlatformNorth(platform);
    return computeLoad(platform);
}


template<>
struct std::hash<std::vector<std::string>> {
    std::size_t operator()(const std::vector<std::string>& v) const noexcept {
        return std::hash<std::string>()(std::accumulate(std::next(v.begin()), v.end(), v[0]));
    }
};

auto solvePart2(std::vector<std::string> platform, long repetitions) {
    std::unordered_map<std::vector<std::string>, long> cache;
    for (long i{0}; i < repetitions; ++i) {
        if (!cache.contains(platform))
            cache[platform] = i;
        else {
            auto period = i - cache[platform];
            i = repetitions - (repetitions - i) % period;
        }
        cyclePlatform(platform);
    }
    return computeLoad(platform);
}


int main() {
    std::println("AoC 2023: 14");

    try {
        auto platform = parseInput("../inputs/14.txt");
        std::println("Part 1: {}", solvePart1(platform));
        std::println("Part 2: {}", solvePart2(platform, 1'000'000'000));
    }
    catch (std::exception& e) {
        std::println("Exception: {}", e.what());
    }

    return 0;
}