#pragma once

#include <utility>
#include <array>
#include <algorithm>
#include <stdexcept>

template <typename Enum, typename Value, size_t Size>
struct EnumValueMap {
    [[nodiscard]] constexpr Value toValue(const Enum& e) const 
    {
        const auto it = std::find_if(data.begin(), data.end(), [&e](const auto& v) { return v.first == e; });
        if (it != data.end()) {
            return it->second;
        } else {
            throw std::range_error("Not Found");
        }
    }

    [[nodiscard]] constexpr Enum toEnum(const Value& value) const 
    {
        const auto it = std::find_if(data.begin(), data.end(), [&value](const auto& v) { return v.second == value; });
        if (it != data.end()) {
            return it->first;
        } else {
            throw std::range_error("Not Found");
        }
    }

    std::array<std::pair<Enum, Value>, Size> data;
};