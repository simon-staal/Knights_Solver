#pragma once

#include <cstdint>

#include "common.hpp"

namespace Helpers {
// Returns fewest moves for a knight to reach a destination assuming an infinitely large, empty chessboard.
// See https://stackoverflow.com/a/41704071 for more details
uint32_t MinimumMovesToDestination(const BoardPos& curr, const BoardPos& dest);

constexpr unsigned floorlog2(unsigned x)
{
    return x == 1 ? 0 : 1+floorlog2(x >> 1);
}

constexpr unsigned ceillog2(unsigned x)
{
    return x == 1 ? 0 : Helpers::floorlog2(x - 1) + 1;
}
}