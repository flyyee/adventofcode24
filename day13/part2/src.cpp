#include <iostream>
#include <fstream>
#include <vector>
#include <optional>
#include <climits>
#include <numeric>
#include <array>
#include <cmath>
#include <cassert>

using namespace std;

template <size_t N>
class Equation {
    using T = int_fast64_t;
    array<T, N> coefficients_;
public:
    Equation() = default;

    Equation(const T(&list)[N]) {
        for (size_t i = 0; i < N; i++) {
            coefficients_[i] = list[i];
        }
    }

    template <size_t X>
        requires (X < N)
    T get() const {
        return coefficients_[X];
    }

    Equation& operator+= (const Equation<N>& other) {
        for (size_t i = 0; i < N; i++) {
            coefficients_[i] += other.coefficients_[i];
        }
        return *this;
    }

    Equation& operator-= (const Equation<N>& other) {
        for (size_t i = 0; i < N; i++) {
            coefficients_[i] -= other.coefficients_[i];
        }
        return *this;
    }

    Equation& operator*= (const T& x) {
        for (size_t i = 0; i < N; i++) {
            coefficients_[i] *= x;
        }
        return *this;
    }

    friend Equation operator+ (Equation lhs, const Equation& rhs) {
        lhs += rhs;
        return lhs;
    }

    friend Equation operator- (Equation lhs, const Equation& rhs) {
        lhs -= rhs;
        return lhs;
    }

    friend Equation operator* (Equation lhs, const T& rhs) {
        lhs *= rhs;
        return lhs;
    }

    bool equals(const Equation<N>& other) const {
        for (size_t i = 0; i < N; i++) {
            if (coefficients_[i] != other.coefficients_[i]) {
                return false;
            }
        }
        return true;
    }
};

struct Game {
    pair<unsigned int, unsigned int> button_a, button_b, prize;
};

struct EquationSet {
    Equation<3> a, b;
};

vector<EquationSet> parse() {
    ifstream file("../input");
    string line;
    vector<EquationSet> equations;
    do {
        Game game;
        const auto parse_button = [&file](pair<unsigned int, unsigned int>& button, const char delim = '+') -> void {
            string line;
            getline(file, line);
            auto pos_1 = line.find(delim) + 1, pos_2 = line.find(',', pos_1);
            button.first = static_cast<unsigned int>(stoi(line.substr(pos_1, pos_2 - pos_1)));
            pos_1 = line.find(delim, pos_2) + 1, pos_2 = line.length();
            button.second = static_cast<unsigned int>(stoi(line.substr(pos_1, pos_2 - pos_1)));
            };
        parse_button(game.button_a);
        parse_button(game.button_b);
        parse_button(game.prize, '=');

        constexpr long offset = 10000000000000;
        equations.emplace_back(EquationSet{
            .a = Equation<3>({game.button_a.first, game.button_b.first, game.prize.first + offset}),
            .b = Equation<3>({game.button_a.second, game.button_b.second, game.prize.second + offset}),
            });
    } while (getline(file, line));

    return equations;
}

// Returns {gcd, {a, b}}, where gcd == a * x_ + b * y_
pair<long, Equation<2>> diophantine(long x_, long y_) {
    // implements https://math.stackexchange.com/a/20727 (extended euclidean algorithm)
    Equation<2> x({ 1, 0 }), y({ 0, 1 }); // store combination count of x_ and y_
    const auto eval = [&x_, &y_](Equation<2>& eqn) {
        return eqn.get<0>() * x_ + eqn.get<1>() * y_;
        };

    while (true) {
        auto q = eval(x) / eval(y), r = eval(x) % eval(y);
        if (r == 0) {
            // y is our answer
            break;
        }
        auto z = x - (y * q); // the new combination count
        x = y;
        y = z;
    }

    return { eval(y), y };
}

optional<long> solve_cost(EquationSet& es) {
    // either identical lines (infinite solutions)
    // distinct parallel lines (no solutions)
    // intersecting lines (1 solution - may not be integral)

    uint_fast64_t n, m;
    (void)n, void(m);

    const auto es_copy = es;

    // Make first coeff the same for both equations
    const auto a = es.a.get<0>();
    es.a *= es.b.get<0>();
    es.b *= a;

    if (es.a.equals(es.b)) {
        // identical lines
        assert(false); // turns out the testcase doesn't include any such lines
        cout << "identical lines" << endl;

        const auto a = es_copy.a.get<0>(), b = es_copy.a.get<1>(), c = es_copy.a.get<2>();
        // find integer solutions to the diophatine: ax + by = c
        const auto [gcd, dio] = diophantine(a, b);
        if (c % gcd != 0) {
            // no integer solutions
            return nullopt;
        }

        // calc initial solution
        const auto A = (c / gcd) * dio.get<0>(), B = (c / gcd) * dio.get<1>();
        cout << "Found integer solution: " << A << ',' << B << endl;

        // family of solutions
        // ( x, y ) = ( A - (b / gcd)t , B + (a / gcd)t )
        cout << "Solutions of the form: " << A << " - " << (b / gcd) << "t, " << B << " + " << (a / gcd) << "t" << endl;

        // find range of t for non-negative (x, y)
        // A / (b / gcd) >= t
        // t >= -B / (a / gcd)
        const auto div_down = [](auto x, auto y) -> decltype(x) {
            return floor(static_cast<double>(x) / static_cast<double>(y));
            };
        const auto div_up = [](auto x, auto y) -> decltype(x) {
            return ceil(static_cast<double>(x) / static_cast<double>(y));
            };
        const auto t_max = div_down(A, (b / gcd)), t_min = div_up(-B, (a / gcd)); // incl
        cout << "t_max = " << t_max << ", t_min = " << t_min << endl;
        if (t_max < t_min) {
            // no positive solutions
            return nullopt;
        }

        // finally, find optimal t in range
        const auto t_to_solution = [&A, &B, &a, &b, &gcd](const auto& t) {
            return pair<long, long>{ A - (b / gcd) * t, B + (a / gcd) * t };
            };
        const auto calculate_cost = [](const pair<long, long>& p) {
            return 3 * p.first + p.second;
            };
        // ax + by = c describes a line with negative gradient in the top right quadrant
        // as x increases, the cost of (3x + y) either increases monotonically, or decreases monotonically
        // so, the solution is either the min or the max in the range
        return min(calculate_cost(t_to_solution(t_max)), calculate_cost(t_to_solution(t_min)));
    }
    else if (es.a.get<1>() == es.b.get<1>()) {
        // distinct parallel lines
        return nullopt;
    }
    else {
        // intersecting lines
        es.a -= es.b; // reduce the first coefficient to 0
        auto coeff = es.a.get<1>(), val = es.a.get<2>();
        if (val % coeff != 0) {
            // non-integral
            return nullopt;
        }
        auto y = val / coeff;

        // Sub y back into the equation. p = sum, q = coeff 0
        auto p = es_copy.a.get<2>() - y * es_copy.a.get<1>(), q = es_copy.a.get<0>();
        if (p % q != 0) {
            // non integral
            return nullopt;
        }
        auto x = p / q;
        if (x < 0 || y < 0) {
            // negative solution
            return nullopt;
        }
        return x * 3 + y;
    }
}

int main() {
    auto equations = parse();
    long ans = accumulate(begin(equations), end(equations), 0ll, [](auto sum, auto& equation) {
        return sum + solve_cost(equation).value_or(0);
        });
    cout << ans << endl;
}