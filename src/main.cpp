#include <iostream>

#include "board.hpp"

int main()
{
    constexpr std::array<std::array<char, 2>, 2> initialState {{
        {' ', 'B'},
        {'R', 'X'}
    }};

    Board<2,2> board(initialState);
    std::cout << board << std::endl;
}