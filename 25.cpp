#include <print>
#include <string>
#include <string_view>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <ranges>

using Edges = std::unordered_set<std::string>;
using Graph = std::unordered_map<std::string, Edges>;

auto parseInput(std::string_view fileName) {
    std::ifstream infile(fileName);
    if (!infile)
        throw std::runtime_error("Could not open file " + std::string(fileName));

    Graph nodes;
    while (!infile.eof()) {
        std::string line;
        std::getline(infile, line);
        if (line.empty()) continue;

        auto itColon = line.find_first_of(':');
        std::string id = line.substr(0, itColon);
        auto edgeStr = line.substr(itColon + 2);
        auto edges =  edgeStr | std::views::split(' ') | std::ranges::to<std::vector<std::string>>();

        for (const auto& e : edges) {
            nodes[id].insert(e);
            nodes[e].insert(id);
        }
    }

    return nodes;
}

auto getNoConnectionsFromPartition(const Graph& graph, const std::unordered_set<std::string>& partition) {
    std::unordered_map<std::string, int> noConnections;
    for (const auto& id: partition) {
        for (const auto& e : graph.at(id))
            if (!partition.contains(e))
                ++noConnections[id];
    }
    return noConnections;
}

auto getSumOfConnections(const std::unordered_map<std::string, int>& noConnections) {
    long sum{0};
    for (const auto& [id, count] : noConnections)
        sum += count;
    return sum;
}

auto solvePart1(const Graph& graph) {
    // solution inspired by Reddit
    std::unordered_set<std::string> partition1;
    for (const auto& [id, node] : graph)
            partition1.insert(id);

    auto noConnections = getNoConnectionsFromPartition(graph, partition1);
    while (getSumOfConnections(noConnections) != 3) {
        if (noConnections.empty())
            partition1.erase(partition1.begin());
        else {
            auto itMax = std::max_element(noConnections.begin(), noConnections.end(),
                                          [](const auto &a, const auto &b) { return a.second < b.second; });
            partition1.erase(itMax->first);
        }
        noConnections = getNoConnectionsFromPartition(graph, partition1);
    }

    auto sizePartition1 = partition1.size();
    auto sizePartition2 = graph.size() - sizePartition1;
    return sizePartition1 * sizePartition2;
}

int main() {
    std::println("AoC 2023: 25");

    try {
        auto graph = parseInput("../inputs/25.txt");
        std::println("Part 1: {}", solvePart1(graph));
    }
    catch (std::exception& e) {
        std::println("Exception: {}", e.what());
    }

    return 0;
}
