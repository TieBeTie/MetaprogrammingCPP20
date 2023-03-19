#pragma once

#include <type_traits>
#include <cstdint>


template <class Enum, std::size_t MAXN = 512>
	requires std::is_enum_v<Enum>
struct EnumeratorTraits {
    static constexpr std::size_t size() noexcept;
    static constexpr Enum at(std::size_t i) noexcept;
    static constexpr std::string_view nameAt(std::size_t i) noexcept;
};
