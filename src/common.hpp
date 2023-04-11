#pragma once

#include <cstdint>
#include <iostream>
#include <unordered_set>

struct BoardPos {
    int8_t x;
    int8_t y;

    BoardPos operator+(const BoardPos& other) const {
        return {static_cast<int8_t>(x + other.x), static_cast<int8_t>(y + other.y)};
    }

    BoardPos operator-(const BoardPos& other) const {
        return {static_cast<int8_t>(x - other.x), static_cast<int8_t>(y - other.y)};
    }
};

std::ostream& operator<<(std::ostream& os, const BoardPos& bp);

static constexpr std::array<BoardPos, 8> knightMoves = {{
    BoardPos{1, 2},
    BoardPos{2, 1},
    BoardPos{-1, 2},
    BoardPos{-2, 1},
    BoardPos{1, -2},
    BoardPos{2, -1},
    BoardPos{-1, -2},
    BoardPos{-2, -1}
}};

struct Move {
    BoardPos start;
    BoardPos end;
};

std::ostream& operator<<(std::ostream& os, const Move& m);