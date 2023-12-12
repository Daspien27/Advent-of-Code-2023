// Day12.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <regex>
#include <map>
#include <algorithm>
#include <numeric>
#include <queue>
#include <sstream>
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


struct record {
    std::string mask;
    std::deque<int> blocks;
};

struct day12_parse {
    std::vector<record> records;
};


std::deque<int> deque_of_ints(std::string list){
    std::regex re{ "\\d+" };
    
    auto re_begin = std::sregex_iterator(list.begin(), list.end(), re);
    auto re_end = std::sregex_iterator();
    std::deque<int> nums;
    for (std::sregex_iterator i = re_begin; i != re_end; ++i)
    {
        nums.push_back(std::stoi(i->str()));
    }

    return nums;
}

auto parse_day12() {
    auto input = lines("day12_input.txt");

    day12_parse parse{};
    
    std::regex re{ "([\\?\\.#]+) ((?:[|,]?\\d+)+)"};

    for (const auto& line : input) {
        std::smatch sm;
        std::regex_search(line, sm, re);
        parse.records.push_back(record{ .mask = sm.str(1), .blocks = deque_of_ints(sm.str(2)) });
    }

    return parse;
}

struct contains_n_lead {
    std::map<int, std::regex> cache;
    bool operator()(const std::string& s, int n){
        if (!cache.contains(n)){
            using namespace std::string_literals;
            std::stringstream ss;
            ss << "^[#\\?]{"s << n << "}([\\.\\?].*)?"s;
            cache.emplace(n, ss.str());
        }

        return std::regex_match(s, cache.at(n));
    }
};

long long num_arrangements(const record& r) {
    static contains_n_lead contains;
    
    if (r.mask == ""){
        return (r.blocks.empty())? 1 : 0;
    }

    if (r.mask.front() == '.') {
        return num_arrangements(record{ .mask = r.mask.substr(1), .blocks = r.blocks });
    }
    if (r.mask.front() == '#') {
        if (r.blocks.empty()) return 0;

        if (contains(r.mask, r.blocks.front()))
        {
            int r_mask_size = r.mask.size();
            record next_r{ .mask = r.mask.substr(std::min(r.blocks.front() + 1, r_mask_size)), .blocks = r.blocks };
            next_r.blocks.pop_front();
            return num_arrangements(next_r);
        }
        else 
        {
            return 0;
        }
    }

    if (r.mask.front() == '?') {
        record next_r_damaged = r;
        next_r_damaged.mask[0] = '#';
        record next_r_safe = r;
        next_r_safe.mask = r.mask.substr(1);

        return num_arrangements(next_r_damaged) + num_arrangements(next_r_safe);
    }

    throw std::runtime_error("Unexpected case in num_arrangements.");
}

auto process_day12_part1(const day12_parse& parse) {

    auto num_arrangement = std::transform_reduce(parse.records.begin(), parse.records.end(), 0, [](auto running_sum, auto x){
        //std::cout << x << std::endl;
        return running_sum + x;
        }, num_arrangements);

    return num_arrangement;
}

auto process_day12_part2(const day12_parse& parse) {

    day12_parse unfold = parse;

    for (int i = 0; i < unfold.records.size(); ++i) {
        auto& r = unfold.records[i];
        const auto& pr = parse.records[i];

        for (int j = 0; j < 4; ++j) {
            using namespace std::string_literals;
            r.mask = r.mask + "?"s + pr.mask;
            std::copy(pr.blocks.begin(), pr.blocks.end(), std::back_inserter(r.blocks));
        }
    }

    auto num_arrangement = std::transform_reduce(unfold.records.begin(), unfold.records.end(), 0ll, [i = 0](auto running_sum, auto x) mutable {
        std::cout << ++i << ": " <<  x << std::endl;
        return running_sum + x;
    }, [](const auto& r) {
        return num_arrangements(r); 
    });

    return num_arrangement;
}


int main()
{
    auto parse = parse_day12();
    auto num_arrangement = process_day12_part1(parse);
        std::cout << "Day 12 part 1 number of arrangements: " << num_arrangement << '\n';

    auto num_unfolded_arrangement = process_day12_part2(parse);
    std::cout << "Day 12 part 2 number of arrangements: " << num_unfolded_arrangement << '\n';
}