#include <iostream>
#include <fstream>
#include <vector>
using namespace std;

bool process(const unsigned long target, const vector<unsigned long> &values)
{
    if (values.size() < 2)
    {
        throw invalid_argument("Values array is too small");
    }
    const auto operand_count = values.size() - 1;
    unsigned long permutation = 0; // use bitwise ops
    constexpr auto permutation_var_bitcount = sizeof(permutation) * 8;
    if (operand_count > permutation_var_bitcount)
    {
        throw invalid_argument("Values array is too large");
    }
    const unsigned long max_permutation = (((1 << (operand_count - 1)) - 1) << 1) + 1; // bit manip to avoid overflow when operand_count == permutation_var_bitcount
    for (; permutation <= max_permutation; permutation++)
    {
        unsigned long calc_result = values[0];
        for (unsigned long bit_idx = 0; bit_idx < operand_count; bit_idx++)
        {
            const int operand_type = ((permutation >> bit_idx) & 1);
            if (operand_type)
            {
                // plus
                calc_result += values[bit_idx + 1];
            }
            else
            {
                // times
                calc_result *= values[bit_idx + 1];
            }
        }

        if (calc_result == target)
        {
            return true;
        }
    }
    return false;
}

int main()
{
    ifstream file("../input");
    string line;
    unsigned long ans = 0;
    while (getline(file, line))
    {
        // <target>: <value_0> <value_1> ...
        const auto target_split_idx = line.find(":");
        unsigned long target = stoul(line.substr(0, target_split_idx));
        vector<unsigned long> values;
        size_t value_start_pos = target_split_idx + 2, value_end_pos;
        do
        {
            value_end_pos = line.find(" ", value_start_pos);
            if (value_end_pos == string::npos)
            {
                value_end_pos = line.length();
            }
            values.push_back(stoul(line.substr(value_start_pos, value_end_pos - value_start_pos)));
            value_start_pos = value_end_pos + 1;
        } while (value_end_pos < line.length());
        if (process(target, values))
        {
            ans += target;
        }
    }
    cout << ans << endl;
}