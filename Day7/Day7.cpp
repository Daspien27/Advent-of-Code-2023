// Day7.cpp : This file contains the 'main' function. Program execution begins and ends there.
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

struct day7_parse {
    std::vector <std::pair<std::string, int>> hands;
};

auto parse_day7() {
    std::vector<std::string> input = lines("day7_input.txt");

    std::regex re{ R"((\w+) (\d+))" };
    
    day7_parse parse;

    for (const auto& hand_str : input) {

        std::vector<std::unordered_map<std::string, int>> rounds;
        std::smatch sm; std::regex_search(hand_str, sm, re);

        parse.hands.emplace_back(sm.str(1), std::stoi(sm.str(2)));
    }

    return parse;
}


enum class hand_type {
    high_card,
    one_pair,
    two_pair,
    three_of_a_kind,
    full_house,
    four_of_a_kind,
    five_of_a_kind,
};

struct ranking {
    hand_type type;
    int card1;
    int card2;
    int card3;
    int card4;
    int card5;

    int location;
    auto operator<=>(const ranking&) const = default;
};

auto process_day7_part1(const auto& day7) {

    std::vector<ranking> rankings;

    std::transform(day7.hands.begin(), day7.hands.end(), std::back_inserter(rankings), [i = 0](const auto& hand) mutable{
    
        std::unordered_map<char, int> counts;
        for (auto c : hand.first) {
            ++counts[c];
        }

        auto make_ranking = [&hand, &i](hand_type ht) {
            auto valueof = [](char c) {
                switch (c) {
                case '2': return 2;
                case '3': return 3;
                case '4': return 4;
                case '5': return 5;
                case '6': return 6;
                case '7': return 7;
                case '8': return 8;
                case '9': return 9;
                case 'T': return 10;
                case 'J': return 11;
                case 'Q': return 12;
                case 'K': return 13;
                case 'A': return 14;
                default: throw std::runtime_error("Unexpected card value");
                }
            };
            
            return ranking{ .type = ht,
                .card1 = valueof(hand.first[0]),
                .card2 = valueof(hand.first[1]),
                .card3 = valueof(hand.first[2]),
                .card4 = valueof(hand.first[3]),
                .card5 = valueof(hand.first[4]),
                .location = i++
            };
        };

        switch (counts.size()) {
        case 1: return make_ranking(hand_type::five_of_a_kind);
        case 2: 
            
            if (counts.begin()->second == 1 || counts.begin()->second == 4) return make_ranking(hand_type::four_of_a_kind);
            else return make_ranking(hand_type::full_house);
            
        case 3: 
            if(std::find_if(counts.begin(), counts.end(), [](const auto& c){ return c.second == 3; }) != counts.end()) return make_ranking(hand_type::three_of_a_kind);
            return make_ranking(hand_type::two_pair);

        case 4: return make_ranking(hand_type::one_pair);
        case 5: return make_ranking(hand_type::high_card);
        default: throw std::runtime_error("Hand is too big");
        }
    });

    std::sort(rankings.begin(), rankings.end());

    int sum = 0;
    for (int i = 0; i < rankings.size(); ++i) {
        sum += day7.hands[rankings[i].location].second * (i + 1);
    }

    return sum;
}


auto process_day7_part2(const auto& day7) {

    std::vector<ranking> rankings;

    std::transform(day7.hands.begin(), day7.hands.end(), std::back_inserter(rankings), [i = 0](const auto& hand) mutable{

        std::unordered_map<char, int> counts;
        for (auto c : hand.first) {
            ++counts[c];
        }

        auto determine_ranking_with_counts = [&](auto& counts){

            auto make_ranking = [&hand, &i](hand_type ht) {
                auto valueof = [](char c) {
                    switch (c) {
                    case 'J': return 1;
                    case '2': return 2;
                    case '3': return 3;
                    case '4': return 4;
                    case '5': return 5;
                    case '6': return 6;
                    case '7': return 7;
                    case '8': return 8;
                    case '9': return 9;
                    case 'T': return 10;
                        //case 'J': return 11;
                    case 'Q': return 12;
                    case 'K': return 13;
                    case 'A': return 14;
                    default: throw std::runtime_error("Unexpected card value");
                    }
                };

                return ranking{ .type = ht,
                    .card1 = valueof(hand.first[0]),
                    .card2 = valueof(hand.first[1]),
                    .card3 = valueof(hand.first[2]),
                    .card4 = valueof(hand.first[3]),
                    .card5 = valueof(hand.first[4]),
                    .location = i++
                };
            };

            switch (counts.size()) {
            case 1: return make_ranking(hand_type::five_of_a_kind);
            case 2:

                if (counts.begin()->second == 1 || counts.begin()->second == 4) return make_ranking(hand_type::four_of_a_kind);
                else return make_ranking(hand_type::full_house);

            case 3:
                if (std::find_if(counts.begin(), counts.end(), [](const auto& c) { return c.second == 3; }) != counts.end()) return make_ranking(hand_type::three_of_a_kind);
                return make_ranking(hand_type::two_pair);

            case 4: return make_ranking(hand_type::one_pair);
            case 5: return make_ranking(hand_type::high_card);
            default: throw std::runtime_error("Hand is too big");
            }
        };


        if (auto contains_jokers = counts.count('J'); contains_jokers && counts.at('J')<5) {

            auto jokers = counts.extract('J');

            auto most = std::max_element(counts.begin(), counts.end(), [](const auto& a, const auto& b) {
                return a.second < b.second;
            });

            counts[most->first] += jokers.mapped();

         //   if (jokers.mapped() + most->second > 3) {
         //       counts[most->first] += jokers.mapped();
         //   }
         //   else if (jokers.mapped() + most->second == 3) { 
         //       //1 or 2 are pairs, if all ones just add 2 to a count since 3 of kind is better than 2 pair
         //       // if 2* 2* J we can have a full house, just add 1 to a pair
         //       // if 2* 1* 1* J we can have 3 of a kind
         //       // J + most = 2, i.e. all were 1, just add one to the first count
         //       counts[most->first] += jokers.mapped();
         //   }
        }
        
        return determine_ranking_with_counts(counts);

    });

    std::sort(rankings.begin(), rankings.end());

    int sum = 0;
    for (int i = 0; i < rankings.size(); ++i) {
        sum += day7.hands[rankings[i].location].second * (i + 1);
    }

    return sum;
}


int main()
{
    auto parsed_day7 = parse_day7();

    auto total_winnings = process_day7_part1(parsed_day7);

    std::cout << "Day 7 part1 total winnings: " << total_winnings << std::endl;

    auto total_winnings_with_jokers = process_day7_part2(parsed_day7);

    std::cout << "Day 7 part2 total winnings with jokers: " << total_winnings_with_jokers << std::endl;
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
