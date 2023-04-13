#pragma once

#include <cstdint>
#include <iostream>
#include <unordered_set>

#include "enum_value_map.hpp"

struct BoardPos {
    int8_t x;
    int8_t y;

    BoardPos operator+(const BoardPos& other) const {
        return {static_cast<int8_t>(x + other.x), static_cast<int8_t>(y + other.y)};
    }

    BoardPos operator-(const BoardPos& other) const {
        return {static_cast<int8_t>(x - other.x), static_cast<int8_t>(y - other.y)};
    }

    bool operator==(const BoardPos& other) const {
        return (x == other.x && y == other.y);
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

enum class Target {
    BLUE,
    RED
};

static constexpr auto targetCharMapping = EnumValueMap<Target, char, static_cast<uint32_t>(Target::RED) + 1> {{
    {{Target::BLUE, 'B'},
    {Target::RED, 'R'}}
}};