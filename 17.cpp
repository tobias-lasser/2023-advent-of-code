#include <print>
#include <vector>
#include <string>
#include <string_view>
#include <fstream>
#include <unordered_map>
#include <map>
#include <queue>

auto parseInput(std::string_view fileName) {
    std::ifstream infile(fileName);
    if (!infile)
        throw std::runtime_error("Could not open file " + std::string(fileName));

    std::vector<std::vector<int>> city;
    while (!infile.eof()) {
        std::string line;
        std::getline(infile, line);
        if (line.empty()) continue;

        std::vector<int> numbers;
        numbers.reserve(line.size());
        for (const auto& c : line)
            numbers.push_back(c - '0');
        city.push_back(std::move(numbers));
    }
    return city;
}

struct Coord {
    int x{-1}, y{-1};
    int dirx{-5}, diry{-5};
    int stepsInDir{0};

    [[nodiscard]] bool isSameDir(int dx, int dy) const {
        return dirx == dx && diry == dy;
    }

    [[nodiscard]] bool isOppositeDir(int dx, int dy) const {
        if (dirx == 0 && diry == 0) return false;
        if (dirx != 0) return (dirx == -dx);
        else return (diry == -dy);
    }

    [[nodiscard]] std::vector<Coord> getNeighbors(const std::vector<std::vector<int>>& city, bool ultra) const {
        std::vector<Coord> neighbors;
        const int maxSteps = ultra ? 10 : 3;
        if (x > 0 && !isOppositeDir(-1, 0) && !(isSameDir(-1, 0) && stepsInDir >= maxSteps)) {
            if (!ultra || stepsInDir >= 4 || (isSameDir(-1, 0) && stepsInDir < 4) || isSameDir(0, 0))
                neighbors.emplace_back(x - 1, y, -1, 0, isSameDir(-1, 0) ? stepsInDir + 1 : 1);
        }
        if (x < city.front().size() - 1 && !isOppositeDir(1, 0) && !(isSameDir(1, 0) && stepsInDir >= maxSteps)) {
            if (!ultra || stepsInDir >= 4 || (isSameDir(1, 0) && stepsInDir < 4) || isSameDir(0, 0))
                neighbors.emplace_back(x + 1, y, 1, 0, isSameDir(1, 0) ? stepsInDir + 1 : 1);
        }
        if (y > 0 && !isOppositeDir(0, -1) && !(isSameDir(0, -1) && stepsInDir >= maxSteps)) {
            if (!ultra || stepsInDir >= 4 || (isSameDir(0, -1) && stepsInDir < 4) || isSameDir(0, 0))
                neighbors.emplace_back(x, y - 1, 0, -1, isSameDir(0, -1) ? stepsInDir + 1 : 1);
        }
        if (y < city.size() - 1 && !isOppositeDir(0, 1) && !(isSameDir(0, 1) && stepsInDir >= maxSteps)) {
            if (!ultra || stepsInDir >= 4 || (isSameDir(0, 1) && stepsInDir < 4) || isSameDir(0, 0))
                neighbors.emplace_back(x, y + 1, 0, 1, isSameDir(0, 1) ? stepsInDir + 1 : 1);
        }
        return neighbors;
    }
};

bool operator==(const Coord& lhs, const Coord& rhs) {
    return lhs.x == rhs.x && lhs.y == rhs.y
            && lhs.dirx == rhs.dirx && lhs.diry == rhs.diry
            && lhs.stepsInDir == rhs.stepsInDir;
}

template<>
struct std::hash<Coord> {
    std::size_t operator()(const Coord& c) const noexcept {
        return c.x * 31 + c.y + c.dirx * 53 + c.diry * 41 + c.stepsInDir * 79;
    }
};


auto dijkstra(const std::vector<std::vector<int>>& city, bool ultra = false) {
    Coord startCoord{0, 0, 0, 0, 0};
    std::unordered_map<Coord, int> distances;
    distances[startCoord] = 0;

    using CoordDist = std::pair<Coord, int>;

    struct compareCoordDist {
        bool operator()(const CoordDist& a, const CoordDist& b) const {
            return a.second > b.second;
        }
    };
    std::priority_queue<CoordDist, std::vector<CoordDist>, compareCoordDist> pq;
    pq.emplace(startCoord, 0);
    while (!pq.empty()) {
        auto [coord, dist] = pq.top();
        pq.pop();

        if (coord.x == static_cast<int>(city.front().size() - 1) && coord.y == static_cast<int>(city.size() - 1))
            return dist;

        for (const auto& neighbor : coord.getNeighbors(city, ultra)) {
            int newDist = dist + city[neighbor.y][neighbor.x];
            if (!distances.contains(neighbor) || newDist < distances[neighbor]) {
                pq.emplace(neighbor, newDist);
                distances[neighbor] = newDist;
            }
        }
    }

    return -1;
}

auto solvePart1(const std::vector<std::vector<int>>& city) {
    return dijkstra(city);
}

auto solvePart2(const std::vector<std::vector<int>>& city) {
    return dijkstra(city, true);
}

int main() {
    std::println("AoC 2023: 17");

    try {
        auto city = parseInput("../inputs/17.txt");
        std::println("Part 1: {}", solvePart1(city));
        std::println("Part 2: {}", solvePart2(city));
    }
    catch (std::exception& e) {
        std::println("Exception: {}", e.what());
    }

    return 0;
}