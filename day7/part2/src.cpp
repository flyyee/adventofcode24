#include <iostream>
#include <fstream>
#include <vector>
#include <ranges>
using namespace std;

template <size_t N, typename Bit = unsigned char>
    requires(N <= 8 * sizeof(Bit))
class SequenceGenerator
{
public:
    using Sequence = vector<Bit>;
    [[nodiscard]] optional<reference_wrapper<const Sequence>> init(size_t length)
    {
        if (length == 0)
        {
            return nullopt;
        }
        this->sequence.assign(length, Bit{0});
        return ref(this->sequence);
    }
    [[nodiscard]] optional<reference_wrapper<const Sequence>> next()
    {
        size_t idx = 0;
        while (idx < this->sequence.size())
        {
            this->sequence[idx] = (this->sequence[idx] + 1) % N;
            if (this->sequence[idx] != 0)
            {
                break;
            }
            idx++;
        }
        return (idx == this->sequence.size()) ? nullopt : optional{ref(this->sequence)};
    }

private:
    Sequence sequence;
};

bool process(const unsigned long target, const vector<unsigned long> &values)
{
    if (values.size() < 2)
    {
        throw invalid_argument("Values array is too small");
    }
    const auto concat = [](unsigned long first, unsigned long second) -> unsigned long
    {
        unsigned long times = 1;
        while (times <= second)
        {
            times *= 10;
        }
        return first * times + second;
    };
    const auto operand_count = values.size() - 1;
    SequenceGenerator<3> generator{};
    for (auto iter = generator.init(operand_count); iter; iter = generator.next())
    {
        const auto &seq = iter->get();
        unsigned long calc_result = values[0];
        for (size_t bit_idx = 0; bit_idx < operand_count; bit_idx++)
        {
            const auto operand_type = seq[bit_idx];
            switch (operand_type)
            {
            case 0:
                calc_result += values[bit_idx + 1];
                break;
            case 1:
                calc_result *= values[bit_idx + 1];
                break;
            case 2:
                calc_result = concat(calc_result, values[bit_idx + 1]);
                break;
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