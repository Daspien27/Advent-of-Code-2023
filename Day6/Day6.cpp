// Day6.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <algorithm>
#include <numeric>
#include <vector>
#include <regex>
#include <string>
#include <fstream>
#include <sstream>

using ULL = unsigned long long;

struct day6_parse {
    std::vector<ULL> time;
    std::vector<ULL> distance;
};

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

auto parse_day6(const std::string& file) {
    std::fstream fs(file);
    fs.unsetf(std::ios_base::skipws);
    auto file_contents = std::string(std::istream_iterator<char>(fs), std::istream_iterator<char>());

    day6_parse parse;

    std::smatch sm;

    std::regex time_re("Time:.*");
    std::regex_search(file_contents, sm, time_re);
    parse.time = parse_nums(sm.str());


    std::regex distance_re("Distance:.*");
    std::regex_search(file_contents, sm, distance_re);
    parse.distance = parse_nums(sm.str());

    return parse;
}

auto process_day6_part1(const day6_parse& input) -> ULL {

    auto factor = std::transform_reduce(input.time.begin(), input.time.end(), input.distance.begin(), 1ull, std::multiplies<ULL>{}, [](const auto& t, const auto& d) -> ULL {
       
            //quadratic formula = (t-x)*x-d=0 ===> -x^2+xt-d=0 ===>   x = (-t +- sqrt(t^2 - 4*d)) / 2

            auto discriminant = t*t - 4 * d;
            auto sqrt_discriminant = std::sqrt(discriminant);
            ULL lx = std::ceil((t - sqrt_discriminant)/2 + 0.000001);
            ULL ux = std::floor((t + sqrt_discriminant)/2 - 0.000001);
            auto r = ux - lx + 1;
            return r;
        });


    return factor;
}

auto process_day6_part2(const day6_parse& input) -> ULL {

    std::stringstream time_ss;
    std::stringstream distance_ss;

    for (int i = 0; i < input.time.size(); ++i) {
        time_ss << input.time[i];
        distance_ss << input.distance[i];
    }

    auto long_time = std::stoull(time_ss.str());
    auto long_distance = std::stoull(distance_ss.str());

    auto number_of_ways = [](const auto& t, const auto& d) -> ULL {

        //quadratic formula = (t-x)*x-d=0 ===> -x^2+xt-d=0 ===>   x = (-t +- sqrt(t^2 - 4*d)) / 2

        auto discriminant = t * t - 4 * d;
        auto sqrt_discriminant = std::sqrt(discriminant);
        // we push the solution ever so slightly closer to the next int because we want to beat d, not come out equal to it
        ULL lx = std::ceil((t - sqrt_discriminant) / 2 + 0.000001);
        ULL ux = std::floor((t + sqrt_discriminant) / 2 - 0.000001);
        auto r = ux - lx + 1;
        return r;
    };

    auto factor = number_of_ways(long_time, long_distance);

    return factor;
}

int main()
{
    auto day6_parsed = parse_day6("day6_input.txt");
    auto factor = process_day6_part1(day6_parsed);
    std::cout << "Day 6 part 1 factor is: " << factor << "\n";
    auto factor2 = process_day6_part2(day6_parsed);
    std::cout << "Day 6 part 2 factor is: " << factor2 << "\n";
}