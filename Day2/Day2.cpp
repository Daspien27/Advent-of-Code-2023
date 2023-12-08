// Day2.cpp : This file contains the 'main' function. Program execution begins and ends there.
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


auto parse_day2_part1() {
    std::vector<std::string> input = lines("day2_input.txt");

    std::regex re {R"(((?: |, )(\d+ \w+))+)" };
    
    std::vector<
        std::vector<std::unordered_map<std::string, int>>> games;

    for (const auto& game_str : input){

    std::vector<std::unordered_map<std::string, int>> rounds;
    std::string remaining = game_str;
    for (std::smatch sm; std::regex_search(remaining, sm, re);)
    {
        auto round = sm.str();
        remaining = sm.suffix();
        

        const auto track_round = [](const auto& round){
                static const std::regex play_re{ R"((\d+) (\w+))" };
                std::unordered_map<std::string, int> r;
                std::string remaining = round;
                for (std::smatch sm; std::regex_search(remaining, sm, play_re);)
                {
                    r[sm.str(2)] += std::stoi(sm.str(1));
                    remaining = sm.suffix();
                }

                return r;
            };

            rounds.push_back(track_round(round));
        }
    games.push_back(rounds);
    }

    return games;
}

auto process_day2_part1(auto data, const std::unordered_map<std::string, int>& starting_state) {
  
    int id_sum = 0;

    const auto valid_game = [&starting_state](const auto& game) {
        return std::all_of(game.begin(), game.end(), [&](const auto& round) {
            return std::all_of(round.begin(), round.end(), [&starting_state](const auto& set) {
                return set.second <= starting_state.at(set.first);
                });
            });
    };

    for (int i = 0; i < data.size(); ++i) {

        if (valid_game(data[i])) {

            id_sum += 1 + i;
        }

    }

    

    return id_sum;
}


auto process_day2_part2(auto data) {


    auto power = [](const auto& game) {

        std::unordered_map<std::string, int> minimum_set;

        for (const auto& r : game) {

            for (const auto& s : r) {
                if (minimum_set[s.first] < s.second) {
                    minimum_set[s.first] = s.second;
                }
            }
        }

        return std::accumulate(minimum_set.begin(), minimum_set.end(), 1, [](const auto& product, const auto& s) {
            return s.second * product;
            });
    };

    auto power_sum = std::transform_reduce(data.begin(), data.end(), 0ll, std::plus<long long>{}, power);

    return power_sum;
}

int main()
{
    auto parsed_day2_part1 = parse_day2_part1();

    auto num = process_day2_part1(parsed_day2_part1, {{"red", 12},{"green", 13} ,{"blue", 14}});

    std::cout << "Day 2 part 1 ID sum: " << num << "\n";

    auto num2 = process_day2_part2(parsed_day2_part1);
    
    std::cout << "Day 2 part 2 power sum: " << num2 << "\n";

    return 0;
}