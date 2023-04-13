#include <iostream>

#include "board.hpp"
#include "solver.hpp"
#include "puzzles.hpp"

int main()
{
    Solver solver(std::move(Puzzles::Bishop_A1));

    auto solution = solver.GenerateSolution();
    std::cout << solution << std::endl;
}