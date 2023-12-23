#include <print>
#include <string>
#include <string_view>
#include <vector>
#include <fstream>
#include <ranges>
#include <format>
#include <array>
#include <charconv>
#include <mdspan>
#include <unordered_map>
#include <unordered_set>

struct Coord {
    std::array<int, 3> c{0, 0, 0};

    auto& operator[](int i) { return c[i]; }
    const auto& operator[](int i) const { return c[i]; }
    bool operator!=(const Coord& o) const { return c != o.c; }
};

struct Brick {
    int id{0};
    Coord start, end;
};

struct BrickWorld {
    std::unordered_map<int, Brick> bricks;
    std::vector<int> gridVector;
    std::mdspan<int, std::dextents<int, 3>> grid;

    BrickWorld(std::vector<Brick>&& bs) {
        for (const auto& b : bs)
            bricks[b.id] = b;
        Coord minC{10000, 10000, 10000}, maxC{-1, -1, -1};
        for (const auto& [id, b] : bricks) {
            for (auto i{0}; i < 3; ++i) {
                if (b.start[i] < minC[i]) minC[i] = b.start[i];
                if (b.start[i] > maxC[i]) maxC[i] = b.start[i];
                if (b.end[i] < minC[i]) minC[i] = b.end[i];
                if (b.end[i] > maxC[i]) maxC[i] = b.end[i];
            }
        }
        if (minC != Coord{0, 0, 1})
            throw std::runtime_error("Invalid min coords");
        auto extents = std::dextents<int, 3>(maxC[0]+1, maxC[1]+1, maxC[2]+1);
        gridVector.resize(extents.extent(0) * extents.extent(1) * extents.extent(2));
        grid = std::mdspan<int, std::dextents<int, 3>>(gridVector.data(), extents);
    }

    BrickWorld(const BrickWorld& bw) : bricks{bw.bricks}, gridVector{bw.gridVector.begin(), bw.gridVector.end()} {
        grid = std::mdspan<int, std::dextents<int, 3>>(gridVector.data(), bw.grid.extents());
    }

    void populateGrid() {
        std::fill(gridVector.begin(), gridVector.end(), 0);
        for (const auto& [id, b] : bricks)
            populateBrick(id);
    }

    void populateBrick(int id, bool remove = false) {
        const auto& b = bricks[id];
        for (int x{b.start[0]}; x <= b.end[0]; ++x) {
            for (int y{b.start[1]}; y <= b.end[1]; ++y) {
                for (int z{b.start[2]}; z <= b.end[2]; ++z)
                    grid[x, y, z] = (remove) ? 0 :b.id;
            }
        }
    }

    [[nodiscard]] bool isUnoccupied(int id, const Coord& llf, const Coord& lrb) const {
        int z = llf[2];
        for (int x{llf[0]}; x <= lrb[0]; ++x) {
            for (int y{llf[1]}; y <= lrb[1]; ++y) {
                if (grid[x,y,z] != 0 && grid[x,y,z] != id)
                    return false;
            }
        }
        return true;
    }

    void shiftIdDownTo(int id, int currentZ) {
        Brick& b = bricks[id];
        populateBrick(id, true);
        b.end[2] -= b.start[2] - currentZ;
        b.start[2] = currentZ;
        populateBrick(id);
    }

    [[nodiscard]] bool letItGo(int id) {
        const Brick& b = bricks[id];
        Coord lowerLeftFront{b.start[0], b.start[1], b.start[2]};
        Coord lowerRightBack{b.end[0], b.end[1], b.start[2]};
        int currentZ = b.start[2] - 1;
        while (currentZ >= 1) {
            lowerLeftFront[2] = currentZ;
            lowerRightBack[2] = currentZ;
            if (isUnoccupied(id, lowerLeftFront, lowerRightBack))
                --currentZ;
            else break;
        }
        if (currentZ < b.start[2] - 1) {
            shiftIdDownTo(id, currentZ + 1);
            return true;
        }
        else return false;
    }

    [[nodiscard]] long letBricksFall() {
        long count{0};
        for (int z{2}; z < grid.extent(2); ++z) {
            std::unordered_set<int> handledBricks;
            for (int x{0}; x < grid.extent(0); ++x) {
                for (int y{0}; y < grid.extent(1); ++y) {
                    auto id = grid[x,y,z];
                    if (id != 0 && !handledBricks.contains(id)) {
                        count += letItGo(id);
                        handledBricks.insert(id);
                    }
                }
            }
        }
        return count;
    }

    [[nodiscard]] std::unordered_set<int> isSittingOn(int id) const {
        std::unordered_set<int> sittingOn;
        const Brick& b = bricks.at(id);
        for (int x{b.start[0]}; x <= b.end[0]; ++x) {
            for (int y{b.start[1]}; y <= b.end[1]; ++y) {
                auto c = grid[x,y,b.start[2]-1];
                if (c != 0)
                    sittingOn.insert(c);
            }
        }
        return sittingOn;
    }

    [[nodiscard]] long countDisintegrate() const {
        std::unordered_set<int> cannotDisintegrate;
        for (const auto& [id, b] : bricks) {
            auto sittingOn = isSittingOn(id);
            if (sittingOn.size() == 1)
                cannotDisintegrate.insert(*sittingOn.begin());
        }

        long count{0};
        for (const auto& [id, b] : bricks) {
            if (!cannotDisintegrate.contains(id))
                ++count;
        }
        return count;
    }
};

auto splitIntString(std::string_view sv, std::string_view sep) {
    return sv | std::views::split(sep) | std::views::filter([](auto v) { return !v.empty(); }) | std::views::transform([](auto v) { int i{0}; std::from_chars(v.data(), v.data() + v.size(), i); return i; }) | std::ranges::to<std::vector<int>>();
}

auto parseInput(std::string_view fileName) {
    std::ifstream infile(fileName);
    if (!infile)
        throw std::runtime_error("Could not open file " + std::string(fileName));

    std::vector<Brick> bricks;
    int id{1};
    while (!infile.eof()) {
        std::string line;
        std::getline(infile, line);
        if (line.empty()) continue;

        auto parts = line | std::views::split('~') | std::ranges::to<std::vector<std::string>>();
        auto startParts = splitIntString(parts[0], ",");
        auto endParts = splitIntString(parts[1], ",");
        Brick b;
        b.id = id++;
        std::copy(startParts.begin(), startParts.end(), b.start.c.begin());
        std::copy(endParts.begin(), endParts.end(), b.end.c.begin());
        bricks.push_back(b);
    }
    BrickWorld bw(std::move(bricks));

    return bw;
}

auto solvePart1(auto& brickWorld) {
    brickWorld.populateGrid();
    auto ignore = brickWorld.letBricksFall();
    return brickWorld.countDisintegrate();
}

auto solvePart2(const auto& brickWorld) {
    std::vector<int> ids;
    ids.reserve(brickWorld.bricks.size());
    for (const auto& [id, b] : brickWorld.bricks)
        ids.emplace_back(id);

    auto sum{0};
    for (auto id : ids) {
        auto bw{brickWorld};
        bw.bricks.erase(id);
        bw.populateGrid();
        sum += bw.letBricksFall();
    }
    return sum;
}

int main() {
    std::println("AoC 2023: 22");

    try {
        auto brickWorld = parseInput("../inputs/22.txt");
        std::println("Part 1: {}", solvePart1(brickWorld));
        std::println("Part 2: {}", solvePart2(brickWorld));
    }
    catch (std::exception& e) {
        std::println("Exception: {}", e.what());
    }

    return 0;
}