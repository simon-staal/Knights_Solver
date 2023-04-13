#include <iostream>

#include "board.hpp"
#include "solver.hpp"
#include "puzzles.hpp"

int main()
{
    Solver solver(std::move(Puzzles::King_E1));

    auto solution = solver.GenerateSolution();
    std::cout << solution << std::endl;
}