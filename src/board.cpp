#include "board.hpp"

#include <type_traits>

#include "enum_value_map.hpp"

static constexpr std::array<std::pair<BoardState, char>, static_cast<int>(BoardState::BLOCKED) + 1> boardStateValues {{
    {BoardState::EMPTY, ' '},
    {BoardState::BLUE, 'B'},
    {BoardState::RED, 'R'},
    {BoardState::YELLOW, 'Y'},
    {BoardState::BLOCKED, 'X'}
}};

static constexpr auto boardStateMapping = EnumValueMap<BoardState, char, boardStateValues.size()>{{boardStateValues}};

std::ostream& operator<<(std::ostream& os, BoardState boardState)
{
    os << boardStateMapping.toValue(boardState);
    return os;
}

template<size_t Width, size_t Height>
Board<Width, Height>::Board(const std::array<std::array<char, Width>, Height>& refBoard)
{
    for (int y = 0; y < Height; y++)
    {
        for (int x = 0; x < Width; x++)
        {
            board[y][x] = boardStateMapping.toEnum(refBoard[y][x]);
        }
    }
}

namespace {
constexpr unsigned floorlog2(unsigned x)
{
    return x == 1 ? 0 : 1+floorlog2(x >> 1);
}

constexpr unsigned ceillog2(unsigned x)
{
    return x == 1 ? 0 : floorlog2(x - 1) + 1;
}

bool boardStateMatchesTarget(BoardState boardState, Target target) 
{
    switch(target) {
        case Target::BLUE:
            return boardState == BoardState::BLUE;
        case Target::RED:
            return boardState == BoardState::RED;
    }
    std::cerr << "Unexpected target " << static_cast<std::underlying_type<Target>::type>(target) << std::endl;
    exit(1);
}
}

template<size_t Width, size_t Height>
bool Board<Width, Height>::IsSolved(const std::unordered_map<Target, std::vector<BoardPos>>& targets) const
{
    for (const auto& [target, positions] : targets)
    {
        for (const auto& pos : positions)
        {
            if (!boardStateMatchesTarget(board.at(pos), target))
                return false;
        }
    }
    return true;
}

template<size_t Width, size_t Height>
void Board<Width, Height>::ApplyMove(const Move& move)
{
    if (!IsMoveValid(move, true))
        exit(1);
    
    board[move.end] = board[move.start];
    board[move.start] = BoardState::EMPTY;
}

template<size_t Width, size_t Height>
bool Board<Width, Height>::IsInBounds(const BoardPos& pos) const
{
    return pos.x >= 0 && pos.x < Width && pos.y >= 0 && pos.y < Height;
}

namespace {
bool IsKnight(BoardState state)
{
    return (state == BoardState::BLUE 
            || state == BoardState::RED 
            || state == BoardState::YELLOW);
}
}

template<size_t Width, size_t Height>
bool Board<Width, Height>::IsMoveValid(const Move& move, bool enableLogging) const
{
    if (!IsInBounds(move.start) || !IsInBounds(move.end))
    {
        if (enableLogging)
            std::cerr << "Move " << move << " is not in bounds of the board (" << Height << 'x' << Width << ')' << std::endl;
        return false;
    }

    if (!IsKnight(board.at(move.start)))
    {
        if (enableLogging)
            std::cerr << "Cannot move from " << move.start << " as tile does not contain a knight (" << board.at(move.start) << ')' << std::endl;
        return false;
    }

    if (board.at(move.end) != BoardState::EMPTY)
    {
        if (enableLogging)
            std::cerr << "Cannot move to " << move.end << " as tile is not empty (" << board.at(move.end) << ')' << std::endl;
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

template<size_t Width, size_t Height>
std::vector<Move> Board<Width, Height>::GetPossibleMoves() const
{
    std::vector<Move> moves;
    for (uint8_t y = 0; y < height(); y++)
    {
        for (uint8_t x = 0; x < width(); x++)
        {
            BoardPos start = {x, y};
            if (IsKnight(board[start]))
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

template<size_t Width, size_t Height>
std::ostream& operator<<(std::ostream& os, const Board<Width, Height>& b)
{
    for (const auto& row : b.board)
    {
        os << std::string('-', Width * 2 + 1) << '\n';
        for (BoardState tile : row)
        {
            os << '|' << tile;
        }
        os << "|\n";
    }
    os << std::string('-', Width * 2 + 1);
    return os;
}

namespace std {
    template <uint8_t Width, uint8_t Height>
    struct hash<Board<Width, Height>>
    {
        std::size_t operator()(const Board<Width, Height>& b) const
        {
            size_t boardHash = 0;
            constexpr unsigned boardStateWidth = ceillog2(static_cast<int>(BoardState::BLOCKED) - 1);
            size_t temp = 0;
            int tempBitsUsed = 0;
            for (uint8_t y = 0; y < b.height(); y++) 
            {
                for (uint8_t x = 0; x < b.width(); x++)
                {
                    BoardState s = b.at({y, x});
                    // The blocked board state is not included in the hash since it tiles with this state are constant
                    if (s != BoardState::BLOCKED)
                    {
                        temp += static_cast<int>(s);
                        temp <<= boardStateWidth;
                        tempBitsUsed += boardStateWidth;
                    }

                    constexpr int totalBits = 8 * sizeof(size_t);
                    // Check if there's enough space for the next tile
                    if (tempBitsUsed + boardStateWidth > totalBits) 
                    {
                        boardHash ^= temp;
                        temp = 0;
                        tempBitsUsed = 0;
                    }
                }
            }
            return boardHash;
        }
    };
}