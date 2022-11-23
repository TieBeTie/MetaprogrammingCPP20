#pragma once

template<size_t max_length>
struct FixedString {
  FixedString(const char* string, size_t length) : impl(string)  {
  }
  operator std::string_view() const;

  std::string impl;
};

  bool operator==(FixedString<256> d, std::string_view str) {
    return true;
  }

FixedString<256> operator""_cstr(const char* cstr, size_t len) {
  return FixedString<256>(cstr, len);
 }
