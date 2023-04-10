#pragma once

#include <array>

#include "common.hpp"

// Returns fewest moves for a knight to reach a destination assuming an infinitely large, empty chessboard.
// See https://stackoverflow.com/a/41704071 for more details
int MinimumMovesToDestination(const BoardPos& curr, const BoardPos& dest);