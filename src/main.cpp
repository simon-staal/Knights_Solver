#include <iostream>

#include "board.hpp"
#include "solver.hpp"

int main()
{
    // Testing KNIGHTS-E1
    constexpr size_t WIDTH = 5;
    constexpr size_t HEIGHT = 5;
    constexpr std::array<std::array<char, WIDTH>, HEIGHT> initialState {{
        {'R', ' ', 'B', ' ', 'R'},
        {' ', 'Y', 'Y', 'Y', ' '},
        {'B', 'X', 'Y', 'X', 'B'},
        {'X', 'R', 'Y', 'R', 'X'},
        {'Y', 'Y', 'X', 'Y', 'Y'}
    }};

    Board<WIDTH, HEIGHT> board(initialState);
    std::cout << board << std::endl;

    std::unordered_map<Target, std::vector<BoardPos>> targets {{
        {Target::BLUE, {{2, 2}, {1, 3}, {3, 3}}},
        {Target::RED, {{0, 1}, {4, 1}, {0, 4}, {4, 4}}},
    }};

    Solver<WIDTH, HEIGHT> solver(std::move(board), std::move(targets));

    auto solution = solver.GenerateSolution();
    std::cout << solution << std::endl;
}