#pragma once

#include <iostream>
#include <cstdint>
#include <array>
#include <vector>
#include <unordered_map>
#include <concepts>
#include <string>
#include <type_traits>
#include <optional>
#include <algorithm>

#include "common.hpp"
#include "enum_value_map.hpp"
#include "helper.hpp"

static constexpr size_t MAX_BOARD_SIZE = 9;
static constexpr uint32_t TOTAL_HASH_BITS = 8 * sizeof(size_t);  

enum class BoardState : uint8_t { // Ensures smallest possible size is used for enum
    EMPTY,
    BLUE,
    RED,
    YELLOW,
    BLOCKED // This state remains fixed throughout solver runtime
}; 

std::ostream& operator<<(std::ostream& os, BoardState boardState);

template <size_t Width, size_t Height> requires (Width <= MAX_BOARD_SIZE && Height <= MAX_BOARD_SIZE)
class Board {
public:
    Board(const std::array<std::array<char, Width>, Height>& refBoard);

    constexpr int8_t width() const { return Width; }
    constexpr int8_t height() const { return Height; }
    BoardState at(const BoardPos& bp) const { return mBoard[bp.y][bp.x]; }
    const std::array<std::array<BoardState, Width>, Height>& GetBoard() const { return mBoard; }
    bool operator==(const Board<Width, Height>& other) const { return mBoard == other.GetBoard(); }
    
    bool IsSolved(const std::unordered_map<Target, std::vector<BoardPos>>& targets) const;
    uint32_t GetTileHeuristicCost(const BoardPos& tile, const std::unordered_map<Target, std::vector<BoardPos>>& targets) const;
    uint32_t GetHeuristicCost(const std::unordered_map<Target, std::vector<BoardPos>>& targets) const;

    void ApplyMove(const Move& move);
    std::vector<Move> GetPossibleMoves() const;

    static constexpr uint32_t kBoardStateBitWidth = Helpers::ceillog2(static_cast<int>(BoardState::BLOCKED) - 1);  
    static constexpr bool kHashOverflowPossible = kBoardStateBitWidth * Width * Height > TOTAL_HASH_BITS;

    template<size_t W, size_t H>
    friend std::ostream& operator<<(std::ostream& os, const Board<W, H>& b);

private:
    BoardState& operator[](const BoardPos& bp) { return mBoard[bp.y][bp.x]; }

    constexpr bool IsInBounds(const BoardPos& pos) const;
    bool IsMoveValid(const Move& move, bool enableLogging = false) const;

    std::array<std::array<BoardState, Width>, Height> mBoard;
};

// Templated functions must be defined in the same translation unit they are declared, implementation is below

static constexpr auto boardStateMapping = EnumValueMap<BoardState, char, static_cast<uint32_t>(BoardState::BLOCKED) + 1>{{
    {{BoardState::EMPTY, ' '},
    {BoardState::BLUE, 'B'},
    {BoardState::RED, 'R'},
    {BoardState::YELLOW, 'Y'},
    {BoardState::BLOCKED, 'X'}}
}};

std::ostream& operator<<(std::ostream& os, BoardState boardState)
{
    os << boardStateMapping.toValue(boardState);
    return os;
}

template <size_t Width, size_t Height>
Board<Width, Height>::Board(const std::array<std::array<char, Width>, Height>& refBoard)
{
    for (size_t y = 0; y < Height; y++)
    {
        for (size_t x = 0; x < Width; x++)
        {
            mBoard[y][x] = boardStateMapping.toEnum(refBoard[y][x]);
        }
    }
}

namespace {
std::optional<Target> GetCorrespondingTarget(BoardState boardState)
{
    switch (boardState) 
    {
        case BoardState::BLUE:
            return Target::BLUE;
        case BoardState::RED:
            return Target::RED;
        case BoardState::EMPTY:
        case BoardState::YELLOW:
        case BoardState::BLOCKED:
            return {};
    }
    std::cerr << "Unexpected boardState " << static_cast<std::underlying_type<BoardState>::type>(boardState) << std::endl;
    exit(1);
}

bool boardStateMatchesTarget(BoardState boardState, Target target) 
{
    auto boardStateTarget = GetCorrespondingTarget(boardState);
    return boardStateTarget && boardStateTarget.value() == target;
}
}

template <size_t Width, size_t Height>
bool Board<Width, Height>::IsSolved(const std::unordered_map<Target, std::vector<BoardPos>>& targets) const
{
    for (const auto& [target, positions] : targets)
    {
        for (const auto& pos : positions)
        {
            if (!boardStateMatchesTarget(this->at(pos), target))
                return false;
        }
    }
    return true;
}

template <size_t W, size_t H>
uint32_t Board<W, H>::GetTileHeuristicCost(const BoardPos& tile, const std::unordered_map<Target, std::vector<BoardPos>>& targets) const
{
    auto target = GetCorrespondingTarget(this->at(tile));
    if (!target)
        return 0;

    const auto& candidateDestinations = targets.at(target.value());
    auto cmp = [tile](BoardPos end1, BoardPos end2) { return Helpers::MinimumMovesToDestination(tile, end1) < Helpers::MinimumMovesToDestination(tile, end2); };
    auto minIt = std::min_element(candidateDestinations.begin(), candidateDestinations.end(), cmp);
    return Helpers::MinimumMovesToDestination(tile, *minIt);
}

template <size_t W, size_t H>
uint32_t Board<W, H>::GetHeuristicCost(const std::unordered_map<Target, std::vector<BoardPos>>& targets) const
{
    uint32_t minimumMovesToSolve = 0;
    // Minimum moves to get pieces to target
    for (int8_t y = 0; y < height(); y++)
    {
        for (int8_t x = 0; x < width(); x++)
        {
            BoardPos start{x, y};
            minimumMovesToSolve += GetTileHeuristicCost(start, targets);
        }
    }

    // Minimum moves to get pieces out of the way for target
    for (auto& [_, tiles] : targets)
    {
        for (auto tile : tiles)
        {
            minimumMovesToSolve += this->at(tile) == BoardState::YELLOW;
        }
    }

    return minimumMovesToSolve;
}

template <size_t Width, size_t Height>
void Board<Width, Height>::ApplyMove(const Move& move)
{
    if (!IsMoveValid(move, true))
        exit(1);
    
    (*this)[move.end] = (*this)[move.start];
    (*this)[move.start] = BoardState::EMPTY;
}

template <size_t Width, size_t Height>
constexpr bool Board<Width, Height>::IsInBounds(const BoardPos& pos) const
{
    return pos.x >= 0 && pos.x < width() && pos.y >= 0 && pos.y < height();
}

namespace {
bool IsKnight(BoardState state)
{
    return (state == BoardState::BLUE 
            || state == BoardState::RED 
            || state == BoardState::YELLOW);
}
}

template <size_t Width, size_t Height>
bool Board<Width, Height>::IsMoveValid(const Move& move, bool enableLogging) const
{
    if (!IsInBounds(move.start) || !IsInBounds(move.end))
    {
        if (enableLogging)
            std::cerr << "Move " << move << " is not in bounds of the board (" << Height << 'x' << Width << ')' << std::endl;
        return false;
    }

    if (!IsKnight(this->at(move.start)))
    {
        if (enableLogging)
            std::cerr << "Cannot move from " << move.start << " as tile does not contain a knight (" << this->at(move.start) << ')' << std::endl;
        return false;
    }

    if (this->at(move.end) != BoardState::EMPTY)
    {
        if (enableLogging)
            std::cerr << "Cannot move to " << move.end << " as tile is not empty (" << this->at(move.end) << ')' << std::endl;
        return false;
    }

    BoardPos displacement = move.end - move.start;
    const auto it = std::find_if(knightMoves.begin(), knightMoves.end(), [&displacement](const auto& v) { return v == displacement; });

    if (it == knightMoves.end())
    {
        if (enableLogging)
            std::cerr << "Move " << move << " is not a valid knight move" << std::endl;
        return false;
    }

    return true;
}

template <size_t Width, size_t Height>
std::vector<Move> Board<Width, Height>::GetPossibleMoves() const
{
    std::vector<Move> moves;
    for (int8_t y = 0; y < height(); y++)
    {
        for (int8_t x = 0; x < width(); x++)
        {
            BoardPos start = {x, y};
            if (IsKnight(this->at(start)))
            {
                for (BoardPos displacement : knightMoves)
                {
                    Move candidate {start, start + displacement};
                    if (IsMoveValid(candidate))
                        moves.push_back(std::move(candidate));
                }
            }
        }
    }

    return moves;
}

namespace {
std::string GenerateRowSeperator(size_t width)
{
    size_t sepLength = width * 2 + 2;
    std::string rowSeperator;
    rowSeperator.resize(sepLength);
    rowSeperator[0] = ' ';
    for (size_t i = 1; i < sepLength; i++)
    {
        if (i % 2) {
            rowSeperator[i] = '+';
        } else {
            rowSeperator[i] = '-';
        }
    }
    return rowSeperator;
}

std::string GenerateRowHeader(int8_t width)
{
    size_t headerLength = 2 * width + 1;
    std::string rowHeader;
    rowHeader.reserve(headerLength);
    rowHeader.push_back(' ');
    for (int8_t x = 0; x < width; x++)
    {
        rowHeader.push_back(' ');
        rowHeader.push_back(x + 'a');
    }
    return rowHeader;
}
}

template <size_t Width, size_t Height>
std::ostream& operator<<(std::ostream& os, const Board<Width, Height>& b)
{
    static const std::string rowHeader{GenerateRowHeader(b.width())};
    static const std::string rowSeperator{GenerateRowSeperator(Width)};

    os << rowHeader << '\n';

    for (int8_t y = 0; y < b.height(); y++)
    {
        os << rowSeperator << '\n' << y+1;
        for (int8_t x = 0; x < b.width(); x++)
        {
            os << '|' << b.at({x, y});
        }
        os << "|\n";
    }
    os << rowSeperator;
    
    return os;
}

namespace std {
    template <size_t Width, size_t Height>
    struct hash<Board<Width, Height>>
    {
        std::size_t operator()(const Board<Width, Height>& b) const
        {
            size_t boardHash = 0;
            size_t temp = 0;
            int tempBitsUsed = 0;

            for (int8_t y = 0; y < b.height(); y++) 
            {
                for (int8_t x = 0; x < b.width(); x++)
                {
                    BoardState s = b.at({x, y});
                    // The blocked board state is not included in the hash since it tiles with this state are constant
                    if (s != BoardState::BLOCKED)
                    {
                        temp <<= Board<Width, Height>::kBoardStateBitWidth;
                        temp += static_cast<uint32_t>(s);
                        tempBitsUsed += Board<Width, Height>::kBoardStateBitWidth;
                    }

                    // Check if there's enough space for the next tile
                    if constexpr (Board<Width, Height>::kHashOverflowPossible) {
                        if (tempBitsUsed + Board<Width, Height>::kBoardStateBitWidth > TOTAL_HASH_BITS) 
                        {
                            boardHash ^= temp;
                            temp = 0;
                            tempBitsUsed = 0;
                        }
                    }
                }
            }
            return boardHash ^ temp;
        }
    };
}