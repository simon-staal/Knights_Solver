#pragma once

#include <iostream>
#include <cstdint>
#include <array>
#include <vector>
#include <unordered_map>
#include <concepts>

#include "common.hpp"

constexpr size_t MAX_BOARD_SIZE = 100;

enum class __attribute__((packed)) BoardState { // Ensures smallest possible size is used for enum
    EMPTY,
    BLUE,
    RED,
    YELLOW,
    BLOCKED // This state cannot be changed
}; 

std::ostream& operator<<(std::ostream& os, BoardState boardState);

enum class Target {
    BLUE,
    RED
};

template<size_t Width, size_t Height> requires (Width <= MAX_BOARD_SIZE && Height <= MAX_BOARD_SIZE)
class Board {
public:
    Board(const std::array<std::array<char, Width>, Height>& refBoard);

    uint8_t width() const { return Width; }
    uint8_t height() const { return Height; }
    BoardState at(const BoardPos& bp) const { return board[bp]; }

    bool IsSolved(const std::unordered_map<Target, std::vector<BoardPos>>& targets) const;
    void ApplyMove(const Move& move);
    std::vector<Move> GetPossibleMoves() const;

    template<size_t W, size_t H>
    friend std::ostream& operator<<(std::ostream& os, const Board<W, H>& b);

private:
    BoardState& operator[](const BoardPos& bp) { return board[bp.y][bp.x]; }

    bool IsInBounds(const BoardPos& pos) const;
    bool IsMoveValid(const Move& move, bool enableLogging) const;

    std::array<std::array<BoardState, Width>, Height> board;
};

namespace std {
    template <size_t Width, size_t Height>
    struct hash<Board<Width, Height>>;
}