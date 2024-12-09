#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

int main()
{
    ifstream file("../input");
    char ch;
    bool is_file = true;
    struct Span
    {
        size_t offset;
        unsigned char size;
    };
    vector<Span> files, spaces;
    size_t offset = 0;
    while (file >> ch)
    {
        const unsigned char sz = ch - '0';
        if (is_file)
        {
            files.push_back({offset, sz});
        }
        else
        {
            spaces.push_back({offset, sz});
        }
        offset += sz;
        is_file = !is_file;
    }

    unsigned long ans = 0;
    const auto consec_sum = [](size_t start, size_t cnt) -> size_t
    {
        // use gauss theorem
        return (start * 2 - 1 + cnt) * cnt / 2;
    };

    for (size_t r = files.size() - 1; r >= 1; r--)
    {
        const auto &file = files[r];
        const auto iter_end = next(begin(spaces), r);
        auto space = find_if(begin(spaces), iter_end, [&file](const auto &space)
                             { return space.size >= file.size; });
        if (space == iter_end)
        {
            // remains in place
            ans += (consec_sum(file.offset, file.size)) * r;
        }
        else
        {
            // we can slot it
            ans += (consec_sum(space->offset, file.size)) * r;
            space->size -= file.size;
            space->offset += file.size;
        }
    }

    cout << ans << endl;
}