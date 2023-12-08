// Day 4.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>
#include <algorithm>
#include <set>
#include <regex>
#include <string>
#include <fstream>
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


auto parse_ints(const std::string& list_ints) {
    std::regex re{ "\\d+" };
    auto re_begin = std::sregex_iterator(list_ints.begin(), list_ints.end(), re);
    auto re_end = std::sregex_iterator();
    std::vector<int> matches;
    for (std::sregex_iterator i = re_begin; i != re_end; ++i)
    {
        matches.push_back(std::stoi(i->str()));
    }

    return matches;
}

auto parse_day4_part1() {
    std::vector<std::string> input = lines("day4_input.txt");

    std::regex re{ R"(Card\W+\d+:(.*) \|(.*))" };
    
    std::vector<std::pair<std::vector<int>, std::vector<int>>> cards;
    
    for (const auto& card : input){
        std::smatch sm; 
        std::regex_search(card, sm, re);
        cards.emplace_back(parse_ints(sm.str(1)), parse_ints(sm.str(2)));
    }

    return cards;
}

auto process_day4_part1(const auto& cards) {


    int points = std::transform_reduce(cards.begin(), cards.end(), 0, std::plus<int>{}, [](const auto& c) {
        std::set<int> winning_numbers(c.first.begin(), c.first.end());
        std::set<int> numbers(c.second.begin(), c.second.end());
            
        auto count = std::count_if(numbers.begin(), numbers.end(), [&](const auto& n) {
                return winning_numbers.contains(n);
            });

        return count ? 1 << (count - 1) : 0;
        });

    return points;
}


auto process_day4_part2(const auto& cards) {


    std::vector<int> copies_count(cards.size(), 0);
    std::vector<int> card_count(cards.size(), 1);
    
    std::transform(cards.begin(), cards.end(), copies_count.begin(), [](const auto& c) {
        std::set<int> winning_numbers(c.first.begin(), c.first.end());
        std::set<int> numbers(c.second.begin(), c.second.end());

        auto count = std::count_if(numbers.begin(), numbers.end(), [&](const auto& n) {
            return winning_numbers.contains(n);
            });

        return count;
     });

    for (int i = 0; i < cards.size(); i++) {
        int count = copies_count[i];
        for (int j = 1; j <= count && i+j < cards.size();++j) {
            card_count[i+j] += card_count[i];
        }
    }

    return std::accumulate(card_count.begin(), card_count.end(), 0);
}




int main()
{
    auto cards = parse_day4_part1();
    
    int points = process_day4_part1(cards);

    std::cout << "Day 4 part 1 points sum: " << points << "\n";

    int points2 = process_day4_part2(cards);

    std::cout << "Day 4 part 2 points sum: " << points2 << "\n";
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
