#include "helper.hpp"

#include <cmath>
#include <algorithm>

int MinimumMovesToDestination(const BoardPos& curr, const BoardPos& dest)
{
    BoardPos diff = dest - curr;

    // axes symmetry
    int x = abs(diff.x);
    int y = abs(diff.y);

    // diagonal symmetry
    if (x < y) {
        std::swap(x, y);
    }

    // 2 corner cases
    if (x == 1 && y == 0) {
        return 3;
    }
    if (x == 2 && y == 2) {
        return 4;
    }

    int delta = x - y;
    if (y > delta) {
        return delta - 2 * static_cast<int>(static_cast<float>(delta - y) / 3);
    }

    return delta - 2 * static_cast<int>(static_cast<float>(delta - y) / 4);   
}