// Day8.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <regex>
#include <unordered_map>
#include <algorithm>
#include <numeric>

auto lines(const std::string& filename) {
    std::fstream fs(filename);
    std::string line;
    std::vector<std::string> myLines;
    while (std::getline(fs, line))
    {
        myLines.push_back(line);
    }

    return myLines;
}

struct day8_parse {
    std::string instructions;
    std::unordered_map<std::string, std::pair<std::string, std::string>> paths;
};

auto parse_day8() {
   
    std::vector<std::string> input = lines("day8_input.txt");

    std::regex re{ R"((\w+) = \((\w+), (\w+)\))" };

    day8_parse parse;
    parse.instructions = input.front();

    for (const auto& hand_str : input) {
        std::smatch sm; 
        if (std::regex_search(hand_str, sm, re)) {
            parse.paths.emplace(sm.str(1), std::pair{sm.str(2), sm.str(3) });
        }
    }

    return parse;
}

auto process_day8_part1(const day8_parse& parse) {

    std::vector<std::string> path{ "AAA" };

    int step = 0;
    while (path.back() != "ZZZ") {
        switch (parse.instructions[step++ % parse.instructions.size()]) {
            case 'L':
                path.push_back(parse.paths.at(path.back()).first); continue;
            case 'R':
                path.push_back(parse.paths.at(path.back()).second); continue;
            default:
                throw std::runtime_error("Unexpected instruction.");
        }
    }

    return path.size() - 1;
}



auto process_day8_part2(const day8_parse& parse) {

    

    auto next_path = [&](const auto& path, const auto& instruction) {
        switch (instruction) {
        case 'L':
            return parse.paths.at(path).first;
        case 'R':
            return parse.paths.at(path).second;
        default:
            throw std::runtime_error("Unexpected instruction.");
        }
    };

    std::vector<std::vector<std::string>> paths;
    for (const auto& p : parse.paths) {
        paths.emplace_back(std::vector{ p.first });
    }


    int step = 0;
    while (std::any_of(paths.begin(), paths.end(), [](const auto& path) { return path.back()[2] != 'Z'; })) {
        char instruction = parse.instructions[step++ % parse.instructions.size()];

        for (auto& p : paths) {
            if (p.back()[2] != 'Z'){
                p.push_back(next_path(p.back(), instruction));
            }
        }
    }

    std::unordered_map<std::string, unsigned long long> steps_to_next_Z;

    for (const auto& p : paths) {
        steps_to_next_Z[p.front()] = p.size() - 1;
    }


    std::vector<std::string> ghost_path;

    for (const auto& p : parse.paths) {
        if (p.first[2] == 'A') {
            ghost_path.push_back(p.first);
        }
    }

    auto least_steps = std::accumulate(ghost_path.begin(), ghost_path.end(), 1ull, [&steps_to_next_Z](auto lcm, const auto& p){
        return std::lcm(lcm, steps_to_next_Z.at(p));
    });



    return least_steps;
}

int main()
{
    auto day8_parse = parse_day8();

    auto num_steps = process_day8_part1(day8_parse);
    std::cout << "Day 8 part 1 number of steps: " << num_steps << std::endl;


    auto num_ghost_steps = process_day8_part2(day8_parse);
    std::cout << "Day 8 part 1 number of ghost steps: " << num_ghost_steps << std::endl;
}