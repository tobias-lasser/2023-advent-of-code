#include <print>
#include <string>
#include <string_view>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <ranges>
#include <queue>
#include <numeric>

enum class Type { flipflop, broadcaster, conjunction };

struct Module {
    Type type{Type::flipflop};
    std::vector<std::string> destinations;
    std::unordered_map<std::string, bool> inputs;
    bool on{false};

    [[nodiscard]] bool allInputsHigh() const {
        bool allHigh{true};
        for (const auto& [id, high] : inputs)
            allHigh &= high;
        return allHigh;
    }
};

struct Pulses {
    long low{0}, high{0};

    Pulses& operator+=(const Pulses& p) {
        low += p.low;
        high += p.high;
        return *this;
    }
};

void connectSources(std::unordered_map<std::string, Module>& modules) {
    std::unordered_set<std::string> conjunctions;
    for (const auto& [id, m] : modules)
        if (m.type == Type::conjunction) conjunctions.insert(id);

    for (const auto& [id, m] : modules) {
        for (const auto& dest : m.destinations) {
            if (conjunctions.contains(dest))
                modules[dest].inputs[id] = false;
        }
    }
}

auto parseInput(std::string_view fileName) {
    std::ifstream infile(fileName);
    if (!infile)
        throw std::runtime_error("Could not open file " + std::string(fileName));

    std::unordered_map<std::string, Module> modules;
    while (!infile.eof()) {
        std::string line;
        std::getline(infile, line);
        if (line.empty()) continue;

        Module m;
        std::string id;
        if (line.starts_with("broadcaster")) {
            id = "broadcaster";
            m.type = Type::broadcaster;
        }
        else {
            id = line.substr(1, line.find(' ')-1);
            m.type = line[0] == '%' ? Type::flipflop : Type::conjunction;
        }
        for (auto p : std::views::split(line.substr(line.find('>')+2), ',') | std::views::common) {
            auto psv{std::string_view(p)};
            if (auto res{psv.find_first_of(' ')}; res != std::string_view::npos)
                psv.remove_prefix(1);
            m.destinations.emplace_back(psv);
        }
        modules[id] = m;
    }
    connectSources(modules);

    return modules;
}

struct Action {
    std::string id;
    bool high{false};
    std::string source;
};

auto pressButton(std::unordered_map<std::string, Module>& modules, const std::string& checkHighForConj = "") {
    Pulses pulses;

    std::queue<Action> q;
    for (const auto& dest : modules["broadcaster"].destinations)
        q.emplace(dest, false, "broadcaster");
    pulses.low += static_cast<long>(modules["broadcaster"].destinations.size()) + 1; // +1 for initial button pulse

    while (!q.empty()) {
        auto [id, high, source] = q.front();
        q.pop();

        if (!modules.contains(id)) continue;

        Module& m = modules[id];
        if (m.type == Type::flipflop && !high) {
            if (m.on) {
                for (const auto& dest : m.destinations) {
                    ++pulses.low;
                    q.emplace(dest, false, id);
                }
            }
            else {
                for (const auto& dest : m.destinations) {
                    ++pulses.high;
                    q.emplace(dest, true, id);
                }
            }
            m.on = !m.on;
        }
        else if (m.type == Type::conjunction) {
            m.inputs[source] = high;
            if (m.allInputsHigh()) {
                for (const auto& dest : m.destinations) {
                    ++pulses.low;
                    q.emplace(dest, false, id);
                }
            }
            else {
                if (!checkHighForConj.empty() && checkHighForConj == id) // part 2: conjunction that fires high!
                    return std::make_pair(pulses, true);
                for (const auto &dest: m.destinations) {
                    ++pulses.high;
                    q.emplace(dest, true, id);
                }
            }
        }
    }

    return std::make_pair(pulses, false);
}

auto findConnectionTo(const std::string& id, const std::unordered_map<std::string, Module>& modules) {
    std::vector<std::string> origins;
    for (const auto& [k, m] : modules) {
        auto it = std::find(m.destinations.begin(), m.destinations.end(), id);
        if (it != m.destinations.end())
            origins.emplace_back(k);
    }
    return origins;
}

auto lcm(const std::vector<long>& numbers) {
    if (numbers.size() < 2) return 0l;
    long lcm = std::lcm(numbers[0], numbers[1]);
    for (std::size_t i{2}; i < numbers.size(); ++i)
        lcm = std::lcm(lcm, numbers[i]);
    return lcm;
}

auto solvePart1(std::unordered_map<std::string, Module> modules) {
    Pulses pulses;
    for (std::size_t i{0}; i < 1000; ++i) {
        auto [result, ignore] = pressButton(modules);
        pulses += result;
    }
    return pulses.low * pulses.high;
}

auto solvePart2(std::unordered_map<std::string, Module> modules) {
    auto modulesCopy = modules;
    std::string beforeRx = findConnectionTo("rx", modules).front();
    std::vector<std::string> conjsBeforeRx = findConnectionTo(beforeRx, modules);

    std::vector<long> buttonPressesForConj;
    buttonPressesForConj.reserve(conjsBeforeRx.size());

    for (const auto& id : conjsBeforeRx) {
        modules = modulesCopy;
        long buttonPresses{0};
        bool conjHigh{false};
        do {
            ++buttonPresses;
            auto [ignore, result] = pressButton(modules, id);
            if (result) conjHigh = true;
        } while (!conjHigh);
        buttonPressesForConj.push_back(buttonPresses);
    }

    return lcm(buttonPressesForConj);
}

int main() {
    std::println("AoC 2023: 20");

    try {
        auto modules = parseInput("../inputs/20.txt");
        std::println("Part 1: {}", solvePart1(modules));
        std::println("Part 2: {}", solvePart2(modules));
    }
    catch (std::exception& e) {
        std::println("Exception: {}", e.what());
    }

    return 0;
}