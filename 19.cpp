#include <print>
#include <ranges>
#include <vector>
#include <string>
#include <string_view>
#include <fstream>
#include <utility>
#include <unordered_map>
#include <optional>
#include <functional>

struct Part {
    std::unordered_map<char, int> vars;
    Part() : vars{ {'x', 0}, {'m', 0}, {'a', 0}, {'s', 0} } {}
};

struct Rule {
    char var{' '};
    char op{' '};
    int value{0};
    std::string id;

    [[nodiscard]] std::optional<std::string> evaluate(const Part& p) const {
        if (op == ' ') return id;
        if (op == '<') return (p.vars.at(var) < value) ? std::make_optional(id) : std::nullopt;
        if (op == '>') return (p.vars.at(var) > value) ? std::make_optional(id) : std::nullopt;
        return std::nullopt;
    }
};

struct Workflow {
    std::vector<Rule> rules;
};


auto parseInput(std::string_view fileName) {
    std::ifstream infile(fileName);
    if (!infile)
        throw std::runtime_error("Could not open file " + std::string(fileName));

    std::unordered_map<std::string, Workflow> workflows;
    std::vector<Part> parts;
    bool inWorkflow{true};
    while (!infile.eof()) {
        std::string line;
        std::getline(infile, line);
        if (line.empty()) {
            inWorkflow = false;
            continue;
        }
        std::string_view pstr{line};

        if (inWorkflow) { // parse rules
            Workflow w;
            auto it = pstr.find_first_of('{');
            if (it == std::string_view::npos) throw std::runtime_error("Invalid rule: " + line);
            std::string id{line.substr(0, it)};
            pstr = pstr.substr(it+1, pstr.size() - 2 - it);
            auto pStrParts = pstr | std::views::split(',') | std::ranges::to<std::vector<std::string>>();
            for (const auto& psp : pStrParts) {
                Rule r;
                auto itOp = psp.find_first_of("<>");
                if (itOp == std::string_view::npos)
                    r.id = psp;
                else {
                    r.var = psp[0];
                    r.op = psp[itOp];
                    auto itColon = psp.find_first_of(':');
                    if (itColon == std::string_view::npos) throw std::runtime_error("Invalid rule: " + line);
                    r.value = std::stoi(psp.substr(itOp+1));
                    r.id = psp.substr(itColon+1);
                }
                w.rules.push_back(r);
            }
            workflows[id] = w;
        }
        else { // parse parts
            pstr = pstr.substr(1, pstr.size() - 2);
            auto pstrParts = pstr | std::views::split(',') | std::ranges::to<std::vector<std::string>>();
            if (pstrParts.size() != 4)
                throw std::runtime_error("Invalid part definition: " + line);
            Part p;
            p.vars.at('x') = std::stoi(pstrParts[0].substr(2));
            p.vars.at('m') = std::stoi(pstrParts[1].substr(2));
            p.vars.at('a') = std::stoi(pstrParts[2].substr(2));
            p.vars.at('s') = std::stoi(pstrParts[3].substr(2));
            parts.push_back(p);
        }
    }

    return std::make_pair(workflows, parts);
}

auto solvePart1(const std::unordered_map<std::string, Workflow>& workflows, const std::vector<Part>& parts) {
    int sumParts{0};
    for (const auto& p : parts) {
        std::string id{"in"};
        while (id != "A" && id != "R") {
            auto w{workflows.at(id)};
            for (const auto &rule: w.rules) {
                if (auto res{rule.evaluate(p)}; res) {
                    id = *res;
                    break;
                }
            }
        }
        if (id == "A")
            sumParts += p.vars.at('x') + p.vars.at('m') + p.vars.at('a') + p.vars.at('s');
    }
    return sumParts;
}

struct Range {
    unsigned long long min{1}, max{4000};
};

struct RatingRanges {
    std::unordered_map<char, Range> r;
    RatingRanges() : r{ {'x', {1, 4000}}, {'m', {1, 4000}}, {'a', {1, 4000}}, {'s', {1, 4000}} } {}

    [[nodiscard]] unsigned long long countCombinations() {
        auto res = (r.at('x').max - r.at('x').min + 1) *
                   (r.at('m').max - r.at('m').min + 1) *
                   (r.at('a').max - r.at('a').min + 1) *
                   (r.at('s').max - r.at('s').min + 1);
        return res;
    }
};

void countAccepted(const std::unordered_map<std::string, Workflow>& workflows, const std::string& currentWorkflow, RatingRanges ranges, unsigned long long& result) {
    if (currentWorkflow == "R") return;
    if (currentWorkflow == "A") {
        result += ranges.countCombinations();
        return;
    }

    auto w{workflows.at(currentWorkflow)};
    unsigned long long combinations{0};
    for (const auto& rule : w.rules) {
        if (rule.op == ' ') {
            countAccepted(workflows, rule.id, ranges, result);
            break;
        }
        else if (rule.op == '<') {
            auto max = ranges.r.at(rule.var).max;
            ranges.r.at(rule.var).max = rule.value - 1;
            if (ranges.r.at(rule.var).min <= ranges.r.at(rule.var).max)
                countAccepted(workflows, rule.id, ranges, result);
            ranges.r.at(rule.var).min = rule.value;
            ranges.r.at(rule.var).max = max;
        } else { // op == '>'
            auto min = ranges.r.at(rule.var).min;
            ranges.r.at(rule.var).min = rule.value + 1;
            if (ranges.r.at(rule.var).min <= ranges.r.at(rule.var).max)
                countAccepted(workflows, rule.id, ranges, result);
            ranges.r.at(rule.var).min = min;
            ranges.r.at(rule.var).max = rule.value;
        }
        if (ranges.r.at(rule.var).min > ranges.r.at(rule.var).max)
            break;
    }
}

auto solvePart2(const std::unordered_map<std::string, Workflow>& workflows) {
    unsigned long long result{0};
    countAccepted(workflows, "in", RatingRanges{}, result);
    return result;
}

int main() {
    std::println("AoC 2023: 19");

    try {
        auto [workflows, parts] = parseInput("../inputs/19.txt");
        std::println("Part 1: {}", solvePart1(workflows, parts));
        std::println("Part 2: {}", solvePart2(workflows));
    }
    catch (std::exception& e) {
        std::println("Exception: {}", e.what());
    }

    return 0;
}