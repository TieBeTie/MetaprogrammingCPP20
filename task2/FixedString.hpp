#pragma once

#include <string_view>

template <size_t max_length>
struct FixedString
{
  constexpr FixedString(const char *string, size_t length) : len_(length)
  {
    std::copy(string, string + length, impl_);
  }
  constexpr operator std::string_view() const
  {
    return std::string_view(impl_, len_);
  }

  char impl_[max_length]{0};
  size_t len_;
};

constexpr FixedString<256> operator""_cstr(const char *cstr, size_t len)
{
  return FixedString<256>(cstr, len);
}
