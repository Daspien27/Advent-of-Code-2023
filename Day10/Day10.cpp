// Day10.cpp : This file contains the 'main' function. Program execution begins and ends there.
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

enum class piping {
    vertical,
    horizontal,
    north_east,
    north_west,
    south_east,
    south_west,
    ground,
};

struct index_pair {
    size_t x;
    size_t y;

    auto operator<=> (const index_pair&) const = default;
};

struct day10_parse {

    std::vector <std::vector<piping>> network;
    index_pair starting_position;


};

auto parse_day10() {

    auto input = lines("day10_input.txt");

    day10_parse parse{};

    for (size_t i = 0; i < input.size(); ++i) {
        const auto& row = input[i];
        parse.network.emplace_back(row.size(), piping::ground);
        for (size_t j = 0; j < row.size(); ++j) {
            
            switch (row[j]) {
            case '|':  parse.network[i][j] = piping::vertical; break;
            case '-':  parse.network[i][j] = piping::horizontal; break;
            case 'L':  parse.network[i][j] = piping::north_east; break;
            case 'J':  parse.network[i][j] = piping::north_west; break;
            case '7':  parse.network[i][j] = piping::south_west; break;
            case 'F':  parse.network[i][j] = piping::south_east; break;
            case '.':  parse.network[i][j] = piping::ground; break;
            case 'S':  
            {
                parse.starting_position = index_pair{ .x = i, .y = j };
                break;
            }
            }
        }
    }

    bool has_north = [&]() {
        auto north_of_start = (parse.starting_position.x > 0) ? parse.network[parse.starting_position.x - 1][parse.starting_position.y] : piping::ground;
        switch (north_of_start) {
        case piping::vertical:
        case piping::south_east:
        case piping::south_west:
            return true;
        default:
            return false;
    }}();

    bool has_south = [&]() {
        auto south_of_start = (parse.starting_position.x + 1 < parse.network.size()) ? parse.network[parse.starting_position.x + 1][parse.starting_position.y] : piping::ground;
        switch (south_of_start) {
        case piping::vertical:
        case piping::north_east:
        case piping::north_west:
            return true;
        default:
            return false;
        }}();

    bool has_west = [&]() {
        auto west_of_start = (parse.starting_position.y > 0) ? parse.network[parse.starting_position.x][parse.starting_position.y - 1] : piping::ground;
        switch (west_of_start) {
        case piping::horizontal:
        case piping::north_east:
        case piping::south_east:
            return true;
        default:
            return false;
    }}();

    bool has_east = [&]() {
        auto east_of_start = (parse.starting_position.y + 1 < parse.network[parse.starting_position.x].size()) ? parse.network[parse.starting_position.x][parse.starting_position.y + 1] : piping::ground;
        switch (east_of_start) {
        case piping::horizontal:
        case piping::north_west:
        case piping::south_west:
            return true;
        default:
            return false;
        }}();
    
    auto start_piping = [&](){
        if ( has_north && !has_east &&  has_south && !has_west) return piping::vertical;
        if (!has_north &&  has_east && !has_south &&  has_west) return piping::horizontal;
        if ( has_north &&  has_east && !has_south && !has_west) return piping::north_east;
        if ( has_north && !has_east && !has_south &&  has_west) return piping::north_west;
        if (!has_north &&  has_east &&  has_south && !has_west) return piping::south_east;
        if (!has_north && !has_east &&  has_south &&  has_west) return piping::south_west;
            
        throw std::runtime_error("Should have at least 2 directions.");
    }();

    parse.network[parse.starting_position.x][parse.starting_position.y] = start_piping;

    return parse;
}

auto process_day10_part1(const day10_parse& parse) {
    
    auto get_neighbours = [&](index_pair pos) -> std::pair<index_pair, index_pair> {
        auto validate = [&](const index_pair& pos) {
            if (0 <= pos.x && pos.x < parse.network.size()
                && 0 <= pos.y && pos.y < parse.network[pos.x].size()) return true;

            
            throw std::runtime_error("Unexpected pos attempting to validate.");
            return false;
        };
                
        auto n = index_pair{ .x = pos.x - 1, .y = pos.y };
        auto s = index_pair{ .x = pos.x + 1, .y = pos.y };
        auto e = index_pair{ .x = pos.x, .y = pos.y + 1 };
        auto w = index_pair{ .x = pos.x, .y = pos.y - 1 };

        switch (parse.network[pos.x][pos.y]) {
        case piping::vertical:
        {
            validate(n);
            validate(s);
            return std::pair{ n,s };
        }
        case piping::horizontal:
        {
            validate(e);
            validate(w);
            return std::pair{ e,w };
        }
        case piping::north_east:
        {
            validate(n);
            validate(e);
            return std::pair{ n,e };
        }
        case piping::north_west:
        {
            validate(n);
            validate(w);
            return std::pair{ n,w };
        }
        case piping::south_west:
        {
            validate(s);
            validate(w);
            return std::pair{ s,w };
        }
        case piping::south_east: 
        {
            validate(s);
            validate(e);
            return std::pair{ s,e };
        }
        default:
            throw std::runtime_error("Unexpected piping type.");
        }
    };

    std::map<index_pair, int> distances;
    std::queue<std::pair<index_pair, int>> positions;
    positions.push(std::pair{ parse.starting_position, 0 });

    while (!positions.empty()) {
        const auto& next = positions.front();
        positions.pop();
  
        auto [entry, was_inserted] = distances.try_emplace(next.first, next.second);
        if (was_inserted){
            auto [pos1, pos2] = get_neighbours(next.first);
            positions.push(std::pair{pos1, entry->second + 1 });
            positions.push(std::pair{pos2, entry->second + 1 });
        }
    }

    auto furthest_space = std::max_element(distances.begin(), distances.end(), [](const auto& lhs, const auto& rhs) {
        return lhs.second < rhs.second;
        });

    return furthest_space->second;
}

auto process_day10_part2(const day10_parse& parse) {

    enum class pixel {
        uncoloured,
        colour_a,
        colour_b,
        wall,
    };

    std::vector<std::vector<pixel>> grid(parse.network.size() * 3, std::vector<pixel>(parse.network[0].size() * 3, pixel::uncoloured));

    for (int i = 0; i < parse.network.size(); ++i) {
        for (int j = 0; j < parse.network[i].size(); ++j) {
            const auto& pipe = parse.network[i][j];
            int i3 = i * 3;
            int j3 = j * 3;
            switch (pipe) {
            case piping::vertical:
                grid[i3+0][j3+1] = pixel::wall;
                grid[i3+1][j3+1] = pixel::wall;
                grid[i3+2][j3+1] = pixel::wall;
                break;
            case piping::horizontal:
                grid[i3+1][j3+0] = pixel::wall;
                grid[i3+1][j3+1] = pixel::wall;
                grid[i3+1][j3+2] = pixel::wall;
                break;
            case piping::north_east:
                grid[i3+0][j3+1] = pixel::wall;
                grid[i3+1][j3+1] = pixel::wall;
                grid[i3+1][j3+2] = pixel::wall;
                break;
            case piping::north_west:
                grid[i3+0][j3+1] = pixel::wall;
                grid[i3+1][j3+1] = pixel::wall;
                grid[i3+1][j3+0] = pixel::wall;
                break;
            case piping::south_east:
                grid[i3+2][j3+1] = pixel::wall;
                grid[i3+1][j3+1] = pixel::wall;
                grid[i3+1][j3+2] = pixel::wall;
                break;
            case piping::south_west:
                grid[i3+2][j3+1] = pixel::wall;
                grid[i3+1][j3+1] = pixel::wall;
                grid[i3+1][j3+0] = pixel::wall;
                break;
            }
        }
    }

    auto floodfill_grid = grid;
    

    auto flood_fill_with_color = [&](pixel colour_to_flood, const auto& starting_spot) {
        std::vector<index_pair> flood{ starting_spot };
        while (!flood.empty()) {
            const auto pos = flood.back();
            flood.pop_back();
            if (pos.x > 0 && floodfill_grid[pos.x - 1][pos.y] == pixel::uncoloured)
            {
                floodfill_grid[pos.x - 1][pos.y] = colour_to_flood;
                flood.push_back(index_pair{ .x = pos.x - 1, .y = pos.y });
            }

            if (pos.x + 1 < floodfill_grid.size() && floodfill_grid[pos.x + 1][pos.y] == pixel::uncoloured)
            {
                floodfill_grid[pos.x + 1][pos.y] = colour_to_flood;
                flood.push_back(index_pair{ .x = pos.x + 1, .y = pos.y });
            }

            if (pos.y > 0 && floodfill_grid[pos.x][pos.y - 1] == pixel::uncoloured)
            {
                floodfill_grid[pos.x][pos.y - 1] = colour_to_flood;
                flood.push_back(index_pair{ .x = pos.x, .y = pos.y - 1 });
            }

            if (pos.y + 1 < floodfill_grid[pos.x].size() && floodfill_grid[pos.x][pos.y + 1] == pixel::uncoloured)
            {
                floodfill_grid[pos.x][pos.y + 1] = colour_to_flood;
                flood.push_back(index_pair{ .x = pos.x, .y = pos.y + 1 });
            }
        }
    };

    auto partition_spot = [&](index_pair pos) {
        index_pair a = index_pair{ .x = pos.x * 3 + 1, .y = pos.y * 3 + 1 };
        index_pair nw{ .x = a.x - 1, .y = a.y - 1 };
        index_pair ne{ .x = a.x - 1, .y = a.y + 1 };
        index_pair se{ .x = a.x + 1, .y = a.y + 1 };
        index_pair sw{ .x = a.x + 1, .y = a.y - 1 };

        switch (parse.network[pos.x][pos.y]) {
        case piping::vertical:   return std::pair{ nw, ne };
        case piping::horizontal: return std::pair{ nw, sw };
        case piping::north_east: return std::pair{ ne, sw };
        case piping::north_west: return std::pair{ nw, se };
        case piping::south_east: return std::pair{ nw, se };
        case piping::south_west: return std::pair{ ne, sw };
        default:
            throw std::runtime_error("Unexpected starting position to flood fill from.");
        }
    };

    const auto [starting_spot1, starting_spot2] = partition_spot(parse.starting_position);
    flood_fill_with_color(pixel::colour_a, starting_spot1);
    flood_fill_with_color(pixel::colour_b, starting_spot2);

    
    auto color_count = [&](pixel color) {
        int count = 0;
        for (size_t i = 0; i < parse.network.size(); ++i) {
            for (size_t j = 0; j < parse.network[i].size(); ++j) {
                if (parse.network[i][j] == piping::ground) {
                     if (floodfill_grid[3 * i + 1][3 * j + 1] == color) ++count;
                    continue;
                }
                
                auto [pos1, pos2] = partition_spot(index_pair{ .x = i, .y = j });
                if (   (floodfill_grid[pos1.x][pos1.y] == color || floodfill_grid[pos1.x][pos1.y] == pixel::uncoloured)
                    && (floodfill_grid[pos2.x][pos2.y] == color || floodfill_grid[pos2.x][pos2.y] == pixel::uncoloured)) {
                    ++count;
                }
            }
        }
        return count;
    };

    for (int i = 0; i < floodfill_grid.size(); ++i) {
        for (int j = 0; j < floodfill_grid[i].size(); ++j) {
            auto outchar = [&](){
                switch (floodfill_grid[i][j]){
                case pixel::wall:       return "\u2588";
                case pixel::uncoloured: return " ";
                case pixel::colour_a:   return "\u2591";
                case pixel::colour_b:   return "\u2593";
                default: return "x";
                }}();
            
             std::cout << outchar;
        }
        std::cout << '\n';
    }

    auto counts = std::pair{ color_count(pixel::colour_a), color_count(pixel::colour_b) };

    return counts;
}

int main()
{
    auto day10_parse = parse_day10();
    auto furthest = process_day10_part1(day10_parse);

    std::cout << "Day 10 part 1 furthest space: " << furthest << std::endl;

    auto enclosed = process_day10_part2(day10_parse);

    std::cout << "Day 10 part 2 furthest space: " << enclosed.first << " or " << enclosed.second << std::endl;

}