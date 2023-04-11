#pragma once

#include <cstdint>

#include "common.hpp"

namespace Helpers {
// Returns fewest moves for a knight to reach a destination assuming an infinitely large, empty chessboard.
// See https://stackoverflow.com/a/41704071 for more details
uint32_t MinimumMovesToDestination(const BoardPos& curr, const BoardPos& dest);
}