#pragma once

#include <vector>
#include <unordered_map>
#include <iostream>

#include "common.hpp"
#include "board.hpp"

template <size_t Width, size_t Height>
struct Solution {
    Board<Width, Height> board;
    std::vector<Move> moves;
    uint32_t heuristicCost;

    Solution(Board<Width, Height>&& board, const std::unordered_map<Target, std::vector<BoardPos>>& targets)
        : board(board), moves{}, heuristicCost(board.GetHeuristicCost(targets))
    {}

    uint32_t GetTotalCost() const { return NOfMoves() + heuristicCost; }
    uint32_t NOfMoves() const { return static_cast<uint32_t>(moves.size()); }
    bool IsComplete(const std::unordered_map<Target, std::vector<BoardPos>>& targets) { return board.IsSolved(targets); }

    void ApplyMove(Move&& move, const std::unordered_map<Target, std::vector<BoardPos>>& targets);
};

template <size_t W, size_t H>
void Solution<W, H>::ApplyMove(Move&& move, const std::unordered_map<Target, std::vector<BoardPos>>& targets)
{
    uint32_t oldHeuristicCost = board.GetTileHeuristicCost(move.start, targets);
    board.ApplyMove(move);
    uint32_t newHeuristicCost = board.GetTileHeuristicCost(move.end, targets);
    heuristicCost += newHeuristicCost - oldHeuristicCost;
    moves.push_back(move);
}

template <size_t W, size_t H>
std::ostream& operator<<(std::ostream& os, const Solution<W, H>& soln)
{
    os << soln.NOfMoves() << " moves: ";
    for (auto it = soln.moves.begin(); it != soln.moves.end(); it++)
    {
        os << *it;
        if (it != soln.moves.end() - 1)
            os << ", ";
    }
    return os;
}