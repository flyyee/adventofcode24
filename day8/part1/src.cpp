#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <unordered_map>
using namespace std;

typedef pair<int, int> Position;
typedef char Frequency;
int main()
{
    ifstream file("../input");
    unordered_map<Frequency, vector<Position>> antennas;
    set<Position> antinodes;
    string line;
    size_t map_width = 0, map_height = 0;
    while (getline(file, line))
    {
        for (size_t i = 0; i < line.length(); i++)
        {
            bool is_antenna = line[i] != '.';
            if (!is_antenna)
                continue;
            antennas[line[i]].push_back({map_height, i});
        }
        map_height++;
        if (!map_width)
            map_width = line.length();
    }
    const auto is_within_map = [map_width, map_height](Position pos) -> bool
    {
        const auto [row, col] = pos;
        return (row >= 0 && row < static_cast<int>(map_height) && col >= 0 && col < static_cast<int>(map_width));
    };
    for (const auto &[freq, positions] : antennas)
    {
        // iterate pairwise
        for (size_t i = 0; i + 1 < positions.size(); i++)
        {
            for (size_t j = i + 1; j < positions.size(); j++)
            {
                const auto [row_a, col_a] = positions[i];
                const auto [row_b, col_b] = positions[j];
                // calculate antinodes
                Position antinode_left = {row_a - (row_b - row_a), col_a - (col_b - col_a)};
                Position antinode_right = {row_b + (row_b - row_a), col_b + (col_b - col_a)};
                if (is_within_map(antinode_left))
                {
                    antinodes.insert(antinode_left);
                }
                if (is_within_map(antinode_right))
                {
                    antinodes.insert(antinode_right);
                }
            }
        }
    }
    cout << antinodes.size() << endl;
}