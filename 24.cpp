#include <print>
#include <vector>
#include <string>
#include <string_view>
#include <fstream>
#include <ranges>
#include <charconv>
#include <array>
#include <optional>
#include <Eigen/Dense>

struct Coord {
    std::array<double, 3> xyz = {{0, 0, 0}};
};

Coord operator+(const Coord& a, const Coord& b) {
    return {a.xyz[0] + b.xyz[0], a.xyz[1] + b.xyz[1], a.xyz[2] + b.xyz[2]};
}

Coord operator*(double param, const Coord& a) {
    return {param * a.xyz[0], param * a.xyz[1], param * a.xyz[2]};
}

struct Hailstone {
    Coord position, velocity;

    [[nodiscard]] Coord getCoordAt(double param) const {
        return position + param * velocity;
    }
};

auto splitIntString(std::string_view sv, std::string_view sep) {
    return sv | std::views::split(sep) | std::views::filter([](auto v) { return !v.empty(); }) | std::views::transform([](auto v) { auto c = v | std::views::common; return std::stoll(std::string(c.begin(), c.end())); }) | std::ranges::to<std::vector<long long>>();
}

auto parseInput(std::string_view fileName) {
    std::ifstream infile(fileName);
    if (!infile)
        throw std::runtime_error("Could not open file " + std::string(fileName));

    std::vector<Hailstone> hailstones;
    while (!infile.eof()) {
        std::string line;
        std::getline(infile, line);
        if (line.empty()) continue;

        Hailstone hs;
        auto parts = line | std::views::split('@') | std::ranges::to<std::vector<std::string>>();
        auto pos = splitIntString(parts[0], ",");
        auto vel = splitIntString(parts[1], ",");
        std::copy_n(pos.begin(), 3, hs.position.xyz.begin());
        std::copy_n(vel.begin(), 3, hs.velocity.xyz.begin());
        hailstones.push_back(hs);
    }
    return hailstones;
}

std::optional<std::pair<double, double>> computeIntersection2D(const Hailstone& a, const Hailstone& b) {
    auto determinant = static_cast<double>(a.velocity.xyz[0] * b.velocity.xyz[1] - a.velocity.xyz[1] * b.velocity.xyz[0]);
    if (determinant == 0)
        return std::nullopt;
    auto diffx = a.position.xyz[0] - b.position.xyz[0];
    auto diffy = a.position.xyz[1] - b.position.xyz[1];
    auto paramA = static_cast<double>(-b.velocity.xyz[1] * diffx + b.velocity.xyz[0] * diffy) / determinant;
    auto paramB = static_cast<double>(-a.velocity.xyz[1] * diffx + a.velocity.xyz[0] * diffy) / determinant;
    return std::make_pair(paramA, paramB);
}

auto solvePart1(const std::vector<Hailstone>& hailstones) {
    const double minCoord = 200000000000000;
    const double maxCoord = 400000000000000;

    int count{0};
    for (auto i{0uz}; i < hailstones.size(); ++i) {
        for (auto j{i+1}; j < hailstones.size(); ++j) {
            auto intersection = computeIntersection2D(hailstones[i], hailstones[j]);
            if (intersection) {
                auto [paramA, paramB] = *intersection;
                auto coord = hailstones[i].getCoordAt(paramA);
                bool inBounds = (coord.xyz[0] >= minCoord && coord.xyz[0] <= maxCoord)
                             && (coord.xyz[1] >= minCoord && coord.xyz[1] <= maxCoord);
                bool inPast = (paramA < 0) || (paramB < 0);
                if (inBounds && !inPast)
                    ++count;
            }
        }
    }

    return count;
}

auto solvePart2(const std::vector<Hailstone>& hailstones) {
    if (hailstones.size() <= 3)
        throw std::runtime_error("Not enough hailstones");
    // should really rewrite everything with Eigen, including part 1...
    Eigen::Vector<double, 3> v0 = { hailstones[0].velocity.xyz[0], hailstones[0].velocity.xyz[1], hailstones[0].velocity.xyz[2] };
    Eigen::Vector<double, 3> v1 = { hailstones[1].velocity.xyz[0], hailstones[1].velocity.xyz[1], hailstones[1].velocity.xyz[2] };
    Eigen::Vector<double, 3> v2 = { hailstones[2].velocity.xyz[0], hailstones[2].velocity.xyz[1], hailstones[2].velocity.xyz[2] };
    Eigen::Vector<double, 3> p0 = { hailstones[0].position.xyz[0], hailstones[0].position.xyz[1], hailstones[0].position.xyz[2] };
    Eigen::Vector<double, 3> p1 = { hailstones[1].position.xyz[0], hailstones[1].position.xyz[1], hailstones[1].position.xyz[2] };
    Eigen::Vector<double, 3> p2 = { hailstones[2].position.xyz[0], hailstones[2].position.xyz[1], hailstones[2].position.xyz[2] };

    Eigen::Vector<double, 6> b;
    b.head<3>() = v0.cross(p0) - v1.cross(p1);
    b.tail<3>() = v0.cross(p0) - v2.cross(p2);
    Eigen::Matrix<double, 6, 6> A;
    A.block<3,3>(0,0) = (v0-v1).asSkewSymmetric();
    A.block<3,3>(0,3) = (p1-p0).asSkewSymmetric();
    A.block<3,3>(3,0) = (v0-v2).asSkewSymmetric();
    A.block<3,3>(3,3) = (p2-p0).asSkewSymmetric();

    Eigen::Vector<long, 6> solution = A.colPivHouseholderQr().solve(b).array().round().cast<long>();
    return solution(0) + solution(1) + solution(2);
}

int main() {
    std::println("AoC 2023: 24");

    try {
        auto hailstones = parseInput("../inputs/24.txt");
        std::println("Part 1: {}", solvePart1(hailstones));
        std::println("Part 2: {}", solvePart2(hailstones));
    }
    catch (std::exception& e) {
        std::println("Exception: {}", e.what());
    }

    return 0;
}