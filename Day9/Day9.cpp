// Day9.cpp : This file contains the 'main' function. Program execution begins and ends there.
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


using LL = long long;

auto parse_nums(const std::string& list_ints) {
    std::regex re{ "-?\\d+" };
    auto re_begin = std::sregex_iterator(list_ints.begin(), list_ints.end(), re);
    auto re_end = std::sregex_iterator();
    std::vector<LL> nums;
    for (std::sregex_iterator i = re_begin; i != re_end; ++i)
    {
        nums.push_back(std::stoll(i->str()));
    }

    return nums;
}

struct day9_parse {
    std::vector<std::vector<LL>> sequences;
};

auto parse_day9() {

    std::vector<std::string> input = lines("day9_input.txt");

    day9_parse parse;
    
    for (const auto& seq_str : input) {
        
        parse.sequences.push_back(parse_nums(seq_str));
       
    }

    return parse;
}

auto process_day9_part1(const day9_parse& parse) {


    auto num = std::transform_reduce(parse.sequences.begin(), parse.sequences.end(), 0ll, std::plus<LL>{}, [](const auto& seq){
        
        std::vector<std::vector<LL>> diffs;
        diffs.emplace_back(seq);


        while (std::any_of(diffs.back().begin(), diffs.back().end(), [](const auto n) { return n != 0; })) {
            
            
            diffs.emplace_back(diffs.back().size(), 0);

            const auto& diff_2nd_last = *(diffs.rbegin() + 1);
            auto& diff_last = diffs.back();


            std::adjacent_difference(diff_2nd_last.begin(), diff_2nd_last.end(), diff_last.begin());
            diff_last.erase(diff_last.begin());
        }

        
        auto extrapolate = std::accumulate(diffs.begin(), diffs.end(), 0ll, [](auto sum, const auto& diff_seq) {
            return sum + diff_seq.back();
        });

        return extrapolate;
        });

    return num;
}

auto process_day9_part2(const day9_parse& parse) {


    auto num = std::transform_reduce(parse.sequences.begin(), parse.sequences.end(), 0ll, std::plus<LL>{}, [](const auto& seq) {

        std::vector<std::vector<LL>> diffs;
        diffs.emplace_back(seq);


        while (std::any_of(diffs.back().begin(), diffs.back().end(), [](const auto n) { return n != 0; })) {


            diffs.emplace_back(diffs.back().size(), 0);

            const auto& diff_2nd_last = *(diffs.rbegin() + 1);
            auto& diff_last = diffs.back();


            std::adjacent_difference(diff_2nd_last.begin(), diff_2nd_last.end(), diff_last.begin());
            diff_last.erase(diff_last.begin());
        }


        auto extrapolate = std::accumulate(diffs.rbegin(), diffs.rend(), 0ll, [](auto sum, const auto& diff_seq) {
            return diff_seq.front() - sum;
            });

        return extrapolate;
        });

    return num;
}

int main()
{
    auto day9_parse = parse_day9();

    auto num = process_day9_part1(day9_parse);

    std::cout << "Day 9 part 1 num: " << num << std::endl;

    auto num2 = process_day9_part2(day9_parse);

    std::cout << "Day 9 part 2 num: " << num2 << std::endl;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
