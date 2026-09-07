#ifndef CSV_INC_PARAMETERS_HPP
#define CSV_INC_PARAMETERS_HPP
#include <string>

namespace csv {

template <bool b>
struct first_row_is_header {
  static constexpr bool value = b;
};

template <char ch>
struct delimiter {
  static constexpr char value = ch;
};

template <char ch>
struct quoteCharacter {
  static constexpr char value = ch;
};

struct trim_none {
  static void trim(std::string&) noexcept {}
};

struct trim_whitespace {
  static void trim(std::string& s) {
    auto start = s.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) {
      s.clear();
      return;
    }
    auto end = s.find_last_not_of(" \t\r\n");
    s = s.substr(start, end - start + 1);
  }
};

}  // namespace csv

#endif