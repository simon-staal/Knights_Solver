#pragma once

#include <unordered_map>
#include <vector>

#include "common.hpp"
#include "board.hpp"

template <size_t Width, size_t Height>
struct Puzzle {
    std::array<std::array<char, Width>, Height> initialState;
    std::unordered_map<Target, std::vector<BoardPos>> targets;
};

namespace Puzzles {

// Solves in 2 iterations (2ms)
// 2 moves: a3->b1, b1->c3
Puzzle<3, 3> Pawn_A1 {
    .initialState{{
        {' ', ' ', ' '},
        {' ', 'X', 'X'},
        {'B', 'X', ' '}
    }},
    .targets{{
        {Target::BLUE, {{2, 2}}}
    }}
};

// Solves in 2 iterations (2ms)
// 2 moves: c1->b3, a3->b1
Puzzle<3, 3> Pawn_C1 {
    .initialState{{
        {' ', ' ', 'R'},
        {'X', 'X', 'X'},
        {'B', ' ', ' '}
    }},
    .targets{{
        {Target::BLUE, {{1, 0}}},
        {Target::RED, {{1, 2}}}
    }}
};

// Solves in 11 iterations (2ms)
// 8 moves: c1->b3, a3->b1, b1->c3, c3->a2, a2->c1, b3->a1, a1->c2, c2->a3
Puzzle<3, 3> Pawn_C2 {
    .initialState{{
        {' ', ' ', 'R'},
        {' ', 'X', ' '},
        {'B', ' ', ' '}
    }},
    .targets{{
        {Target::BLUE, {{2, 0}}},
        {Target::RED, {{0, 2}}}
    }}
};

// Solves in 208 iterations (4ms)
// 8 moves: a1->b3, c1->a2, a3->c2, c3->a4, a2->c3, c3->b1, a4->c3, c3->a2
Puzzle<3, 4> Rook_A1 {
    .initialState{{
        {'B', ' ', 'B'},
        {' ', ' ', ' '},
        {'R', ' ', 'R'},
        {' ', 'X', ' '}
    }},
    .targets{{
        {Target::BLUE, {{1, 0}, {1, 2}}},
        {Target::RED, {{0, 1}, {2, 1}}}
    }}
};

// Solves in 70 iterations (2ms)
// 8 moves: c1->a2, a2->c3, b1->a3, a3->c2, c3->b1, b1->a3, a4->c3, c3->b1
Puzzle<3, 4> Rook_C4 {
    .initialState{{
        {'X', 'R', 'B'},
        {' ', 'X', ' '},
        {' ', 'X', ' '},
        {'R', 'B', 'X'}
    }},
    .targets{{
        {Target::BLUE, {{0, 2}, {1, 3}}},
        {Target::RED, {{1, 0}, {2, 1}}}
    }}
};

// Solves in 5351 iterations (17ms)
// 14 moves: d4->b3, a2->b4, d3->c1, c1->a2, b4->d3, b3->d2, a1->b3, b1->a3, d2->b1, c4->d2, a3->c4, b3->d4, d2->b3, b3->a1
Puzzle<4, 4> Bishop_A1 {
    .initialState{{
        {'R', 'R', ' ', ' '},
        {'R', 'X', 'X', ' '},
        {' ', ' ', 'X', 'B'},
        {' ', ' ', 'B', 'B'}
    }},
    .targets{{
        {Target::BLUE, {{0, 0}, {1, 0}, {0, 1}}},
        {Target::RED, {{3, 2}, {2, 3}, {3, 3}}}
    }}
};

// Solves in 9507 iterations (31ms)
// 21 moves: c2->a3, b2->c4, a4->b2, c3->a2, a3->b1, b1->c3, a1->c2, c2->a3, b3->d4, a3->b1, d4->c2, c2->a3, c3->a4, b1->c3, a3->b1, c4->a3, b2->c4, a4->b2, c3->a4, b1->c3, a3->b1
Puzzle<4, 4> Bishop_D4 {
    .initialState{{
        {'R', ' ', 'X', ' '},
        {' ', 'B', 'R', 'X'},
        {' ', 'R', 'B', 'X'},
        {'B', 'X', ' ', ' '}
    }},
    .targets{{
        {Target::BLUE, {{1, 0}, {0, 1}, {2, 3}}},
        {Target::RED, {{1, 1}, {2, 2}, {0, 3}}}
    }}
};

// Solves in 26532 iterations (176ms)
// 16 moves: b1->a3, c1->d3, d5->c3, c3->b1, d1->c3, c3->d5, b5->c3, a3->b5, c3->d1, a5->b3, b3->c1, a1->b3, b3->a5, c5->b3, d3->c5, b3->a1
Puzzle<4, 5> Queen_A1 {
    .initialState{{
        {'R', 'R', 'R', 'R'},
        {' ', 'X', 'X', 'X'},
        {' ', ' ', ' ', ' '},
        {'X', 'X', 'X', ' '},
        {'B', 'B', 'B', 'B'}
    }},
    .targets{{
        {Target::BLUE, {{0, 0}, {1, 0}, {2, 0}, {3, 0}}},
        {Target::RED, {{0, 4}, {1, 4}, {2, 4}, {3, 4}}}
    }}
};

// Currently too hard to solve (100000+ iterations)
Puzzle<5, 5> King_E1 {
    .initialState{{
        {'R', ' ', 'B', ' ', 'R'},
        {' ', 'Y', 'Y', 'Y', ' '},
        {'B', 'X', 'Y', 'X', 'B'},
        {'X', 'R', 'Y', 'R', 'X'},
        {'Y', 'Y', 'X', 'Y', 'Y'}
    }},
    .targets{{
        {Target::BLUE, {{2, 2}, {1, 3}, {3, 3}}},
        {Target::RED, {{0, 1}, {4, 1}, {0, 4}, {4, 4}}},
    }}
};

}