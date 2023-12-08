// Day5.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>
#include <algorithm>
#include <set>
#include <regex>
#include <string>
#include <fstream>
#include <numeric>
#include <map>

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







using ULL = unsigned long long;

struct day5_range {
    ULL destination_range_start;
    ULL source_range_start;
    ULL range_length;
};

struct day5_maps {

    std::vector<ULL> seeds;

    const static std::map<std::string, std::string> map_links;

    std::map<std::string, std::vector<day5_range>> maps;
    
    std::vector<day5_range> seed_to_soil;
    std::vector<day5_range> soil_to_fertilizer;
    std::vector<day5_range> fertilizer_to_water;
    std::vector<day5_range> water_to_light;
    std::vector<day5_range> light_to_temperature;
    std::vector<day5_range> temperature_to_humidity;
    std::vector<day5_range> humidity_to_location;
};

const std::map<std::string, std::string> day5_maps::map_links = {
        {"seed","soil"},
        {"soil","fertilizer"},
        {"fertilizer","water"},
        {"water","light"},
        {"light","temperature"},
        {"temperature","humidity"},
        {"humidity","location"} };


auto parse_nums(const std::string& list_ints) {
    std::regex re{ "\\d+" };
    auto re_begin = std::sregex_iterator(list_ints.begin(), list_ints.end(), re);
    auto re_end = std::sregex_iterator();
    std::vector<ULL> nums;
    for (std::sregex_iterator i = re_begin; i != re_end; ++i)
    {
        nums.push_back(std::stoull(i->str()));
    }

    return nums;
}

auto parse_day5(const std::string& file) {
    std::fstream fs(file);
    fs.unsetf(std::ios_base::skipws);
    auto file_contents = std::string(std::istream_iterator<char>(fs), std::istream_iterator<char>() );

    day5_maps parse;

    //seeds
    std::regex seed_re("^seeds: .*");
    std::smatch sm;
    std::regex_search(file_contents, sm, seed_re);
    parse.seeds = parse_nums(sm.str());

    auto insert_parse_map = [&](const std::string& map_name) {
        //bullshit c++ regex workaround
        auto file_contents_mangled = file_contents;
        std::replace(file_contents_mangled.begin(), file_contents_mangled.end(), '\n', '~');

        std::string map_re_string = map_name + " map:.*?~(~|$)";

        std::regex map_re(map_re_string);
        std::smatch sm;
        std::regex_search(file_contents_mangled, sm, map_re);
        std::string match = sm.str();

        std::regex re ("(\\d+) (\\d+) (\\d+)");
        auto re_begin = std::sregex_iterator(match.begin(), match.end(), re);
        auto re_end = std::sregex_iterator();

        std::vector<day5_range> ranges;

        for (std::sregex_iterator i = re_begin; i != re_end; ++i)
        {
            parse.maps[map_name].emplace_back(std::stoull(i->str(1)), std::stoull(i->str(2)), std::stoull(i->str(3)));
        }

        return ranges;
    };

    insert_parse_map("seed-to-soil");
    insert_parse_map("soil-to-fertilizer");
    insert_parse_map("fertilizer-to-water");
    insert_parse_map("water-to-light");
    insert_parse_map("light-to-temperature");
    insert_parse_map("temperature-to-humidity");
    insert_parse_map("humidity-to-location");

    return parse;
}

auto process_day5_part1(const auto& day5_state) {
    auto day5_state_sort = day5_state;

    for (auto& [map_key, ranges] : day5_state_sort.maps) {
        std::sort(ranges.begin(), ranges.end(), [](const auto& lhs, const auto& rhs) {
            return lhs.source_range_start < rhs.source_range_start;
        });
    }

    auto lowest_location_number = std::transform_reduce(day5_state_sort.seeds.begin(), day5_state_sort.seeds.end(), std::numeric_limits<ULL>::max(), [](auto x, auto y) { return std::min(x, y); }, [&](const auto& seed) -> ULL {
        
        std::string start = "seed";
        auto wip_seed = seed;
        while (start != "location")
        {
            std::string map_string = start + "-to-" + day5_state_sort.map_links.at(start);
            auto& map = day5_state_sort.maps[map_string];

            auto lb = std::lower_bound(map.begin(), map.end(), wip_seed, [](const day5_range& range, auto seed) {
                return range.source_range_start <= seed;
                });

            if (lb != map.begin()) {
                auto pp = std::prev(lb);
                if (pp->source_range_start <= wip_seed && wip_seed < pp->source_range_start + pp->range_length) {
                    auto diff = wip_seed - pp->source_range_start;
                    wip_seed = pp->destination_range_start + diff;
                }
            }

            start = day5_state_sort.map_links.at(start);
        }
        return wip_seed;
    });



    return lowest_location_number;
}

auto process_day5_part2(const auto& day5_state) {
    auto day5_state_sort = day5_state;

    for (auto& [map_key, ranges] : day5_state_sort.maps) {
        std::sort(ranges.begin(), ranges.end(), [](const auto& lhs, const auto& rhs) {
            return lhs.source_range_start < rhs.source_range_start;
            });

        ULL min_seed = 0;
        ULL max_seed = std::numeric_limits<ULL>::max();
        ULL seed = 0;
        std::vector<day5_range> new_map;
        auto iter = ranges.begin();
        while (iter != ranges.end() && seed != max_seed) {

            if (seed < iter->source_range_start) {
                new_map.emplace_back(seed, seed, iter->source_range_start - seed);
            }

            new_map.push_back(*iter);
            seed = iter->source_range_start + iter->range_length;
            ++iter;
        }
        new_map.emplace_back(seed, seed, max_seed - seed);
        ranges = new_map;
    }

    //part 2 twist
    std::vector<std::pair<ULL, ULL>> new_seeds;
    for (int i = 0; i < day5_state.seeds.size() / 2 + 1; i += 2) {
        new_seeds.emplace_back(day5_state.seeds[i], day5_state.seeds[i + 1]);
    }


    auto num_seeds = std::transform_reduce(new_seeds.begin(), new_seeds.end(), 0, std::plus<int>{}, [&](const auto& a) {
       
        return a.second;
    });

    std::cout << "Finding smallest location out of " << num_seeds << " seeds:\n";

    auto lowest_location_number = std::transform_reduce(new_seeds.begin(), new_seeds.end(), std::numeric_limits<ULL>::max(), [](auto x, auto y) { return std::min(x, y); }, [&](const auto& seed_range_pair) -> ULL{
       

        std::vector < std::pair<ULL, ULL>> ranges;
        std::vector < std::pair<ULL, ULL>> new_ranges;
        std::vector<std::pair<ULL, ULL>> points;
        ranges.push_back(seed_range_pair);


        std::string start = "seed";
        while (start != "location")
        {
            std::string map_string = start + "-to-" + day5_state_sort.map_links.at(start);
            auto& map = day5_state_sort.maps[map_string];
            new_ranges.clear();
            while (!ranges.empty()){
                points.clear();
                auto r = ranges.back();
                ranges.pop_back();
                auto lb = std::lower_bound(map.begin(), map.end(), r.first, [](const day5_range& range, auto seed) {
                                return range.source_range_start + range.range_length < seed;
                                });

                auto ub = std::upper_bound(map.begin(), map.end(), r.first+r.second, [](auto seed, const day5_range& range) {
                    return seed < range.source_range_start;
                    });

                auto end_seed = r.first + r.second;
                auto seed = r.first;
                points.emplace_back(seed, lb->destination_range_start + seed - lb->source_range_start);
                
                for (auto iter = lb; iter != ub;  ++iter) {

                    if (seed < iter->source_range_start && iter->source_range_start < end_seed) {
                        points.emplace_back(iter->source_range_start, iter->destination_range_start);
                    }

                    if ( seed < iter->source_range_start + iter->range_length && iter->source_range_start + iter->range_length < end_seed) {
                        points.emplace_back(iter->source_range_start + iter->range_length, iter->destination_range_start+ iter->range_length);
                    }
                    else {
                        points.emplace_back(end_seed, iter->destination_range_start + end_seed - iter->source_range_start);
                    }
                }

                for (int i = 0; i < points.size() - 1; i++) {
                    auto anchor = points[i].second;
                    auto diff = points[i + 1].first - points[i].first;

                    if (diff > 0){
                    new_ranges.emplace_back(anchor, diff);
                    }
                }
            }

            std::sort(new_ranges.begin(), new_ranges.end(), [](const auto& lhs, const auto& rhs) { return lhs.first < rhs.first; });
            ranges = new_ranges;
            start = day5_state_sort.map_links.at(start);
        }


        return ranges.front().first;

        });



    return lowest_location_number;
}

int main()
{
    auto day5_state = parse_day5("day5_input.txt");

    auto lowest_location_number = process_day5_part1(day5_state);
    std::cout << "Day 5 part 1 lowest_location_number: " << lowest_location_number << "\n";

    auto lowest_location_number2 = process_day5_part2(day5_state);
    std::cout << "Day 5 part 2 lowest_location_number: " << lowest_location_number2 << "\n";
    return 0;
}