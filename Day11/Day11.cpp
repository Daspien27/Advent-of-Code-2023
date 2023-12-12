// Day11.cpp : This file contains the 'main' function. Program execution begins and ends there.
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

enum class object {
    empty,
    galaxy,
};

struct index_pair {
    size_t x;
    size_t y;

    auto operator<=> (const index_pair&) const = default;
};

struct day11_parse {

    std::vector <std::vector<object>> universe;
    std::vector<index_pair> galaxy_locations;
};

auto parse_day11() {

    auto input = lines("day11_input.txt");

    day11_parse parse{};

    for (size_t i = 0; i < input.size(); ++i) {
        const auto& row = input[i];
        parse.universe.emplace_back(row.size(), object::empty);
        for (size_t j = 0; j < row.size(); ++j) {

            switch (row[j]) {
            case '.':  parse.universe[i][j] = object::empty; break;
            case '#':  
            {
                parse.universe[i][j] = object::galaxy; 
                parse.galaxy_locations.push_back({.x = i, .y = j });
            }break;
            }
        }
    }
    return parse;
}

auto process_day11_part1(const day11_parse& parse) {

    std::vector<int> extra_columns;
    std::vector<int> extra_rows;

    for (int i = 0; i < parse.universe.size(); ++i) {
        const auto& row = parse.universe[i];
        auto galaxy = std::find(row.begin(), row.end(), object::galaxy);
        if (galaxy == row.end()) {
            extra_rows.push_back(i);
        }
    }

    for (int j = 0; j < parse.universe[0].size(); ++j) {
        
        auto galaxy = std::find_if(parse.universe.begin(), parse.universe.end(), [&](const auto& row){
            return row[j] == object::galaxy;
        });
        
        if (galaxy == parse.universe.end()) {
            extra_columns.push_back(j);
        }
    }

    auto num_galaxies = parse.galaxy_locations.size();
    std::vector<std::vector<unsigned long long>> starhatten_distances(num_galaxies, std::vector<unsigned long long>(num_galaxies, 0ull));
    unsigned long long sum = 0;
    for (int i = 0; i < num_galaxies; ++i) {
        for (int j = i; j < num_galaxies; ++j) {

            if (i == j) continue;

            auto [min_x, max_x] = std::minmax(parse.galaxy_locations[i].x, parse.galaxy_locations[j].x);
            
            auto smallest_x_blank = std::lower_bound(extra_rows.begin(), extra_rows.end(), min_x);
            auto largest_x_blank = std::upper_bound(extra_rows.begin(), extra_rows.end(), max_x);
            auto number_x_blanks = std::distance(smallest_x_blank, largest_x_blank);
            int starhatten_x = max_x - min_x + number_x_blanks;
            
            auto [min_y, max_y] = std::minmax(parse.galaxy_locations[i].y, parse.galaxy_locations[j].y);
            auto smallest_y_blank = std::lower_bound(extra_columns.begin(), extra_columns.end(), min_y);
            auto largest_y_blank = std::upper_bound(extra_columns.begin(), extra_columns.end(), max_y);
            auto number_y_blanks = std::distance(smallest_y_blank, largest_y_blank);
            int starhatten_y = max_y - min_y + number_y_blanks;

            starhatten_distances[i][j] = starhatten_x+ starhatten_y;

            sum += starhatten_distances[i][j];
        }
    }



    return sum;
}

auto process_day11_part2(const day11_parse& parse, unsigned long long factor) {

    std::vector<int> extra_columns;
    std::vector<int> extra_rows;

    for (int i = 0; i < parse.universe.size(); ++i) {
        const auto& row = parse.universe[i];
        auto galaxy = std::find(row.begin(), row.end(), object::galaxy);
        if (galaxy == row.end()) {
            extra_rows.push_back(i);
        }
    }

    for (int j = 0; j < parse.universe[0].size(); ++j) {

        auto galaxy = std::find_if(parse.universe.begin(), parse.universe.end(), [&](const auto& row) {
            return row[j] == object::galaxy;
            });

        if (galaxy == parse.universe.end()) {
            extra_columns.push_back(j);
        }
    }

    auto num_galaxies = parse.galaxy_locations.size();
    std::vector<std::vector<unsigned long long>> starhatten_distances(num_galaxies, std::vector<unsigned long long>(num_galaxies, 0ull));
    unsigned long long sum = 0;
    for (int i = 0; i < num_galaxies; ++i) {
        for (int j = i; j < num_galaxies; ++j) {

            if (i == j) continue;

            auto [min_x, max_x] = std::minmax(parse.galaxy_locations[i].x, parse.galaxy_locations[j].x);

            auto smallest_x_blank = std::lower_bound(extra_rows.begin(), extra_rows.end(), min_x);
            auto largest_x_blank = std::upper_bound(extra_rows.begin(), extra_rows.end(), max_x);
            auto number_x_blanks = std::distance(smallest_x_blank, largest_x_blank);
            int starhatten_x = max_x - min_x + number_x_blanks * (factor - 1);

            auto [min_y, max_y] = std::minmax(parse.galaxy_locations[i].y, parse.galaxy_locations[j].y);
            auto smallest_y_blank = std::lower_bound(extra_columns.begin(), extra_columns.end(), min_y);
            auto largest_y_blank = std::upper_bound(extra_columns.begin(), extra_columns.end(), max_y);
            auto number_y_blanks = std::distance(smallest_y_blank, largest_y_blank);
            unsigned long long starhatten_y = max_y - min_y + number_y_blanks * (factor - 1);

            starhatten_distances[i][j] = starhatten_x + starhatten_y;

            sum += starhatten_distances[i][j];
        }
    }



    return sum;
}

int main()
{
    auto day11_parse = parse_day11();
    auto sum = process_day11_part1(day11_parse);
    std::cout << "Day 11 part 1 sum: " << sum << std::endl;

    auto super_sum = process_day11_part2(day11_parse, 1'000'000ull);
    std::cout << "Day 11 part 2 sum: " << super_sum << std::endl;
}