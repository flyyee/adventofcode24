#include <iostream>
#include <fstream>
#include <vector>
#include <optional>
#include <climits>
#include <numeric>
using namespace std;

struct Game {
    pair<int, int> button_a;
    pair<int, int> button_b;
    pair<int, int> prize;
};

vector<Game> parse() {
    ifstream file("../input");
    string line;
    vector<Game> games;
    do {
        Game game;
        const auto parse_button = [&file](pair<int, int>& button, const char delim = '+') -> void {
            string line;
            getline(file, line);
            auto pos_1 = line.find(delim) + 1, pos_2 = line.find(',', pos_1);
            button.first = stoi(line.substr(pos_1, pos_2 - pos_1));
            pos_1 = line.find(delim, pos_2) + 1, pos_2 = line.length();
            button.second = stoi(line.substr(pos_1, pos_2 - pos_1));
            };
        parse_button(game.button_a);
        parse_button(game.button_b);
        parse_button(game.prize, '=');

        games.push_back(game);
    } while (getline(file, line));

    return games;
}

optional<int> solve_cost(const Game& game) {
    optional<int> cost = nullopt;
    for (int i = 0; i <= 100; i++) {
        for (int j = 0; j <= 100; j++) {
            auto x = game.button_a.first * i + game.button_b.first * j;
            auto y = game.button_a.second * i + game.button_b.second * j;
            if (x == game.prize.first && y == game.prize.second) {
                // working solution
                const int attempt_cost = i * 3 + j;
                cost = min(cost.value_or(INT_MAX), attempt_cost);
            }
        }
    }
    return cost;
}

int main() {
    const auto games = parse();
    int ans = accumulate(begin(games), end(games), 0, [](auto sum, const auto& game) {
        return sum + solve_cost(game).value_or(0);
        });
    cout << ans << endl;
}