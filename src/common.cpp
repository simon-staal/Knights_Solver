#include "common.hpp"

std::ostream& operator<<(std::ostream& os, const BoardPos& bp) {
    os << static_cast<char>('a' + bp.x) << bp.y + 1;
    return os;
}

std::ostream& operator<<(std::ostream& os, const Move& m) {
    os << m.start << "->" << m.end;
    return os;
}