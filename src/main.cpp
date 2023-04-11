#include <iostream>

#include "board.hpp"
#include "solver.hpp"

int main()
{
    constexpr size_t WIDTH = 3;
    constexpr size_t HEIGHT = 2;
    constexpr std::array<std::array<char, WIDTH>, HEIGHT> initialState {{
        {' ', 'X', ' '},
        {'R', 'X', 'B'}
    }};

    Board<WIDTH, HEIGHT> board(initialState);
    std::cout << board << std::endl;

    std::unordered_map<Target, std::vector<BoardPos>> targets {{
        {Target::BLUE, {{0, 0}}},
        {Target::RED, {{2, 0}}}
    }};

    Solver<WIDTH, HEIGHT> solver(std::move(board), std::move(targets));

    auto solution = solver.GenerateSolution();
    std::cout << solution << std::endl;
}